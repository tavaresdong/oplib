/*
 * Condition.C
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#include "Condition.H"

namespace oplib
{
  Condition::Condition(Mutex& mutex_)
  : _mutex(mutex_)
  {
    pthread_cond_init(&_cond, nullptr);
  }

  void Condition::wait()
  {
    // The first statement will let the Mutex reset
    // holder when wait released the lock
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


