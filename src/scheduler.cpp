// Copyright 2019 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "osfiber.h"  // Must come first. See osfiber_ucontext.h.

#include "marl/scheduler.h"

#include "marl/debug.h"
#include "marl/defer.h"
#include "marl/thread.h"
#include "marl/trace.h"

#if defined(_WIN32)
#include <intrin.h>  // __nop()
#endif

// Enable to trace scheduler events.
#define ENABLE_TRACE_EVENTS 0

#if ENABLE_TRACE_EVENTS
#define TRACE(...) MARL_SCOPED_EVENT(__VA_ARGS__)
#else
#define TRACE(...)
#endif

namespace {

template <typename T>
inline T take(std::queue<T>& queue) {
  auto out = std::move(queue.front());
  queue.pop();
  return out;
}

template <typename T>
inline T take(std::unordered_set<T>& set) {
  auto it = set.begin();
  auto out = std::move(*it);
  set.erase(it);
  return out;
}

inline void nop() {
#if defined(_WIN32)
  __nop();
#else
  __asm__ __volatile__("nop");
#endif
}

}  // anonymous namespace

namespace marl {

////////////////////////////////////////////////////////////////////////////////
// Scheduler
////////////////////////////////////////////////////////////////////////////////
thread_local Scheduler* Scheduler::bound = nullptr;

Scheduler* Scheduler::get() {
  return bound;
}

void Scheduler::bind() {
  MARL_ASSERT(bound == nullptr, "Scheduler already bound");
  bound = this;
  {
    std::unique_lock<std::mutex> lock(singleThreadedWorkerMutex);
    auto worker =
        allocator->make_unique<Worker>(this, Worker::Mode::SingleThreaded, 0);
    worker->start();
    auto tid = std::this_thread::get_id();
    singleThreadedWorkers.emplace(tid, std::move(worker));
  }
}

void Scheduler::unbind() {
  MARL_ASSERT(bound != nullptr, "No scheduler bound");
  Allocator::unique_ptr<Worker> worker;
  {
    std::unique_lock<std::mutex> lock(bound->singleThreadedWorkerMutex);
    auto tid = std::this_thread::get_id();
    auto it = bound->singleThreadedWorkers.find(tid);
    MARL_ASSERT(it != bound->singleThreadedWorkers.end(),
                "singleThreadedWorker not found");
    worker = std::move(it->second);
    bound->singleThreadedWorkers.erase(tid);
  }
  worker->flush();
  worker->stop();
  bound = nullptr;
}

Scheduler::Scheduler(Allocator* allocator /* = Allocator::Default */)
    : allocator(allocator) {
  for (size_t i = 0; i < spinningWorkers.size(); i++) {
    spinningWorkers[i] = -1;
  }
}

Scheduler::~Scheduler() {
  {
    std::unique_lock<std::mutex> lock(singleThreadedWorkerMutex);
    MARL_ASSERT(singleThreadedWorkers.size() == 0,
                "Scheduler still bound on %d threads",
                int(singleThreadedWorkers.size()));
  }
  setWorkerThreadCount(0);
}

void Scheduler::setThreadInitializer(const std::function<void()>& func) {
  std::unique_lock<std::mutex> lock(threadInitFuncMutex);
  threadInitFunc = func;
}

const std::function<void()>& Scheduler::getThreadInitializer() {
  std::unique_lock<std::mutex> lock(threadInitFuncMutex);
  return threadInitFunc;
}

void Scheduler::setWorkerThreadCount(int newCount) {
  MARL_ASSERT(newCount >= 0, "count must be positive");
  if (newCount > int(MaxWorkerThreads)) {
    MARL_WARN(
        "marl::Scheduler::setWorkerThreadCount() called with a count of %d, "
        "which exceeds the maximum of %d. Limiting the number of threads to "
        "%d.",
        newCount, int(MaxWorkerThreads), int(MaxWorkerThreads));
    newCount = MaxWorkerThreads;
  }
  auto oldCount = numWorkerThreads;
  for (int idx = oldCount - 1; idx >= newCount; idx--) {
    workerThreads[idx]->stop();
  }
  for (int idx = oldCount - 1; idx >= newCount; idx--) {
    allocator->destroy(workerThreads[idx]);
  }
  for (int idx = oldCount; idx < newCount; idx++) {
    workerThreads[idx] =
        allocator->create<Worker>(this, Worker::Mode::MultiThreaded, idx);
  }
  numWorkerThreads = newCount;
  for (int idx = oldCount; idx < newCount; idx++) {
    workerThreads[idx]->start();
  }
}

int Scheduler::getWorkerThreadCount() {
  return numWorkerThreads;
}

void Scheduler::enqueue(Task&& task) {
  if (numWorkerThreads > 0) {
    while (true) {
      // Prioritize workers that have recently started spinning.
      auto i = --nextSpinningWorkerIdx % spinningWorkers.size();
      auto idx = spinningWorkers[i].exchange(-1);
      if (idx < 0) {
        // If a spinning worker couldn't be found, round-robin the
        // workers.
        idx = nextEnqueueIndex++ % numWorkerThreads;
      }

      auto worker = workerThreads[idx];
      if (worker->tryLock()) {
        worker->enqueueAndUnlock(std::move(task));
        return;
      }
    }
  } else {
    auto tid = std::this_thread::get_id();
    std::unique_lock<std::mutex> lock(singleThreadedWorkerMutex);
    auto it = singleThreadedWorkers.find(tid);
    MARL_ASSERT(it != singleThreadedWorkers.end(),
                "singleThreadedWorker not found");
    it->second->enqueue(std::move(task));
  }
}

bool Scheduler::stealWork(Worker* thief, uint64_t from, Task& out) {
  if (numWorkerThreads > 0) {
    auto thread = workerThreads[from % numWorkerThreads];
    if (thread != thief) {
      if (thread->dequeue(out)) {
        return true;
      }
    }
  }
  return false;
}

void Scheduler::onBeginSpinning(int workerId) {
  auto idx = nextSpinningWorkerIdx++ % spinningWorkers.size();
  spinningWorkers[idx] = workerId;
}

////////////////////////////////////////////////////////////////////////////////
// Fiber
////////////////////////////////////////////////////////////////////////////////
Scheduler::Fiber::Fiber(Allocator::unique_ptr<OSFiber>&& impl, uint32_t id)
    : id(id), impl(std::move(impl)), worker(Scheduler::Worker::getCurrent()) {
  MARL_ASSERT(worker != nullptr, "No Scheduler::Worker bound");
}

Scheduler::Fiber* Scheduler::Fiber::current() {
  auto worker = Scheduler::Worker::getCurrent();
  return worker != nullptr ? worker->getCurrentFiber() : nullptr;
}

void Scheduler::Fiber::schedule() {
  worker->enqueue(this);
}

void Scheduler::Fiber::yield() {
  MARL_SCOPED_EVENT("YIELD");
  worker->yield(this, nullptr);
}

void Scheduler::Fiber::yield_until_sc(const TimePoint& timeout) {
  MARL_SCOPED_EVENT("YIELD_UNTIL");
  worker->yield(this, &timeout);
}

void Scheduler::Fiber::switchTo(Fiber* to) {
  if (to != this) {
    impl->switchTo(to->impl.get());
  }
}

Allocator::unique_ptr<Scheduler::Fiber> Scheduler::Fiber::create(
    Allocator* allocator,
    uint32_t id,
    size_t stackSize,
    const std::function<void()>& func) {
  return allocator->make_unique<Fiber>(
      OSFiber::createFiber(allocator, stackSize, func), id);
}

Allocator::unique_ptr<Scheduler::Fiber>
Scheduler::Fiber::createFromCurrentThread(Allocator* allocator, uint32_t id) {
  return allocator->make_unique<Fiber>(
      OSFiber::createFiberFromCurrentThread(allocator), id);
}

////////////////////////////////////////////////////////////////////////////////
// Scheduler::WaitingFibers
////////////////////////////////////////////////////////////////////////////////
Scheduler::WaitingFibers::operator bool() const {
  return fibers.size() > 0;
}

Scheduler::Fiber* Scheduler::WaitingFibers::take(const TimePoint& timepoint) {
  if (!*this) {
    return nullptr;
  }
  auto it = timeouts.begin();
  if (timepoint < it->timepoint) {
    return nullptr;
  }
  auto fiber = it->fiber;
  timeouts.erase(it);
  auto deleted = fibers.erase(fiber) != 0;
  (void)deleted;
  MARL_ASSERT(deleted, "WaitingFibers::take() maps out of sync");
  return fiber;
}

Scheduler::TimePoint Scheduler::WaitingFibers::next() const {
  MARL_ASSERT(*this,
              "WaitingFibers::next() called when there' no waiting fibers");
  return timeouts.begin()->timepoint;
}

void Scheduler::WaitingFibers::add(const TimePoint& timepoint, Fiber* fiber) {
  timeouts.emplace(Timeout{timepoint, fiber});
  bool added = fibers.emplace(fiber, timepoint).second;
  (void)added;
  MARL_ASSERT(added, "WaitingFibers::add() fiber already waiting");
}

void Scheduler::WaitingFibers::erase(Fiber* fiber) {
  auto it = fibers.find(fiber);
  if (it != fibers.end()) {
    auto timeout = it->second;
    auto erased = timeouts.erase(Timeout{timeout, fiber}) != 0;
    (void)erased;
    MARL_ASSERT(erased, "WaitingFibers::erase() maps out of sync");
    fibers.erase(it);
  }
}

bool Scheduler::WaitingFibers::Timeout::operator<(const Timeout& o) const {
  if (timepoint != o.timepoint) {
    return timepoint < o.timepoint;
  }
  return fiber < o.fiber;
}

////////////////////////////////////////////////////////////////////////////////
// Scheduler::Worker
////////////////////////////////////////////////////////////////////////////////
thread_local Scheduler::Worker* Scheduler::Worker::current = nullptr;

Scheduler::Worker::Worker(Scheduler* scheduler, Mode mode, uint32_t id)
    : id(id), mode(mode), scheduler(scheduler) {}

void Scheduler::Worker::start() {
  switch (mode) {
    case Mode::MultiThreaded:
      thread = std::thread([=] {
        Thread::setName("Thread<%.2d>", int(id));

        if (auto const& initFunc = scheduler->getThreadInitializer()) {
          initFunc();
        }

        Scheduler::bound = scheduler;
        Worker::current = this;
        mainFiber = Fiber::createFromCurrentThread(scheduler->allocator, 0);
        currentFiber = mainFiber.get();
        run();
        mainFiber.reset();
        Worker::current = nullptr;
      });
      break;

    case Mode::SingleThreaded:
      Worker::current = this;
      mainFiber = Fiber::createFromCurrentThread(scheduler->allocator, 0);
      currentFiber = mainFiber.get();
      break;

    default:
      MARL_ASSERT(false, "Unknown mode: %d", int(mode));
  }
}

void Scheduler::Worker::stop() {
  switch (mode) {
    case Mode::MultiThreaded:
      shutdown = true;
      enqueue([] {});  // Ensure the worker is woken up to notice the shutdown.
      thread.join();
      break;

    case Mode::SingleThreaded:
      Worker::current = nullptr;
      break;

    default:
      MARL_ASSERT(false, "Unknown mode: %d", int(mode));
  }
}

void Scheduler::Worker::yield(
    Fiber* from,
    const std::chrono::system_clock::time_point* timeout) {
  MARL_ASSERT(currentFiber == from,
              "Attempting to call yield from a non-current fiber");

  // Current fiber is yielding as it is blocked.

  std::unique_lock<std::mutex> lock(work.mutex);
  if (timeout != nullptr) {
    work.waiting.add(*timeout, from);
  }

  // First wait until there's something else this worker can do.
  waitForWork(lock);

  if (work.fibers.size() > 0) {
    // There's another fiber that has become unblocked, resume that.
    work.num--;
    auto to = take(work.fibers);
    lock.unlock();
    switchToFiber(to);
  } else if (idleFibers.size() > 0) {
    // There's an old fiber we can reuse, resume that.
    auto to = take(idleFibers);
    lock.unlock();
    switchToFiber(to);
  } else {
    // Tasks to process and no existing fibers to resume. Spawn a new fiber.
    lock.unlock();
    switchToFiber(createWorkerFiber());
  }
}

bool Scheduler::Worker::tryLock() {
  return work.mutex.try_lock();
}

void Scheduler::Worker::enqueue(Fiber* fiber) {
  std::unique_lock<std::mutex> lock(work.mutex);
  auto wasIdle = work.num == 0;
  work.waiting.erase(fiber);
  work.fibers.push(std::move(fiber));
  work.num++;
  lock.unlock();
  if (wasIdle) {
    work.added.notify_one();
  }
}

void Scheduler::Worker::enqueue(Task&& task) {
  work.mutex.lock();
  enqueueAndUnlock(std::move(task));
}

void Scheduler::Worker::enqueueAndUnlock(Task&& task) {
  auto wasIdle = work.num == 0;
  work.tasks.push(std::move(task));
  work.num++;
  work.mutex.unlock();
  if (wasIdle) {
    work.added.notify_one();
  }
}

bool Scheduler::Worker::dequeue(Task& out) {
  if (work.num.load() == 0) {
    return false;
  }
  if (!work.mutex.try_lock()) {
    return false;
  }
  defer(work.mutex.unlock());
  if (work.tasks.size() == 0) {
    return false;
  }
  work.num--;
  out = take(work.tasks);
  return true;
}

void Scheduler::Worker::flush() {
  MARL_ASSERT(mode == Mode::SingleThreaded,
              "flush() can only be used on a single-threaded worker");
  std::unique_lock<std::mutex> lock(work.mutex);
  runUntilIdle(lock);
}

void Scheduler::Worker::run() {
  switch (mode) {
    case Mode::MultiThreaded: {
      MARL_NAME_THREAD("Thread<%.2d> Fiber<%.2d>", int(id),
                       Fiber::current()->id);
      {
        std::unique_lock<std::mutex> lock(work.mutex);
        work.added.wait(
            lock, [this] { return work.num > 0 || work.waiting || shutdown; });
        while (!shutdown || work.num > 0 || numBlockedFibers() > 0U) {
          waitForWork(lock);
          runUntilIdle(lock);
        }
        Worker::current = nullptr;
      }
      switchToFiber(mainFiber.get());
      break;
    }
    case Mode::SingleThreaded:
      while (!shutdown) {
        flush();
        idleFibers.emplace(currentFiber);
        switchToFiber(mainFiber.get());
      }
      break;

    default:
      MARL_ASSERT(false, "Unknown mode: %d", int(mode));
  }
}

_Requires_lock_held_(lock) void Scheduler::Worker::waitForWork(
    std::unique_lock<std::mutex>& lock) {
  MARL_ASSERT(work.num == work.fibers.size() + work.tasks.size(),
              "work.num out of sync");
  if (work.num == 0 && mode == Mode::MultiThreaded) {
    scheduler->onBeginSpinning(id);
    lock.unlock();
    spinForWork();
    lock.lock();
  }

  if (work.waiting) {
    work.added.wait_until(lock, work.waiting.next(), [this] {
      return work.num > 0 || (shutdown && numBlockedFibers() == 0U);
    });
    enqueueFiberTimeouts();
  } else {
    work.added.wait(lock, [this] {
      return work.num > 0 || (shutdown && numBlockedFibers() == 0U);
    });
  }
}

_Requires_lock_held_(lock) void Scheduler::Worker::enqueueFiberTimeouts() {
  auto now = std::chrono::system_clock::now();
  while (auto fiber = work.waiting.take(now)) {
    work.fibers.push(fiber);
    work.num++;
  }
}

void Scheduler::Worker::spinForWork() {
  TRACE("SPIN");
  Task stolen;

  constexpr auto duration = std::chrono::milliseconds(1);
  auto start = std::chrono::high_resolution_clock::now();
  while (std::chrono::high_resolution_clock::now() - start < duration) {
    for (int i = 0; i < 256; i++)  // Empirically picked magic number!
    {
      // clang-format off
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      // clang-format on
      if (work.num > 0) {
        return;
      }
    }

    if (scheduler->stealWork(this, rng(), stolen)) {
      std::unique_lock<std::mutex> lock(work.mutex);
      work.tasks.emplace(std::move(stolen));
      work.num++;
      return;
    }

    std::this_thread::yield();
  }
}

_Requires_lock_held_(lock) void Scheduler::Worker::runUntilIdle(
    std::unique_lock<std::mutex>& lock) {
  MARL_ASSERT(work.num == work.fibers.size() + work.tasks.size(),
              "work.num out of sync");
  while (work.fibers.size() > 0 || work.tasks.size() > 0) {
    // Note: we cannot take and store on the stack more than a single fiber
    // or task at a time, as the Fiber may yield and these items may get
    // held on suspended fiber stack.

    while (work.fibers.size() > 0) {
      work.num--;
      auto fiber = take(work.fibers);
      lock.unlock();

      auto added = idleFibers.emplace(currentFiber).second;
      (void)added;
      MARL_ASSERT(added, "fiber already idle");

      switchToFiber(fiber);
      lock.lock();
    }

    if (work.tasks.size() > 0) {
      work.num--;
      auto task = take(work.tasks);
      lock.unlock();

      // Run the task.
      task();

      // std::function<> can carry arguments with complex destructors.
      // Ensure these are destructed outside of the lock.
      task = Task();

      lock.lock();
    }
  }
}

Scheduler::Fiber* Scheduler::Worker::createWorkerFiber() {
  auto fiberId = static_cast<uint32_t>(workerFibers.size() + 1);
  auto fiber = Fiber::create(scheduler->allocator, fiberId, FiberStackSize,
                             [&] { run(); });
  auto ptr = fiber.get();
  workerFibers.push_back(std::move(fiber));
  return ptr;
}

void Scheduler::Worker::switchToFiber(Fiber* to) {
  MARL_ASSERT(to == mainFiber.get() || idleFibers.count(to) == 0,
              "switching to idle fiber");
  auto from = currentFiber;
  currentFiber = to;
  from->switchTo(to);
}

}  // namespace marl
