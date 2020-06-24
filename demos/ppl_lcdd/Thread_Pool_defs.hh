#ifndef ppl_lcdd_Thread_Pool_hh
#define ppl_lcdd_Thread_Pool_hh 1

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/*
  The classes defined in this file are mostly based on the corresponding
  ones presented by Antony Williams in Chapters 6 and 9 of his book:

    C++ Concurrency in Action - Practical Multithreading
    Manning Editor, February 2012 ISBN 9781933988771

  Note: these are significantly abridged versions, removing all
  functionalities except those directly used in the multi-threaded
  version of the PPL.
*/

template <typename T>
class Threadsafe_Queue {
private:
  mutable std::mutex mut;
  std::queue<T> data_queue;

public:
  Threadsafe_Queue() = default;
  ~Threadsafe_Queue() = default;

  Threadsafe_Queue(Threadsafe_Queue const&) = delete;
  Threadsafe_Queue(Threadsafe_Queue&&) = delete;
  Threadsafe_Queue& operator=(Threadsafe_Queue const&) = delete;
  Threadsafe_Queue& operator=(Threadsafe_Queue&&) = delete;

  bool empty() const;
  void push(T value);
  bool try_pop(T& value);
}; // class Threadsafe_Queue


template <typename Work>
class Thread_Pool {
public:
   explicit Thread_Pool(const unsigned num_tasks = 0);
  ~Thread_Pool();

  Thread_Pool(const Thread_Pool&) = delete;
  Thread_Pool(Thread_Pool&&) = delete;
  Thread_Pool& operator=(const Thread_Pool&) = delete;
  Thread_Pool& operator=(Thread_Pool&&) = delete;

  typedef Work work_type;
  void submit(work_type work);

  void finalize();

private:
  std::atomic_bool done;
  std::vector<std::thread> threads;
  Threadsafe_Queue<work_type> work_queue;
  void worker_thread();
}; // class Thread_Pool

#include "Thread_Pool_templates.hh"

#endif // !defined(ppl_lcdd_Thread_Pool_hh)
