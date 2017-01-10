#include <thread/Mutex.h>
#include <thread/Channel.h>
#include <vector>
#include <iostream>

class Foo
{
public:
  Foo(int val_) : _val(val_) {}
  void doit();

private:
  int _val;
};

std::vector<Foo> gvec;
oplib::Mutex mutex;

void post(const Foo& foo)
{
  oplib::MutexLockGuard guard(mutex);
  std::cout << "Hello" << std::endl;
  gvec.push_back(foo);
}

void Foo::doit()
{
  post(Foo(2));
}

void traverse()
{
  oplib::MutexLockGuard guard(mutex);
  for (auto foo : gvec)
  {
    foo.doit();
    std::cout << "Finish one" << std::endl;
  }
}

int main(int argc, char **argv)
{
  oplib::Channel<int> bqint;
  std::cout << "In main" << std::endl;
  gvec.push_back(Foo(2));
  gvec.push_back(Foo(3));
  traverse();
  std::cout << "Finish Traversing!" << std::endl;
  return 0;
}
