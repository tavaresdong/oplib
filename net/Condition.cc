/*
 * Condition.C
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#include "Condition.h"

namespace oplib
{
  Condition::Condition(Mutex& mutex_)
  : _mutex(mutex_)
  {
    CHECK_RETURN(pthread_cond_init(&_cond, nullptr));
  }

  void Condition::wait()
  {
    // The first statement will let the Mutex reset
    // holder when wait released the lock
    // Use CondGuard not MutexLockGuard because pthread_copnd_wait will release
    // The lock, we cannot release it twice
    Mutex::CondGuard guard(_mutex);
    CHECK_RETURN(pthread_cond_wait(&_cond, _mutex.getRawMutex()))
  }

  void Condition::notify()
  {
    CHECK_RETURN(pthread_cond_signal(&_cond))
  }

  void Condition::notifyAll()
  {
    CHECK_RETURN(pthread_cond_broadcast(&_cond))
  }

  Condition::~Condition()
  {
    pthread_cond_destroy(&_cond);
  }
}


