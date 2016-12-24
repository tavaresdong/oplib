/*
 * CountdownLatch.C
 *
 *  Created on: Dec 24, 2016
 *      Author: yuchend
 */

#include "CountdownLatch.H"

namespace oplib
{
  CountdownLatch::CountdownLatch(int cnt_)
  : _cond(_mutex),
    _count(cnt_)

  {}

  void CountdownLatch::countDown()
  {
    // TODO, use branch prediction here
    // Must lock and then do wait on a condition variable
    MutexLockGuard guard(_mutex);
    if (_count > 0 )
    {
      _count--;
    }
    else
    {
      _cond.notifyAll();
    }
  }

  void CountdownLatch::wait()
  {
    // Spurious wakeup! must use while loop
    MutexLockGuard guard(_mutex);
    while ( _count > 0 )
    {
      _cond.wait();
    }
  }

  CountdownLatch::~CountdownLatch()
  {}
}
