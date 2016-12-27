#include <net/Singleton.H>
#include <net/Thread.H>
#include <vector>
#include <iostream>
#include <string>

int main()
{

  // TODO test later, stub here
  auto pvec = oplib::Singleton<std::vector<int>>::instance();
  auto pvec2 = oplib::Singleton<std::vector<int>>::instance();

  pvec->push_back(1);

  int tid = oplib::CurrentThread::tid();
  std::string tidString = oplib::CurrentThread::tidString();

  std::cout << tidString << std::endl;

  std::cout << pvec2->size() << std::endl;
  return 0;
}
