#ifndef PARSER_HPP
#define PARSER_HPP
#include <atomic>
#include <boost/iostreams/device/mapped_file.hpp>
#include <chrono>
#include <boost/filesystem.hpp>
#include <experimental/string_view>
#include "boost/lexical_cast.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "AU.hpp"

using namespace std::experimental;
using namespace boost::filesystem;

class Parser {
          std::vector<std::pair<std::string,std::vector<int>>> totalAU;
          std::vector<std::vector<AU>> filenameAndAus;
          std::vector<std::vector<std::string>> framesFileName;
          const std::string& directoryPath;
          void ParseData();
          void writeResults();
          void writeFrameFiles();
          void map3Data();
          void parseFrames(std::vector<std::string>&, size_t);
          void getLines(std::vector<std::string>&, size_t);
          void MapAUto3D(std::vector<std::string>&, size_t);


       public:
        Parser(const std::string&);
};

#endif
