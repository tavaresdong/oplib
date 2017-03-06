#include <net/EventLoop.h>
#include <stdio.h>

oplib::EventLoop* gloop;
int gflag = 0;

void run4()
{
  printf("run4(): pid = %d, flag = %d\n", getpid(), gflag);
  gloop->quit();
}

void run3()
{
  // now gflag is 2
  printf("run3(): pid = %d, flag = %d\n", getpid(), gflag);
  gloop->runAfter(3, run4);
  gflag = 3;
}

void run2()
{
  printf("run2(): pid = %d, flag = %d\n", getpid(), gflag);
  // Run afterwards (after the next wait return)
  // wakeup is called.
  gloop->enqueue(run3);
}

void run1()
{
  gflag = 1;
  printf("run1(): pid = %d, flag = %d\n", getpid(), gflag);

  // In loop thread, run directly
  gloop->runInLoop(run2);
  gflag = 2;
}

int main()
{
  printf("main(): pid = %d, flag = %d\n", getpid(), gflag);

  oplib::EventLoop loop;
  gloop = &loop;

  loop.runAfter(2, run1);
  loop.loop();
  printf("main(): pid = %d, flag = %d\n", getpid(), gflag);
}
