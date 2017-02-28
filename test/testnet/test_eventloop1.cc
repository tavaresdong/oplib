#include <net/EventLoop.h>
#include <thread/Thread.h>
#include <stdio.h>

void threadFunc()
{
  printf("Looping in a thread\n");
  oplib::EventLoop loop;
  loop.loop();
}

int main()
{
  oplib::Thread t(threadFunc);
  t.start();

  oplib::EventLoop loop;
  printf("Looing in main thread\n");
  loop.loop();

  t.join();

  return 0;
}
