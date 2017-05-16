#include "FileParser.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <cassert>
#include <iostream>
#include <sstream>

using namespace opse;

namespace 
{
  void createTable(std::string filename_, WordTable& table_)
  {

  };
}

bool FileParser::buildWordTable(std::string filename_, WordTable& table_)
{
  namespace fs = boost::filesystem;

  fs::path p(filename_);
  if (!exists(p) || !fs::is_regular_file(p)) return false;

  fs::ifstream ifs { p };
  std::ostringstream oss;

  oss << ifs.rdbuf();
  std::string filecontent { oss.str() };

  assert(!filecontent.empty());
  table_.clear();

  createTable(filecontent, table_);
  return true;
}
