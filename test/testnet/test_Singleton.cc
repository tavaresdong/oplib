#include <net/Singleton.h>
#include <net/Thread.h>

#include <stdio.h>
#include <unistd.h>

using namespace oplib;

class Test
{
 public:
  Test(const Test& rhs_) = delete;
  Test& operator=(const Test& rhs_) = delete;

  Test()
  {
    printf("tid=%d, constructing %p\n", CurrentThread::tid(), static_cast<void*>(this));
  }

  ~Test()
  {
    printf("tid=%d, destructing %p %s\n", CurrentThread::tid(), static_cast<void*>(this), _name.c_str());
  }

  const std::string& name() const { return _name; }
  void setName(const std::string& n) { _name = n; }

 private:
  std::string _name;
};

void threadFunc()
{
  printf("tid=%d, %p name=%s\n",
         CurrentThread::tid(),
         static_cast<void*>(Singleton<Test>::instance()),
         Singleton<Test>::instance()->name().c_str());
  Singleton<Test>::instance()->setName("only one, changed");
}

template<unsigned N>
class Destruct
{
 public:
  Destruct()
  {
    printf("construct %d\n", N);
    more();
  }

  ~Destruct()
  {
    printf("destruct %d\n", N);
  }

  void more()
  {
    Singleton<Destruct<N-1> >::instance();
  }
};

template<>
void Destruct<0>::more()
{
}

int main()
{
  Singleton<Test>::instance()->setName("only one");
  Thread t1(threadFunc);
  t1.start();
  t1.join();
  printf("tid=%d, %p name=%s\n",
         CurrentThread::tid(),
         static_cast<void*>(Singleton<Test>::instance()),
         Singleton<Test>::instance()->name().c_str()); // Should be changed

  Singleton<Destruct<100> >::instance();
  printf("ATEXIT_MAX = %ld\n", sysconf(_SC_ATEXIT_MAX));
}
