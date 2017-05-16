#ifndef SE_FILEPARSER_H
#define SE_FILEPARSER_H

#include <string>
#include <list>
#include <unordered_map>

namespace opse
{
  struct FileParser 
  {
    struct WordPosition
    {
      std::string _word;
      std::list<uint32_t> _positions;
    };

    using WordTable = std::unordered_map<int64_t, WordPosition>;

    static bool buildWordTable(std::string filename_, WordTable& table_);
  };
}

#endif
