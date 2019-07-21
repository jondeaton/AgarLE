/**
 * Implements a semaphore class
 */

#ifndef _semaphore_
#define _semaphore_

#include <mutex>
#include <condition_variable>

class semaphore {
 public:
  explicit semaphore(int value = 0) : value(value) {}

  void down() {
    std::lock_guard<std::mutex> lg(m);
    cv.wait(m, [this]{ return value > 0; });
    value--;
  }

  void up() {
    std::lock_guard<std::mutex> lg(m);
    value++;
    if (value == 1)
      cv.notify_all();
  }

  semaphore(const semaphore& orig) = delete;
  const semaphore& operator=(const semaphore& rhs) const = delete;

 private:
  int value;
  std::mutex m;
  std::condition_variable_any cv;
};

#endif
