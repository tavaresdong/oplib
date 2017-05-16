#include "FileParser.h"

#include <boost/filesystem.hpp>

using namespace opse;

bool FileParser::buildWordTable(std::string filename_, WordTable& table_)
{
  namespace fs = boost::filesystem;

  fs::path p { filename_ };
  if (!exists(filename_) || !fs::is_regular_file(p) || fs::is_empty(p)) 
    return false;

  // TODO read file contents and parse
  return true;
}
