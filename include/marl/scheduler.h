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

#ifndef marl_scheduler_h
#define marl_scheduler_h

#include "debug.h"
#include "sal.h"

#include <array>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

namespace marl {

class OSFiber;

// Task is a unit of work for the scheduler.
using Task = std::function<void()>;

// Scheduler asynchronously processes Tasks.
// A scheduler can be bound to one or more threads using the bind() method.
// Once bound to a thread, that thread can call marl::schedule() to enqueue
// work tasks to be executed asynchronously.
// Scheduler are initially constructed in single-threaded mode.
// Call setWorkerThreadCount() to spawn dedicated worker threads.
class Scheduler {
  class Worker;

 public:
  Scheduler();
  ~Scheduler();

  // get() returns the scheduler bound to the current thread.
  static Scheduler* get();

  // bind() binds this scheduler to the current thread.
  // There must be no existing scheduler bound to the thread prior to calling.
  void bind();

  // unbind() unbinds the scheduler currently bound to the current thread.
  // There must be a existing scheduler bound to the thread prior to calling.
  static void unbind();

  // enqueue() queues the task for asynchronous execution.
  void enqueue(Task&& task);

  // setThreadInitializer() sets the worker thread initializer function which
  // will be called for each new worker thread spawned.
  // The initializer will only be called on newly created threads (call
  // setThreadInitializer() before setWorkerThreadCount()).
  void setThreadInitializer(const std::function<void()>& init);

  // getThreadInitializer() returns the thread initializer function set by
  // setThreadInitializer().
  const std::function<void()>& getThreadInitializer();

  // setWorkerThreadCount() adjusts the number of dedicated worker threads.
  // A count of 0 puts the scheduler into single-threaded mode.
  // Note: Currently the number of threads cannot be adjusted once tasks
  // have been enqueued. This restriction may be lifted at a later time.
  void setWorkerThreadCount(int count);

  // getWorkerThreadCount() returns the number of worker threads.
  int getWorkerThreadCount();

  // Fibers expose methods to perform cooperative multitasking and are
  // automatically created by the Scheduler.
  //
  // The currently executing Fiber can be obtained by calling Fiber::current().
  //
  // When execution becomes blocked, yield() can be called to suspend execution
  // of the fiber and start executing other pending work. Once the block has
  // been lifted, schedule() can be called to reschedule the Fiber on the same
  // thread that previously executed it.
  class Fiber {
   public:
    ~Fiber();

    // current() returns the currently executing fiber, or nullptr if called
    // without a bound scheduler.
    static Fiber* current();

    // yield() suspends execution of this Fiber, allowing the thread to work
    // on other tasks.
    // yield() must only be called on the currently executing fiber.
    void yield();

    // schedule() reschedules the suspended Fiber for execution.
    void schedule();

    // id is the thread-unique identifier of the Fiber.
    uint32_t const id;

   private:
    friend class Scheduler;

    Fiber(OSFiber*, uint32_t id);

    // switchTo() switches execution to the given fiber.
    // switchTo() must only be called on the currently executing fiber.
    void switchTo(Fiber*);

    // create() constructs and returns a new fiber with the given identifier,
    // stack size that will executed func when switched to.
    static Fiber* create(uint32_t id,
                         size_t stackSize,
                         const std::function<void()>& func);

    // createFromCurrentThread() constructs and returns a new fiber with the
    // given identifier for the current thread.
    static Fiber* createFromCurrentThread(uint32_t id);

    OSFiber* const impl;
    Worker* const worker;
  };

 private:
  // Stack size in bytes of a new fiber.
  // TODO: Make configurable so the default size can be reduced.
  static constexpr size_t FiberStackSize = 1024 * 1024;

  // Maximum number of worker threads.
  static constexpr size_t MaxWorkerThreads = 256;

  // TODO: Implement a queue that recycles elements to reduce number of
  // heap allocations.
  using TaskQueue = std::queue<Task>;
  using FiberQueue = std::queue<Fiber*>;

  // Workers executes Tasks on a single thread.
  // Once a task is started, it may yield to other tasks on the same Worker.
  // Tasks are always resumed by the same Worker.
  class Worker {
   public:
    enum class Mode {
      // Worker will spawn a background thread to process tasks.
      MultiThreaded,

      // Worker will execute tasks whenever it yields.
      SingleThreaded,
    };

    Worker(Scheduler* scheduler, Mode mode, uint32_t id);

    // start() begins execution of the worker.
    void start();

    // stop() ceases execution of the worker, blocking until all pending
    // tasks have fully finished.
    void stop();

    // yield() suspends execution of the current task, and looks for other
    // tasks to start or continue execution.
    void yield(Fiber* fiber);

    // enqueue(Fiber*) enqueues resuming of a suspended fiber.
    void enqueue(Fiber* fiber);

    // enqueue(Task&&) enqueues a new, unstarted task.
    void enqueue(Task&& task);

    // tryLock() attempts to lock the worker for task enqueing.
    // If the lock was successful then true is returned, and the caller must
    // call enqueueAndUnlock().
    bool tryLock();

