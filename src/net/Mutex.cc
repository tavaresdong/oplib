#include <net/Mutex.h>
#include <net/Thread.h>

namespace oplib
{
  void Mutex::lock()
  {
    CHECK_RETURN(pthread_mutex_lock(&_mutex));
    setHolder();
  }

  void Mutex::unlock()
  {
    unsetHolder();
    CHECK_RETURN(pthread_mutex_unlock(&_mutex));
  }

  bool Mutex::lockedByCurrentThread() const
  {
    return _holder == CurrentThread::tid();
  }

  void Mutex::setHolder()
  {
    _holder = CurrentThread::tid();
  }

  void Mutex::unsetHolder()
  {
    _holder = 0;
  }
}
