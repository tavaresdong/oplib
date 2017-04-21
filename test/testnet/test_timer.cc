// transplanted from muduo: muduo/net/tests/TimerQueue_unittest.cc

#include <net/EventLoop.h>
#include <thread/Thread.h>
#include <util/Timestamp.h>
#include <net/TimerManager.h>

#include <functional>

#include <stdio.h>
#include <unistd.h>

int cnt = 0;
oplib::EventLoop* g_loop;
bool cancelled;
oplib::TimerId timerid;

void printTid()
{
  printf("pid = %d, tid = %d\n", getpid(), oplib::CurrentThread::tid());
  printf("now %s\n", oplib::Timestamp::now().toString().c_str());
}

void print(const char* msg)
{
  printf("msg %s %s\n", oplib::Timestamp::now().toFormatString().c_str(), msg);
  if (++cnt == 20)
  {
    g_loop->quit();
  }
}

void guard(const oplib::TimerId& id_)
{
  if (cnt > 10 && !cancelled)
  {
    cancelled = true;
    printf("More than 10 prints, cancel the 3-second timer\n");
    g_loop->cancel(id_);
  }
}

void selfCancel()
{
  printf("selfCancel\n");
  g_loop->cancel(timerid);
}

int main()
{
  printTid();
  oplib::EventLoop loop;
  g_loop = &loop;
  cancelled = false;

  print("main");
  loop.runAfter(1, std::bind(print, "once1"));
  loop.runAfter(1.5, std::bind(print, "once1.5"));
  loop.runAfter(2.5, std::bind(print, "once2.5"));
  loop.runAfter(3.5, std::bind(print, "once3.5"));
  loop.runEvery(2, std::bind(print, "every2"));
  auto id =  loop.runEvery(3, std::bind(print, "every3"));
  loop.runEvery(1, std::bind(guard, id));
  timerid = loop.runEvery(2, selfCancel);

  loop.loop();
  print("main loop exits");
  sleep(1);
}
