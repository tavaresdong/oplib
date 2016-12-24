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
#include "Thread.H"

namespace oplib
{

  class CurrentThread
  {
  public:
    // TODO a stub
    static pid_t tid()
    {
      return static_cast<pid_t>(::syscall(SYS_gettid));
    }
  };
}




#endif /* NET_THREAD_H_ */
