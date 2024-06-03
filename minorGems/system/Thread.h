#pragma once
#include <atomic>
#include <chrono>
#include <thread>

class Thread {
public:
  Thread() = default;
  virtual ~Thread() = default;
  void start(bool isDetach = false) {
    t_ = std::thread(Thread::worker, this);
    isDetatched_.store(isDetach, std::memory_order_release);
    if (isDetach)
      t_.detach();
  }
  virtual void run() = 0;
  void join() { t_.join(); }
  static void staticSleep(unsigned long inTimeInMilliseconds) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(inTimeInMilliseconds));
  }
  virtual void sleep(unsigned long inTimeInMilliseconds) {
    staticSleep(inTimeInMilliseconds);
  }
  static void worker(Thread *t) {
    t->run();
    // This was like this in the original code - does it imply that this class
    // gets created and then never deallocated, hoping that the thread on finish
    // will clean up? No idea.
    if (t->isDetatched()) {
      delete t;
    }
  }
  // This might be a tick-off, but generally faster.
  bool isDetatched() { return isDetatched_.load(std::memory_order_relaxed); }

private:
  std::thread t_;
  std::atomic_bool isDetatched_;
};
