#include <net/Singleton.H>
#include <vector>
#include <iostream>

int main()
{

  // TODO test later, stub here
  auto pvec = oplib::Singleton<std::vector<int>>::instance();
  auto pvec2 = oplib::Singleton<std::vector<int>>::instance();

  pvec->push_back(1);

  std::cout << pvec2->size() << std::endl;
  return 0;
}
