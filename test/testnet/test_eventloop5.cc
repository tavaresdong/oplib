// Transplated from muduo's testcases
#include <net/EventLoop.h>
#include <net/EventLoopThread.h>
#include <stdio.h>
#include <unistd.h>

void runInThread()
{
  printf("runInThread(): pid = %d, tid = %d\n",
         getpid(), oplib::CurrentThread::tid());
}

int main()
{
  printf("main(): pid = %d, tid = %d\n",
         getpid(), oplib::CurrentThread::tid());

  oplib::EventLoopThread loopThread;
  oplib::EventLoop* loop = loopThread.startLoop();

  printf("EventLoopThread running\n");
  loop->runInLoop(runInThread);
  sleep(1);
  loop->runAfter(2, runInThread);
  sleep(3);
  loop->quit();

  printf("exit main().\n");
}
