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
#include "marl/thread.h"
#include "marl/trace.h"

#if defined(_WIN32)
#include <intrin.h>  // __nop()
#endif

// Enable to trace scheduler events.
#define ENABLE_TRACE_EVENTS 0

// Enable to print verbose debug logging.
#define ENABLE_DEBUG_LOGGING 0

#if ENABLE_TRACE_EVENTS
#define TRACE(...) MARL_SCOPED_EVENT(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if ENABLE_DEBUG_LOGGING
#define DBG_LOG(msg, ...) \
  printf("%.3x " msg "\n", (int)threadID() & 0xfff, __VA_ARGS__)
#else
#define DBG_LOG(msg, ...)
#endif

#define ASSERT_FIBER_STATE(FIBER, STATE)                                   \
  MARL_ASSERT(FIBER->state == STATE,                                       \
              "fiber %d was in state %s, but expected %s", (int)FIBER->id, \
              Fiber::toString(FIBER->state), Fiber::toString(STATE))

namespace {

#if ENABLE_DEBUG_LOGGING
// threadID() returns a uint64_t representing the currently executing thread.
// threadID() is only intended to be used for debugging purposes.
inline uint64_t threadID() {
  auto id = std::this_thread::get_id();
  return std::hash<std::thread::id>()(id);
}
#endif

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
        allocator->make_unique<Worker>(this, Worker::Mode::SingleThreaded, -1);
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
    : allocator(allocator), workerThreads{} {
  for (size_t i = 0; i < spinningWorkers.size(); i++) {
    spinningWorkers[i] = -1;
  }
}

Scheduler::~Scheduler() {
#if MARL_DEBUG_ENABLED
  {
    std::unique_lock<std::mutex> lock(singleThreadedWorkerMutex);
    MARL_ASSERT(singleThreadedWorkers.size() == 0,
                "Scheduler still bound on %d threads",
                int(singleThreadedWorkers.size()));
  }
#endif  // MARL_DEBUG_ENABLED

  // Release all worker threads.
  // This will wait for all in-flight tasks to complete before returning.
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

void Scheduler::Fiber::notify() {
  worker->enqueue(this);
}

void Scheduler::Fiber::wait(Lock& lock, const Predicate& pred) {
  worker->wait(lock, nullptr, pred);
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

const char* Scheduler::Fiber::toString(State state) {
  switch (state) {
    case State::Idle:
      return "Idle";
    case State::Yielded:
      return "Yielded";
    case State::Queued:
      return "Queued";
    case State::Running:
      return "Running";
    case State::Waiting:
      return "Waiting";
  }
  MARL_ASSERT(false, "bad fiber state");
  return "<unknown>";
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

bool Scheduler::WaitingFibers::contains(Fiber* fiber) const {
  return fibers.count(fiber) != 0;
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

_Requires_lock_held_(waitLock)
bool Scheduler::Worker::wait(Fiber::Lock& waitLock,
                             const TimePoint* timeout,
                             const Predicate& pred) {
  DBG_LOG("%d: WAIT(%d)", (int)id, (int)currentFiber->id);
  while (!pred()) {
    // Lock the work mutex to call suspend().
    work.mutex.lock();

    // Unlock the wait mutex with the work mutex lock held.
    // Order is important here as we need to ensure that the fiber is not
    // enqueued (via Fiber::notify()) between the waitLock.unlock() and fiber
    // switch, otherwise the Fiber::notify() call may be ignored and the fiber
    // is never woken.
    waitLock.unlock();

    // suspend the fiber.
    suspend(timeout);

    // Fiber resumed. We don't need the work mutex locked any more.
    work.mutex.unlock();

    // Re-lock to either return due to timeout, or call pred().
    waitLock.lock();

    // Check timeout.
    if (timeout != nullptr && std::chrono::system_clock::now() >= *timeout) {
      return false;
    }

    // Spurious wake up. Spin again.
  }
  return true;
}

_Requires_lock_held_(work.mutex)
void Scheduler::Worker::suspend(
    const std::chrono::system_clock::time_point* timeout) {
  // Current fiber is yielding as it is blocked.
  if (timeout != nullptr) {
    changeFiberState(currentFiber, Fiber::State::Running,
                     Fiber::State::Waiting);
    work.waiting.add(*timeout, currentFiber);
  } else {
    changeFiberState(currentFiber, Fiber::State::Running,
                     Fiber::State::Yielded);
  }

  // First wait until there's something else this worker can do.
  waitForWork();

  if (work.fibers.size() > 0) {
    // There's another fiber that has become unblocked, resume that.
    work.num--;
    auto to = take(work.fibers);
    ASSERT_FIBER_STATE(to, Fiber::State::Queued);
    work.mutex.unlock();
    switchToFiber(to);
    work.mutex.lock();
  } else if (idleFibers.size() > 0) {
    // There's an old fiber we can reuse, resume that.
    auto to = take(idleFibers);
    ASSERT_FIBER_STATE(to, Fiber::State::Idle);
    work.mutex.unlock();
    switchToFiber(to);
    work.mutex.lock();
  } else {
    // Tasks to process and no existing fibers to resume.
    // Spawn a new fiber.
    work.mutex.unlock();
    switchToFiber(createWorkerFiber());
    work.mutex.lock();
  }

  setFiberState(currentFiber, Fiber::State::Running);
}

_When_(return == true, _Acquires_lock_(work.mutex))
bool Scheduler::Worker::tryLock() {
  return work.mutex.try_lock();
}

void Scheduler::Worker::enqueue(Fiber* fiber) {
  std::unique_lock<std::mutex> lock(work.mutex);
  DBG_LOG("%d: ENQUEUE(%d %s)", (int)id, (int)fiber->id,
          Fiber::toString(fiber->state));
  switch (fiber->state) {
    case Fiber::State::Running:
    case Fiber::State::Queued:
      return;  // Nothing to do here - task is already queued or running.
    case Fiber::State::Waiting:
      work.waiting.erase(fiber);
      break;
    case Fiber::State::Idle:
    case Fiber::State::Yielded:
      break;
  }
  bool wasIdle = work.num == 0;
  work.fibers.push(std::move(fiber));
  MARL_ASSERT(!work.waiting.contains(fiber),
              "fiber is unexpectedly in the waiting list");
  setFiberState(fiber, Fiber::State::Queued);
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

_Requires_lock_held_(work.mutex)
_Releases_lock_(work.mutex)
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
  if (work.tasks.size() == 0) {
    work.mutex.unlock();
    return false;
  }
  work.num--;
  out = take(work.tasks);
  work.mutex.unlock();
  return true;
}

void Scheduler::Worker::flush() {
  MARL_ASSERT(mode == Mode::SingleThreaded,
              "flush() can only be used on a single-threaded worker");
  std::unique_lock<std::mutex> lock(work.mutex);
  runUntilIdle();
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
          waitForWork();
          runUntilIdle();
        }
        Worker::current = nullptr;
      }
      switchToFiber(mainFiber.get());
      break;
    }
    case Mode::SingleThreaded:
      ASSERT_FIBER_STATE(currentFiber, Fiber::State::Running);
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

_Requires_lock_held_(work.mutex)
void Scheduler::Worker::waitForWork() {
  MARL_ASSERT(work.num == work.fibers.size() + work.tasks.size(),
              "work.num out of sync");
  if (work.num == 0 && mode == Mode::MultiThreaded) {
    scheduler->onBeginSpinning(id);
    work.mutex.unlock();
    spinForWork();
    work.mutex.lock();
  }

  if (work.waiting) {
    std::unique_lock<std::mutex> lock(work.mutex, std::adopt_lock);
    work.added.wait_until(lock, work.waiting.next(), [this] {
      return work.num > 0 || (shutdown && numBlockedFibers() == 0U);
    });
    lock.release();  // Keep the lock held.
    enqueueFiberTimeouts();
  } else {
    std::unique_lock<std::mutex> lock(work.mutex, std::adopt_lock);
    work.added.wait(lock, [this] {
      return work.num > 0 || (shutdown && numBlockedFibers() == 0U);
    });
    lock.release();  // Keep the lock held.
  }
}

_Requires_lock_held_(work.mutex)
void Scheduler::Worker::enqueueFiberTimeouts() {
  auto now = std::chrono::system_clock::now();
  while (auto fiber = work.waiting.take(now)) {
    changeFiberState(fiber, Fiber::State::Waiting, Fiber::State::Queued);
    DBG_LOG("%d: TIMEOUT(%d)", (int)id, (int)fiber->id);
    work.fibers.push(fiber);
    work.num++;
  }
}

_Requires_lock_held_(work.mutex)
void Scheduler::Worker::changeFiberState(Fiber* fiber,
                                         Fiber::State from,
                                         Fiber::State to) const {
  (void)from;  // Unusued parameter when ENABLE_DEBUG_LOGGING is disabled.
  DBG_LOG("%d: CHANGE_FIBER_STATE(%d %s -> %s)", (int)id, (int)fiber->id,
          Fiber::toString(from), Fiber::toString(to));
  ASSERT_FIBER_STATE(fiber, from);
  fiber->state = to;
}

_Requires_lock_held_(work.mutex)
void Scheduler::Worker::setFiberState(Fiber* fiber, Fiber::State to) const {
  DBG_LOG("%d: SET_FIBER_STATE(%d %s -> %s)", (int)id, (int)fiber->id,
          Fiber::toString(fiber->state), Fiber::toString(to));
  fiber->state = to;
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

_Requires_lock_held_(work.mutex)
void Scheduler::Worker::runUntilIdle() {
  ASSERT_FIBER_STATE(currentFiber, Fiber::State::Running);
  MARL_ASSERT(work.num == work.fibers.size() + work.tasks.size(),
              "work.num out of sync");
  while (work.fibers.size() > 0 || work.tasks.size() > 0) {
    // Note: we cannot take and store on the stack more than a single fiber
    // or task at a time, as the Fiber may yield and these items may get
    // held on suspended fiber stack.

    while (work.fibers.size() > 0) {
      work.num--;
      auto fiber = take(work.fibers);
      // Sanity checks,
      MARL_ASSERT(idleFibers.count(fiber) == 0, "dequeued fiber is idle");
      MARL_ASSERT(fiber != currentFiber, "dequeued fiber is currently running");
      ASSERT_FIBER_STATE(fiber, Fiber::State::Queued);

      changeFiberState(currentFiber, Fiber::State::Running, Fiber::State::Idle);
      work.mutex.unlock();
      {  // unlocked
        auto added = idleFibers.emplace(currentFiber).second;
        (void)added;
        MARL_ASSERT(added, "fiber already idle");

        switchToFiber(fiber);
      }
      work.mutex.lock();
      changeFiberState(currentFiber, Fiber::State::Idle, Fiber::State::Running);
    }

    if (work.tasks.size() > 0) {
      work.num--;
      auto task = take(work.tasks);
      work.mutex.unlock();

      // Run the task.
      task();

      // std::function<> can carry arguments with complex destructors.
      // Ensure these are destructed outside of the lock.
      task = Task();

      work.mutex.lock();
    }
  }
}

Scheduler::Fiber* Scheduler::Worker::createWorkerFiber() {
  auto fiberId = static_cast<uint32_t>(workerFibers.size() + 1);
  DBG_LOG("%d: CREATE(%d)", (int)id, (int)fiberId);
  auto fiber = Fiber::create(scheduler->allocator, fiberId, FiberStackSize,
                             [&] { run(); });
  auto ptr = fiber.get();
  workerFibers.push_back(std::move(fiber));
  return ptr;
}

void Scheduler::Worker::switchToFiber(Fiber* to) {
  DBG_LOG("%d: SWITCH(%d -> %d)", (int)id, (int)currentFiber->id, (int)to->id);
  MARL_ASSERT(to == mainFiber.get() || idleFibers.count(to) == 0,
              "switching to idle fiber");
  auto from = currentFiber;
  currentFiber = to;
  from->switchTo(to);
}

}  // namespace marl
