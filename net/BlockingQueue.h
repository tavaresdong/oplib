/*
 * BlockingQueue.H
 *
 *  Created on: Dec 25, 2016
 *      Author: yuchend
 */

#ifndef OPLIB_NET_BLOCKINGQUEUE_H_
#define OPLIB_NET_BLOCKINGQUEUE_H_

#include <net/Condition.h>
#include <net/Mutex.h>
#include <deque>

// TODO chage deque to oplib::deque

namespace oplib
{

template <typename T>
class BlockingQueue
{
public:
  BlockingQueue(const BlockingQueue& rhs_) = delete;
  BlockingQueue& operator = (const BlockingQueue& rhs_) = delete;

  BlockingQueue();
  virtual ~BlockingQueue();

  void enqueue(T&& value_);
  void enqueue(const T& value_);
  T dequeue();

  T take() { return dequeue(); }

  void put(const T& value_) { enqueue(value_); }
  void put(T&& value_) { enqueue(std::move(value_)); }

  size_t size();

private:
  Mutex         _mutex;
  Condition     _notEmpty;
  std::deque<T> _queue;
};

template <typename T>
BlockingQueue<T>::BlockingQueue()
: _notEmpty(_mutex)
{}

template <typename T>
void BlockingQueue<T>::enqueue(T&& value_)
{
  {
    MutexLockGuard guard(_mutex);
    _queue.push_front(std::move(value_));
  }
  _notEmpty.notify();
}

template <typename T>
size_t BlockingQueue<T>::size()
{
  MutexLockGuard guard(_mutex);
  return _queue.size();
}

template <typename T>
void BlockingQueue<T>::enqueue(const T& value_)
{
  {
    MutexLockGuard guard(_mutex);
    _queue.push_front(value_);
  }

  // Nofity without holding a Lock is better
  // Because when the waiting thread received the signal, it will
  // Be more likely to grab the lock immediately
  _notEmpty.notify();
}

template <typename T>
T BlockingQueue<T>::dequeue()
{
  MutexLockGuard guard(_mutex);
  while (_queue.empty())
  {
    _notEmpty.wait();
  }
  // assert(!_queue.empty())
  // In case of spurious wakeup
  T back{std::move(_queue.back())};
  _queue.pop_back();
  return back;
}

template <typename T>
BlockingQueue<T>::~BlockingQueue()
{}

}
#endif /* NET_BLOCKINGQUEUE_H_ */
