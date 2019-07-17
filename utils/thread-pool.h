/**
 * File: thread-pool.h
 * -------------------
 * This file defines the ThreadPool class, which creates
 * a pool of threads to execute
 */

#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef>     // for size_t
#include <functional>  // for the function template used in the schedule signature
#include <thread>      // for thread
#include <vector>      // for vector
#include <map>         // for map
#include <queue>       // for queue
#include "semaphore.h" // for semaphore
#include <mutex>
#include "condition_variable"

class ThreadPool {
public:

  /**
   * Constructs a ThreadPool of size `num_threads` threads
   * @param num_threads : The number of threads in this thread pool
   */
  explicit ThreadPool(size_t num_threads);

  /**
   * Schedules the provided thunk (which is something that can
   * be invoked as a zero-argument function without a return value)
   * to be executed by one of the ThreadPool's threads as soon as
   * all previously scheduled thunks have been handled.
   * @param thunk : A function with no parameters and no return value
   * that will be scheduled for execution by a worker thread
   */
  void schedule(const std::function<void(void)>& thunk);

  /**
   * Blocks until all tasks in the thread pool have been executed.
   */
  void wait();

  /**
   * waits for all scheduled tasks to complete and then joins all worker / dispatcher threads
   */
  ~ThreadPool();

  /**
   * ThreadPools are the type of thing that shouldn't be cloneable, since it's
   * not clear what it means to clone a ThreadPool (should copies of all outstanding
   * functions to be executed be copied?).
   *
   * In order to prevent cloning, we remove the copy constructor and the
   * assignment operator.  By doing so, the compiler will ensure we never clone
   * a ThreadPool.
   */
  ThreadPool(const ThreadPool &original) = delete;
  ThreadPool &operator=(const ThreadPool &rhs) = delete;

private:
  std::thread dispatcher;                    // dispatcher thread handle
  std::vector<std::thread> workers;  // worker thread handles
  size_t num_threads;                 // number of worker threads

  // For indicating when all threads should exit
  bool should_exit;

  // Queue of scheduled functions
  std::queue<std::function<void(void)>> queue;
  std::mutex m;

  typedef size_t id_t; // Type for storing Worker IDs

  std::vector<bool> free_workers; // which workers are free
  std::vector<std::shared_ptr<semaphore>> worker_go;
  std::vector<std::function<void(void)>> worker_funcs;

  std::condition_variable_any can_dispatch;
  std::condition_variable_any all_complete;

  /**
   * Private Method: dispatcher
   * --------------------------
   * This method will continuously wait until there is something in the
   * thunk queue and then dispatch that thunk to be executed by a worker
   */
  void dispatch();

  bool try_dispatch();

  /**
   * Private Method: worker
   * ---------------------
   * This method first waits until signaled by the workerSignalLock associated
   * with the ID, then it will retrieve the function to execute from the queue
   * and execute the funciton. This method will also indicate that it is available
   * after it has completed it's assigned task and notify the condition_variable_any
   * cv that it has finished.
   * This method is meant to be executed as a thread routine.
   * @param workerID : The ID of the worker that is
   */
  void worker(id_t workerID);
};

#endif
