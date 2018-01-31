#include "parser.hpp"

std::mutex vectorLock;
std::atomic<int> readPos{0};

Parser::Parser(const std::string& _dp) : directoryPath(_dp) {
      //  auto start = std::chrono::system_clock::now();
        std::cout << "Parsing data..." << std::endl;
        ParseData();
      //  auto end = std::chrono::system_clock::now();
      //  std::chrono::duration<double> elapsed_seconds = end - start;
      //  std::cout << elapsed_seconds.count() << std::endl;
        std::cout << "Writing Results..." << std::endl;
        writeFrameFiles();
};

void Parser::ParseData() {
        std::vector<std::thread> threads;
        std::vector<std::string> files_to_parse;
        for (auto& f : directory_iterator(directoryPath)) {
                files_to_parse.push_back(f.path().string());
        }
        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
                // The function that we pass to parser is the only thing that needs to be changed depending on which one we want to execute.
                threads.push_back(std::move(std::thread(&Parser::parseFrames, this,
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
                // Buffer so we can use std::getline()
                std::stringstream ss;
                ss << buffer;
                std::vector<int> placeholder;

                std::vector<std::string> split;
                std::string line_dummie;

                // skip header.
                std::getline(ss, line_dummie);

                // Get actual data
                std::getline(ss, line_dummie);
                boost::split(split, line_dummie, boost::is_any_of(","));

                // Initialize the vector
                for(int i = 1; i < split.size(); ++i){
                  placeholder.push_back(boost::lexical_cast<int>(boost::trim_copy(split.at(i))));
                }

                // Add upp all the AU
                for (std::string line; std::getline(ss, line);) {
                        boost::split(split, line, boost::is_any_of(","));
                        for(int i = 1; i < split.size(); ++i){
                          placeholder[i - 1] += boost::lexical_cast<int>(boost::trim_copy(split.at(i)));
                        }
                }
                mmap.close();
                {
                        // lock access from other threads
                        std::lock_guard<std::mutex> lock(vectorLock);
                        // Add summary of the AU's of a subject to the vector
                        totalAU.push_back(std::pair<std::string,std::vector<int>>(filename,placeholder));
                }
        }
};
void Parser::parseFrames(std::vector<std::string>& filesToParse, size_t vecSize) {
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
                // Buffer so we can use std::getline()
                std::stringstream ss;
                ss << buffer;

                // vector that contains all the frames for file/subject
                std::vector<std::string> frames;

                // Used to parse the csv file
                std::vector<std::string> split;
                std::string line_dummie;

                // we need to get the subject from Data/AUCoding/AU_OCC/M016_T7.csv
                boost::split(split, filename, boost::is_any_of("/"));
                // we need to get the subject from:M016_T7.csv
                boost::split(split, split[3], boost::is_any_of("."));

                std::string subjectInfo = split[0];
                std::string dummie_string = subjectInfo;
                // skip header.
                std::getline(ss, line_dummie);

                // Parse out frame number
                for (std::string line; std::getline(ss, line);) {
                        boost::split(split, line, boost::is_any_of(","));
                        dummie_string.append("_");
                        frames.push_back(boost::trim_copy(dummie_string.append(split.at(0))));
                        dummie_string = subjectInfo;

                }
                mmap.close();
                {
                        // lock access from other threads
                        std::lock_guard<std::mutex> lock(vectorLock);
                        // Add summary of the AU's of a subject to the vector
                        framesFileName.push_back(frames);
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
    file.close();
}
void Parser::writeFrameFiles()
{
    ofstream file;
    file.open("Output/allFrameFiles.txt");
    for(int i = 0; i < framesFileName.size(); ++i){
      for(int j = 0; j < framesFileName[i].size(); ++j ){
        file << framesFileName[i][j] << '\n';
      }
    }
    file.close();
}
