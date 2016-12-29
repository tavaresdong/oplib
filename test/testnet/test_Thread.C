#include <net/Thread.H>

#include <string>
#include <functional>
#include <stdio.h>

using namespace oplib;

void threadFunc()
{
  printf("tid=%d\n", CurrentThread::tid());
}

void threadFunc2(int x)
{
  printf("tid=%d, x=%d\n", CurrentThread::tid(), x);
}

void threadFunc3()
{
  printf("tid=%d\n", CurrentThread::tid());
  sleep(1);
}

class Foo
{
 public:
  explicit Foo(double x)
    : _x(x)
  {}

  void memberFunc()
  {
    printf("tid=%d, Foo::x_=%f\n", CurrentThread::tid(), _x);
  }

  void memberFunc2(const std::string& text)
  {
    printf("tid=%d, Foo::x_=%f, text=%s\n", CurrentThread::tid(), _x, text.c_str());
  }

 private:
  double _x;
};

int main()
{
  printf("main pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());

  Thread t1(threadFunc);
  t1.start();
  t1.join();

  Thread t2(std::bind(threadFunc2, 42),
                   "thread for free function with argument");
  t2.start();
  t2.join();

  Foo foo(87.53);
  Thread t3(std::bind(&Foo::memberFunc, &foo),
                   "thread for member function without argument");
  t3.start();
  t3.join();

  Thread t4(std::bind(&Foo::memberFunc2, &foo, std::string("yuchend")));
  t4.start();
  t4.join();

  {
    Thread t5(threadFunc3);
    t5.start(); // should be deleted with thread still running(detach)
  }
  sleep(2);
  printf("number of created threads %d\n", Thread::numCreated());
}
