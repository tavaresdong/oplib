#include <net/BlockingQueue.H>
#include <net/CountdownLatch.H>
#include <net/Thread.H>

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>

#include <string>
#include <stdio.h>

using namespace oplib;

class Test
{
 public:
  Test(int numThreads)
    : _latch(numThreads)
  {
    for (int i = 0; i < numThreads; ++i)
    {
      std::string name { "work thread" };
      name += std::to_string(i);
      _threads.push_back(std::make_shared<Thread>(std::bind(&Test::threadFunc, this),
                                                  name));
    }

    for (auto pthr : _threads)
    {
      pthr->start();
    }
  }

  void run(int times)
  {
    // Producer thread generate value and
    // Put them in the queue
    printf("waiting for count down latch\n");
    _latch.wait();
    printf("all threads started\n");
    for (int i = 0; i < times; ++i)
    {
      std::string buf { "hello" };
      buf += std::to_string(i);
      _queue.put(buf);
      printf("tid=%d, put data = %s, size = %zd\n",
             CurrentThread::tid(), buf.data(), _queue.size());
    }
  }

  void joinAll()
  {
    for (size_t i = 0; i < _threads.size(); ++i)
    {
      _queue.put("stop");
    }

    for (auto pthr : _threads)
    {
      pthr->join();
    }
  }

 private:

  void threadFunc()
  {
    printf("tid=%d, %s started\n",
           CurrentThread::tid(),
           CurrentThread::threadName());

    _latch.countDown();
    bool running = true;
    while (running)
    {
      // take() will block if there are no elements
      std::string d(_queue.take());
      printf("tid=%d, get data = %s, size = %zd\n", CurrentThread::tid(), d.c_str(),
            _queue.size());
      running = (d != "stop");
    }

    printf("tid=%d, %s stopped\n",
           CurrentThread::tid(),
           CurrentThread::threadName());
  }

  BlockingQueue<std::string> _queue;
  CountdownLatch _latch;
  std::vector<std::shared_ptr<Thread> > _threads;
};

int main()
{
  printf("Main Thread pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());
  Test t(5);
  t.run(100);
  t.joinAll();

  printf("number of created threads %d\n", Thread::numCreated());
}
