/**
 * @file thread-pool.cpp
 * @brief implements the ThreadPool class.
 */

#include "thread-pool.h"
#include <iostream>

ThreadPool::ThreadPool(size_t num_threads) :
  workers(num_threads), num_threads(num_threads), should_exit(false) {

  // make a dispatcher thread
  dispatcher = std::thread([this]() {
    dispatch();
  });

  // make all the worker threads
  for (id_t wid = 0; wid < num_threads; wid++) {
    free_workers.emplace_back(false);
    worker_go.emplace_back(std::make_shared<semaphore>());
    worker_funcs.emplace_back();

    workers[wid] = std::thread([this](size_t worker_id) {
      worker(worker_id);
    }, wid);
  }
}

void ThreadPool::schedule(const std::function<void(void)>& thunk) {
  std::lock_guard<std::mutex> lg(m);
  queue.push(thunk); // add function to queue
  can_dispatch.notify_one();
}

void ThreadPool::wait() {
  std::lock_guard<std::mutex> l(m);
  all_complete.wait(m, [this]() {
    if (!queue.empty()) return false;
    for (bool free : free_workers)
      if (!free) return false;
    return true;
  });
}

/**
 * repeatedly waits until there is a job in the queue to schedule
 * and a free worker thread to execute it, then it scheduled the
 * free worker to do that job in the queue
 */
void ThreadPool::dispatch() {
  while (try_dispatch()) { }
}

/*
 * trys to schedule a single job in the thread pool.
 * Returns true if successful, false if the thread pool
 * is being destroyed
 */
bool ThreadPool::try_dispatch() {
  std::lock_guard<std::mutex> l(m);

  can_dispatch.wait(m,[this]() {
    if (should_exit) return true;
    if (queue.empty()) return false;
    for (bool free : free_workers)
      if (free) return true;
    return false;
  });

  if (should_exit) return false;

  // find the thread that is free
  id_t id = -1;
  for (id_t i = 0; i < num_threads; i++) {
    if (free_workers[i]) {
      id = i;
      break;
    }
  }

  // assign it the work and tell it to go
  free_workers[id] = false;
  worker_funcs[id] = queue.front();
  queue.pop();
  worker_go[id]->up();

  return true;
}

/**
 * waits until there is a job assigned, and then does the job
 */
void ThreadPool::worker(const id_t id) {

  while (true) {
    m.lock();
    free_workers[id] = true;
    can_dispatch.notify_one();
    all_complete.notify_all();
    m.unlock();

    worker_go[id]->down(); // wait until there's work to do
    if (should_exit) return;

    auto thunk = worker_funcs[id];
    thunk(); // do the work
  }
}

ThreadPool::~ThreadPool() {

  wait(); // wait for all scheduled tasks to finish

  should_exit = true; // Indicate all threads to exit
  can_dispatch.notify_one(); // signal dispatcher to exit

  // signal worker threads to exit
  for (auto &s : worker_go)
    s->up();

  dispatcher.join(); // wait for the dispatcher to exit

  for (std::thread& t : workers)
    t.join(); // wait for workers to exit
}