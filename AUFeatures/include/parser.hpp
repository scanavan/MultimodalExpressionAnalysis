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
#include "ARFFLine.hpp"
using namespace std::experimental;
using namespace boost::filesystem;

class Parser {
          // Containers
          std::vector<std::pair<std::string,std::vector<int>>> totalAU;
          std::vector<std::vector<AU>> filenameAndAus;
          std::vector<std::vector<std::string>> framesFileName;
          std::vector<ARFFLine> dataARFF;
          const std::string& sourceDirectoryPath;

          // Thread initialization and source file allocation function.
          void ParseData( std::function<void(Parser *, std::vector<std::string>&, size_t)>, std::function<void(Parser *)> );

          // Parsing functions
          void MapAUto3D(std::vector<std::string>&, size_t);
          void create_subsets(std::vector<std::string>&, size_t);
          void getALLdata(std::vector<std::string>&, size_t);
          void cleanEmptyFiles();
          void parseFrames(std::vector<std::string>&, size_t);

          // Output results functions
          void writeMappedAUtoBND();
          void writeNumberFramesPerExpression();
          void writeARFF_ALL_BND_AU();
          void writeARFF_ALL_BND();
          void writeARFF_Header(ofstream&,bool, bool, bool, std::string);
          void writeARFF_ALL_AU();
          void writeARFF_Separete_Emotions();
          void write_separete(bool, bool,ofstream&, std::string&, std::vector<ARFFLine>&);
          void validate_ARFF_FILES();
          void writeFrameList();


       public:
        Parser( std::string, std::string);
};

#endif
