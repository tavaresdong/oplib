#include "FileParser.h"

#include <iostream>

int main(int argc, char **argv)
{
  opse::FileParser::WordTable wt;
  if (opse::FileParser::buildWordTable(argv[1], wt))
    std::cout << "Build successful" << std::endl;
  else
    std::cout << "Build failed" << std::endl;

  return 0;
}
