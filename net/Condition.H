/*
 * Condition.H
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#ifndef NET_CONDITION_H_
#define NET_CONDITION_H_

#include <net/Mutex.H>

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

  private:
    pthread_cond_t _cond;
    Mutex& _mutex;
  };

}

#endif /* NET_CONDITION_H_ */
