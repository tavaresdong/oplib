/*
 * CountdownLatch.H
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#ifndef OPLIB_NET_COUNTDOWNLATCH_H_
#define OPLIB_NET_COUNTDOWNLATCH_H_

#include <thread/Mutex.h>
#include <thread/Condition.h>

namespace oplib
{
  class CountdownLatch {
  public:
    CountdownLatch(const CountdownLatch&) = delete;
    CountdownLatch& operator = (const CountdownLatch&) = delete;

    explicit CountdownLatch(int cnt_);

    virtual ~CountdownLatch();

    void countDown();

    void wait();
  private:
    Mutex _mutex;  // first
    Condition _cond;  // second
    int _count;
  };
}

#endif /* NET_COUNTDOWNLATCH_H_ */
