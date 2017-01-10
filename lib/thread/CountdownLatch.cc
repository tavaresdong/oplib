/*
 * CountdownLatch.C
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#include "CountdownLatch.h"

namespace oplib
{
  CountdownLatch::CountdownLatch(int cnt_)
  : _mutex(),
    _cond(_mutex),
    _count(cnt_)
  {}

  void CountdownLatch::countDown()
  {
    // TODO, use branch prediction here
    // Must lock and then do wait on a condition variable
    MutexLockGuard guard(_mutex);
    --_count;
    if (_count == 0 )
    {
      _cond.notifyAll();
    }

    /* Think about this implementation:
     * if (_count > 0)
     * {
     *   --count;
     * }
     * else
     * {
     *   _cond.notifyAll();
     * }
     * This won't work!
     * because even if the count is decremented to 0, it won't signal
     * and there won't be any more countdowns, so the waiter will never be waken up
     */
  }

  void CountdownLatch::wait()
  {
    // Spurious wakeup! must use while loop
    MutexLockGuard guard(_mutex);
    while ( _count > 0 )
    {
      _cond.wait();
    }

    // Whenever we return, we must have hold the lock!
    // Because when wait returns, the lock is automatically hold
  }

  CountdownLatch::~CountdownLatch()
  {}
}
