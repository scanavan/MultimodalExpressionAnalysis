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
          // Containers
          std::vector<std::pair<std::string,std::vector<int>>> totalAU;
          std::vector<std::vector<AU>> filenameAndAus;
          std::vector<std::vector<std::string>> framesFileName;
          const std::string& directoryPath;
          // Thread initialization and source file allocation function.

          void ParseData();
          // Output results functions
          void writeSumResults();
          void writeFrameFiles();
          void writeMappedAUtoBND();

          // Parsing functions
          void parseFrames(std::vector<std::string>&, size_t);
          void getSumAus(std::vector<std::string>&, size_t);
          void MapAUto3D(std::vector<std::string>&, size_t);

          // ICIP functions
          void randomSampling(std::string);
          void SubjectsInARFF(ifstream&, std::pair<int,std::vector<std::string>>&);
          void fillDataContainer(ifstream&, std::vector<std::string>&);


       public:
        Parser(const std::string&);
};

#endif
