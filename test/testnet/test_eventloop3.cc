#include <sys/timerfd.h>
#include <stdio.h>
#include <strings.h>

#include <net/EventLoop.h>
#include <net/EventDispatcher.h>
#include <util/Timestamp.h>

oplib::EventLoop* gLoop;

void timeout(oplib::Timestamp receiveTime_)
{
  printf("%s Timeout\n", oplib::Timestamp::now().toFormatString().c_str());
  gLoop->quit();
}

int main()
{
  oplib::EventLoop loop;
  gLoop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  oplib::EventDispatcher dispatcher(&loop, timerfd);
  dispatcher.setReadCallback(timeout);

  // Register itself to the EventLoop
  dispatcher.enableReading();
  
  struct itimerspec duration;
  bzero(&duration, sizeof(duration));
  duration.it_value.tv_sec = 5;
  ::timerfd_settime(timerfd, 0, &duration, NULL);

  loop.loop();

  ::close(timerfd);
}
