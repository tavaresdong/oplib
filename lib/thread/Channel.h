/*
 * Channel.H
 *
 *  Created on: Dec 25, 2016
 *      Author: yuchend
 * An implementation of the Golang's Channel
 */

#ifndef OPLIB_NET_CHANNEL_H
#define OPLIB_NET_CHANNEL_H

#include <thread/Condition.h>
#include <thread/Mutex.h>
#include <deque>

namespace oplib
{

template <typename T>
class Channel
{
public:
  Channel(const Channel& rhs_) = delete;
  Channel& operator = (const Channel& rhs_) = delete;

  Channel();
  virtual ~Channel();

  void enqueue(T&& value_);
  void enqueue(const T& value_);
  T dequeue();

  T take() { return dequeue(); }

  void put(const T& value_) { enqueue(value_); }
  void put(T&& value_) { enqueue(std::move(value_)); }

  Channel& operator << (const T& value_)
  {
    put(value_);
    return *this;
  }

  Channel& operator >> (T& sink_)
  {
    sink_ = take();
    return *this;
  }

  size_t size();

private:
  Mutex         _mutex;
  Condition     _notEmpty;
  std::deque<T> _queue;
};

template <typename T>
Channel<T>::Channel()
: _notEmpty(_mutex)
{}

template <typename T>
void Channel<T>::enqueue(T&& value_)
{
  {
    MutexLockGuard guard(_mutex);
    _queue.push_front(std::move(value_));
  }
  _notEmpty.notify();
}

template <typename T>
size_t Channel<T>::size()
{
  MutexLockGuard guard(_mutex);
  return _queue.size();
}

template <typename T>
void Channel<T>::enqueue(const T& value_)
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
T Channel<T>::dequeue()
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
Channel<T>::~Channel()
{}

}
#endif /* NET_Channel_H_ */
