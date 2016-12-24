#ifndef OPLIB_NET_MUTEX_H
#define OPLIB_NET_MUTEX_H

#include <pthread.h>
#include <stdlib.h>

#define CHECK_RETURN(ret) ({__typeof__(ret) rc = (ret);\
	                       if(rc == 0) exit(rc); });
namespace oplib 
{
  // Forward Declaration
  class MutexLockGuard;

  class Mutex 
  {
  public:
  	Mutex(const Mutex& mutex_) = delete;
  	Mutex& operator=(const Mutex& mutex_) = delete;

  	Mutex() 
  	{
      // TODO: Let initializtion with type PTHREAD_MUTEX_NORMAL
  	  CHECK_RETURN(pthread_mutex_init(&_mutex, nullptr))    
  	}

    ~Mutex()
    {
      if (_holder != 0)

      pthread_mutex_destroy(&_mutex);
    }

    bool lockedByCurrentThread() const;

    pthread_mutex_t* getRawMutex()
    {
      return &_mutex;
    }

  private:
    void lock();
    void unlock();

    void setHolder();
    void unsetHolder();

    friend class Condition;

    // Used only by condition, to set/unset holder
    // Because Condition used raw pthread_mutex_t
    class CondGuard
    {
    public:
      CondGuard(Mutex& mutex_)
      :_mutex(mutex_)
      { _mutex.setHolder(); }

      ~CondGuard()
      {
        _mutex.unsetHolder();
      }
    private:
      Mutex& _mutex;
    };
  private:
  	pthread_mutex_t _mutex;
  	pid_t _holder = 0;
    friend class MutexLockGuard;
  };

  class MutexLockGuard
  {
  public:
    MutexLockGuard(const MutexLockGuard& mutexLock_) = delete;
    MutexLockGuard& operator=(const MutexLockGuard& mutexLock_) = delete;

    explicit MutexLockGuard(Mutex& mutex_) : _mutex(mutex_)
    {
      _mutex.lock();
    }

    ~MutexLockGuard()
    {
      _mutex.unlock();
    }

  private:
    Mutex& _mutex;
  };

  // Misuse: MutexLockGuard(mutex) will create a temporary object and then destroy, 
  // Unable to protect the area
  #define MutexLockGuard(x) static_assert(false, "Creating temporary MutexLockGuard")
}
#endif
