#include "Mutex.H"
#include "Thread.H"

namespace oplib
{
  void Mutex::lock()
  {
    CHECK_RETURN(pthread_mutex_lock(&_mutex))
    _holder = CurrentThread::tid();
  }

  void Mutex::unlock()
  {
    _holder = 0;
    CHECK_RETURN(pthread_mutex_lock(&_mutex))
  }

  bool Mutex::lockedByCurrentThread() const
  {
    return _holder == CurrentThread::tid();
  }

}
