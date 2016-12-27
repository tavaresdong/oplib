/*
 * Thread.H
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#ifndef NET_THREAD_H_
#define NET_THREAD_H_

#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <net/Common.H>

#include <string>

namespace oplib
{

namespace CurrentThread
{
  __thread int thread_local_tid;

  namespace
  {
    inline void gettid()
    {
      thread_local_tid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
  }

  inline int tid()
  {
    if (UNLIKELY(thread_local_tid == 0))
    {
      gettid();
    }
    return thread_local_tid;
  }

  inline std::string tidString()
  {
    if (UNLIKELY(thread_local_tid == 0))
    {
      gettid();
    }
    return std::to_string(thread_local_tid);
  }
}

}




#endif /* NET_THREAD_H_ */
