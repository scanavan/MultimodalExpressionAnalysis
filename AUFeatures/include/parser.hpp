#ifndef PARSER_HPP
#define PARSER_HPP
#include <atomic>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>
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
using namespace boost::filesystem;

class Parser {
          // Containers
          std::vector<std::vector<AU>> filenameAndAus;
          std::vector<std::vector<std::string>> framesFileName;
          std::vector<ARFFLine> dataARFF;
          const std::string& sourceDirectoryPath;

          // Thread initialization and source file allocation function.
          void ParseData( std::function<void(Parser *, std::vector<std::string>&, size_t)>, std::function<void(Parser *)> );
          void ParseData(std::function<void(Parser *, std::vector<std::string>&, size_t)> );

          // Parsing functions
          void MapAUto3D(std::vector<std::string>&, size_t);
          void create_subsets(std::vector<std::string>&, size_t);
          void getALLdata(std::vector<std::string>&, size_t);
          void cleanEmptyFiles();
          void parseFrames(std::vector<std::string>&, size_t);
          void moveBNDtoOrigin(std::vector<std::string>&, size_t);
          void mapPhyToBND(std::vector<std::string>&, size_t);
          void writeARFFExecuter();


          // Output results functions
          void writeMappedAUtoBND();
          void writeNumberFramesPerExpression();
          void writeARFF_ALL_BND_AU();
          void writeARFF_ALL_BND();
          void writeARFF_Header(ofstream&,bool, bool,bool, bool, std::string);
          void writeARFF_ALL_AU();
          void writeARFF_Separete_Emotions();
          void write_separete(bool, bool,bool,ofstream&, std::string&, std::vector<ARFFLine>&);
          void validate_ARFF_FILES();
          void writeFrameList();
          void writeARFF_ALL();
          void writeARFF_ALL_BND_PHY();
          void writeARFF_ALL_AU_PHY();
          void writeARFF_ALL_PHY();

       public:
        Parser( std::string, std::string);
};

#endif
