#include "parser.hpp"

std::mutex vectorLock;
std::atomic<int> readPos{0};

Parser::Parser(const std::string& _dp) : directoryPath(_dp) {
        auto start = std::chrono::system_clock::now();
        ParseData();
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << elapsed_seconds.count() << std::endl;
        writeResults();
};

void Parser::ParseData() {
        std::vector<std::thread> threads;
        std::vector<std::string> files_to_parse;
        for (auto& f : directory_iterator(directoryPath)) {
                files_to_parse.push_back(f.path().string());
        }
        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
                threads.push_back(std::move(std::thread(&Parser::getLines, this,
                                                        std::ref(files_to_parse),
                                                        files_to_parse.size())));
         }
        getLines(files_to_parse, files_to_parse.size());
        for (auto& t : threads) {
                t.join();
        }
};

void Parser::getLines(std::vector<std::string>& filesToParse, size_t vecSize) {
        while (readPos < vecSize) {
              std::string filename;
                boost::iostreams::mapped_file_source mmap;
                {
                        std::lock_guard<std::mutex> lock(vectorLock);
                        if (readPos < vecSize) {
                                mmap = boost::iostreams::mapped_file_source(
                                    filesToParse[readPos]);
                                  filename = filesToParse[readPos];
                        } else {
                                break;
                        }
                        readPos++;
                }
                const char* buffer = mmap.data();
                std::stringstream ss;
                ss << buffer;

                std::vector<int> placeholder;

                std::vector<std::string> split;
                std::string line_dummie;

                // skip header.
                std::getline(ss, line_dummie);

                std::getline(ss, line_dummie);
                boost::split(split, line_dummie, boost::is_any_of(","));

                for(int i = 1; i < split.size(); ++i){
                  try{
                  placeholder.push_back(boost::lexical_cast<int>(split.at(i)));
                  }catch(std::exception &e) {
                    placeholder.push_back(0);
                  }
                }

                for (std::string line; std::getline(ss, line);) {
                        boost::split(split, line, boost::is_any_of(","));
                        for(int i = 1; i < split.size(); ++i){
                          try{
                          placeholder[i] += boost::lexical_cast<int>(split.at(i));
                          } catch(std::exception &e) {
                              placeholder[i] += 0;
                          }
                        }
                }
                mmap.close();
                {
                        std::lock_guard<std::mutex> lock(vectorLock);
                        totalAU.push_back(std::pair<std::string,std::vector<int>>(filename,placeholder));
                }
        }
};
void Parser::writeResults()
{
    ofstream file;
    file.open("Output/results.csv");
    for(int i = 0; i < totalAU.size(); ++i){
      file << totalAU[i].first << ',';
      for(int j = 0; j < totalAU[i].second.size(); ++j ){
        file << totalAU[i].second[j] << ',';
      }
      file << '\n';
    }
}
