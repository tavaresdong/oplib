/*
 * Thread.H
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#ifndef OPLIB_NET_THREAD_H_
#define OPLIB_NET_THREAD_H_

#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>

#include <string>
#include <functional>
#include <memory>
#include <atomic>

#include <net/Common.H>

namespace oplib
{
  class Thread
  {
  public:
    Thread(const Thread& rhs_) = delete;
    Thread& operator = (const Thread& rhs_) = delete;

    // Type of a Thread function
    using ThreadFunc = std::function<void()>;

    explicit Thread(const ThreadFunc& func_, const std::string& name_ = std::string());

    explicit Thread(ThreadFunc&& func_, const std::string& name_ = std::string());

    ~Thread();

    void start();

    int join();

    bool started() const { return _started; }

    pid_t tid() const { return *_tid; }

    const std::string& name() const { return _name; }

    static int numCreated() { return _numCreated; }

  private:

    void setDefaultName();

    bool           _started { false };
    bool           _joined { false };
    pthread_t      _pthreadId;
    std::shared_ptr<pid_t> _tid;
    ThreadFunc     _func;
    std::string    _name;

    static std::atomic_long _numCreated;
  };


namespace CurrentThread
{
  extern __thread int tl_tid;
  extern __thread const char* tl_tstatus;

  namespace
  {
    inline void gettid()
    {
      tl_tid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
  }

  inline int tid()
  {
    if (UNLIKELY(tl_tid == 0))
    {
      gettid();
    }
    return tl_tid;
  }

  inline std::string tidString()
  {
    if (UNLIKELY(tl_tid == 0))
    {
      gettid();
    }
    return std::to_string(tl_tid);
  }
}

}

#endif /* NET_THREAD_H_ */
