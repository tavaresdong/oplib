/*
 * Condition.H
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#ifndef OPLIB_NET_CONDITION_H_
#define OPLIB_NET_CONDITION_H_

#include <thread/Mutex.h>

namespace oplib
{
  class Condition {
  public:
    Condition(const Condition&) = delete;
    Condition& operator = (const Condition&) = delete;

    Condition(Mutex& mutex_);

    virtual ~Condition();

    void wait();

    void notify();

    void notifyAll();

    // TODO add timewait function

  private:
    Mutex& _mutex;
    pthread_cond_t _cond;
  };

}

#endif /* NET_CONDITION_H_ */
