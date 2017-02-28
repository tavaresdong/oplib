#include <net/EventLoop.h>
#include <thread/Thread.h>

// Failure test: create two loops in
// one thread, will definitely fail
int main()
{
  oplib::EventLoop loop;

  // Should fail on this line(abort)
  oplib::EventLoop loop2;

  loop.loop();
  loop2.loop();

  return 0;
}