    // enqueueAndUnlock() enqueues the task and unlocks the worker.
    // Must only be called after a call to tryLock() which returned true.
    void enqueueAndUnlock(Task&& task);

    // flush() processes all pending tasks before returning.
    void flush();

    // dequeue() attempts to take a Task from the worker. Returns true if
    // a task was taken and assigned to out, otherwise false.
    bool dequeue(Task& out);

    // getCurrent() returns the Worker currently bound to the current
    // thread.
    static inline Worker* getCurrent();

    // getCurrentFiber() returns the Fiber currently being executed.
    inline Fiber* getCurrentFiber() const;

    // Unique identifier of the Worker.
    const uint32_t id;

   private:
    // run() is the task processing function for the worker.
    // If the worker was constructed in Mode::MultiThreaded, run() will
    // continue to process tasks until stop() is called.
    // If the worker was constructed in Mode::SingleThreaded, run() call
    // flush() and return.
    void run();

    // createWorkerFiber() creates a new fiber that when executed calls
    // run().
    Fiber* createWorkerFiber();

    // switchToFiber() switches execution to the given fiber. The fiber
    // must belong to this worker.
    void switchToFiber(Fiber*);

    // runUntilIdle() executes all pending tasks and then returns.
    _Requires_lock_held_(lock) void runUntilIdle(
        std::unique_lock<std::mutex>& lock);

    // waitForWork() blocks until new work is available, potentially calling
    // spinForWork().
    _Requires_lock_held_(lock) void waitForWork(
        std::unique_lock<std::mutex>& lock);

    // spinForWork() attempts to steal work from another Worker, and keeps
    // the thread awake for a short duration. This reduces overheads of
    // frequently putting the thread to sleep and re-waking.
    void spinForWork();

    // Work holds tasks and fibers that are enqueued on the Worker.
    struct Work {
      std::atomic<uint64_t> num = {0};  // tasks.size() + fibers.size()
      TaskQueue tasks;                  // guarded by mutex
      FiberQueue fibers;                // guarded by mutex
      std::condition_variable added;
      std::mutex mutex;
    };

    // https://en.wikipedia.org/wiki/Xorshift
    class FastRnd {
     public:
      inline uint64_t operator()() {
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        return x;
      }

     private:
      uint64_t x = std::chrono::system_clock::now().time_since_epoch().count();
    };

    // The current worker bound to the current thread.
    static thread_local Worker* current;

    Mode const mode;
    Scheduler* const scheduler;
    std::unique_ptr<Fiber> mainFiber;
    Fiber* currentFiber = nullptr;
    std::thread thread;
    Work work;
    FiberQueue idleFibers;  // Fibers that have completed which can be reused.
    std::vector<std::unique_ptr<Fiber>>
        workerFibers;  // All fibers created by this worker.
    FastRnd rng;
    std::atomic<bool> shutdown = {false};
  };

  // stealWork() attempts to steal a task from the worker with the given id.
  // Returns true if a task was stolen and assigned to out, otherwise false.
  bool stealWork(Worker* thief, uint64_t from, Task& out);

  // onBeginSpinning() is called when a Worker calls spinForWork().
  // The scheduler will prioritize this worker for new tasks to try to prevent
  // it going to sleep.
  void onBeginSpinning(int workerId);

  // The scheduler currently bound to the current thread.
  static thread_local Scheduler* bound;

  std::function<void()> threadInitFunc;
  std::mutex threadInitFuncMutex;

  std::array<std::atomic<int>, 8> spinningWorkers;
  std::atomic<unsigned int> nextSpinningWorkerIdx = {0x8000000};

  // TODO: Make this lot thread-safe so setWorkerThreadCount() can be called
  // during execution of tasks.
  std::atomic<unsigned int> nextEnqueueIndex = {0};
  unsigned int numWorkerThreads = 0;
  std::array<Worker*, MaxWorkerThreads> workerThreads;

  std::mutex singleThreadedWorkerMutex;
  std::unordered_map<std::thread::id, std::unique_ptr<Worker>>
      singleThreadedWorkers;
};

Scheduler::Worker* Scheduler::Worker::getCurrent() {
  return Worker::current;
}

Scheduler::Fiber* Scheduler::Worker::getCurrentFiber() const {
  return currentFiber;
}

// schedule() schedules the function f to be asynchronously called with the
// given arguments using the currently bound scheduler.
template <typename Function, typename... Args>
inline void schedule(Function&& f, Args&&... args) {
  MARL_ASSERT_HAS_BOUND_SCHEDULER("marl::schedule");
  auto scheduler = Scheduler::get();
  scheduler->enqueue(
      std::bind(std::forward<Function>(f), std::forward<Args>(args)...));
}

// schedule() schedules the function f to be asynchronously called using the
// currently bound scheduler.
template <typename Function>
inline void schedule(Function&& f) {
  MARL_ASSERT_HAS_BOUND_SCHEDULER("marl::schedule");
  auto scheduler = Scheduler::get();
  scheduler->enqueue(std::forward<Function>(f));
}

}  // namespace marl

#endif  // marl_scheduler_h
