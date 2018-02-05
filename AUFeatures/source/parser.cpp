#include "parser.hpp"

std::mutex vectorLock;
std::atomic<int> readPos{0};

// ------------------------------------------------------------------------------------------------------------------------
/*
  ~~~~ FUNCTION READ ME ~~~~
  Parser class constructor, it sets the data source path and calls the functions we want to execute

  Must always contain a call to ParseData first (if ECCV work) and then the related writer function
  (will change this to be part of writer class)

  For ICIP, only the randomSampling() call is needed.

*/
Parser::Parser(const std::string& _dp) : directoryPath(_dp) {
      ParseData();
      writeMappedAUtoBND();
};

// ------------------------------------------------------------------------------------------------------------------------
/*
  ~~~~ FUNCTION READ ME ~~~~
  ParseData function actually parses the data, this is done by initializing all the threads in a vector,
  the function takes the address of a function which references the function that all the threads will execute.

  Example:
  std::thread(&Parser::MapAUto3D, this, std::ref(files_to_parse),files_to_parse.size())));
  where MapAUto3D is the function to execute.

  Data source path is defined by the constructor in main.cpp

  Note: This function initializes a vector that contains all the files to read (files_to_parse).
*/
void Parser::ParseData() {
        std::vector<std::thread> threads;
        std::vector<std::string> files_to_parse;
        // directoryPath variable is the path to the source directory of data. This variable gets initilized in the parser constructor.
        for (auto& f : directory_iterator(directoryPath)) {
                files_to_parse.push_back(f.path().string());
        }
        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
                // The function that we pass to parser is the only thing that needs to be changed depending on which one we want to execute.
                threads.push_back(std::move(std::thread(&Parser::MapAUto3D, this,
                                                        std::ref(files_to_parse),
                                                        files_to_parse.size())));
         }
        for (auto& t : threads) {
                t.join();
        }
};
// ------------------------------------------------------------------------------------------------------------------------


/*
~~~~ FUNCTION READ ME ~~~~
getSumAus function takes care of adding all the AU's in a subject file to see which one occurs the most.
This was used as our original thought of a way to compare Aus. Might not be needed anymore.

The results of this parsing are stored in: totalAU vector.

The results of this function must be wrriten with:
  writeSumResults()

*/
void Parser::getSumAus(std::vector<std::string>& filesToParse, size_t vecSize) {
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

                // Initialize the vector, starts at 1 because we ignore the frame number for this task.
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
void Parser::writeSumResults()
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
// ------------------------------------------------------------------------------------------------------------------------

/*
~~~~ FUNCTION READ ME ~~~~
parseFrames function takes care of creating a vector that has all the possible File Names for all the Frames
that we have in the AU data.

The results of this parsing are stored in: framesFileName vector.

The results of this function must be wrriten with:
  writeFrameFiles()
*/
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
                        // Pushes name of the possible target file as M016_T7_XXXX where XXXX is the frame number.
                        frames.push_back(boost::trim_copy(dummie_string.append(split.at(0))));
                        dummie_string = subjectInfo;

                }
                mmap.close();
                {
                        // lock access from other threads
                        std::lock_guard<std::mutex> lock(vectorLock);
                        // Add proper mapped frame name.
                        framesFileName.push_back(frames);
                }
        }
};

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

// ------------------------------------------------------------------------------------------------------------------------
/*
~~~~ FUNCTION READ ME ~~~~
MapAUto3D function takes care of creating a vector that contains all the AU's for a given subject file.
Each member of the vector is an AU object, every AU object corraletes to a given frame in that subject file,
this object also has the filename of the BND file it maps to.

The results of this parsing are stored in: filenameAndAus vector.

The results of this function must be wrriten with:
  writeMappedAUtoBND()
*/

void Parser::MapAUto3D(std::vector<std::string>& filesToParse, size_t vecSize) {
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

                // Needed placeholder as we need to save every line in every file.
                std::vector<AU> placeholder;

                // Path to 3D Folder, this might need to be changed.
                std::string path_to_3D_Data = "Data/BP4D+/3DFeatures/BND/";

                std::vector<std::string> split;
                std::string line_dummie;

                // we need to get the subject from Data/AUCoding/AU_OCC/M016_T7.csv This will break if path is different to this.
                boost::split(split, filename, boost::is_any_of("/"));
                // we need to get the subject from:M016_T7.csv
                boost::split(split, split[3], boost::is_any_of("."));

                std::string subjectInfo = split[0];

                // Dummies for lazy string appending lol.
                std::string dummie_string = subjectInfo;
                std::string dummie_filePath = path_to_3D_Data;

                // skip header.
                std::getline(ss, line_dummie);

                // Get actual data
                std::getline(ss, line_dummie);

                // Loops through everyline in an AU subject file (every frame)
                for (std::string line; std::getline(ss, line);) {
                        boost::split(split, line, boost::is_any_of(","));
                        dummie_string.append("_");
                        int frame_number = boost::lexical_cast<int>(split.at(0));

                        // Deals with the fact that files are format of _0001.bnd, this part builds the right name for the BND files
                        if(frame_number < 10){
                          dummie_string.append("000");
                        } else if(frame_number < 100){
                          dummie_string.append("00");
                        } else if(frame_number < 1000){
                          dummie_string.append("0");
                        }
                        dummie_string.append(split.at(0));
                        dummie_string.append(".bndplus");

                        // Initialize AU struct with full path of the file it maps to in BND data.
                        AU AU_at_frame(dummie_filePath.append(dummie_string));

                        // Add all AU's to struct
                        for(int i = 1; i < split.size(); ++i){
                          AU_at_frame.action_values.push_back(boost::trim_copy(split.at(i)));
                        }
                        // We add the Aus for the given frame to the vector.
                        placeholder.push_back(AU_at_frame);
                        // reset dummies
                        dummie_string = subjectInfo;
                        dummie_filePath = path_to_3D_Data;
                }
                mmap.close();
                {
                        // lock access from other threads
                        std::lock_guard<std::mutex> lock(vectorLock);
                        // Add proper file path mapped to 3D and All AUs to our vector.
                        filenameAndAus.push_back(placeholder);
                }
        }
};

void Parser::writeMappedAUtoBND()
{
    ofstream file;
    for(int i = 0; i < filenameAndAus.size(); ++i){
      for(int j = 0; j < filenameAndAus[i].size(); ++j ){
        // In case file not found...
        if(boost::filesystem::exists(filenameAndAus[i][j].fullPath))
        {
          file.open(filenameAndAus[i][j].fullPath, std::ofstream::app);
          file << filenameAndAus[i][j];
          file.close();
        }
      }
    }
}
// ------------------------------------------------------------------------------------------------------------------------

// This section of the code are functions to do ICIP parsing work.

void Parser::randomSampling(std::string ARFF_file)
{
  ifstream file;
  file.open(ARFF_file);
  std::cout << "Parsing arff file..." << std::endl;
  if(file.fail())
  {
    std::cout << "Could not open file: " << ARFF_file << std::endl;
  }

  // Create and populate placeholder and number of subjects in ARFF file.
  std::pair<int,std::vector<std::string>>DataAndHeader;
  SubjectsInARFF(file, DataAndHeader);

  // Need to rewind back to the start of the file
  file.clear();
  file.seekg(0, std::ios::beg);

  // Number of subjects per file
  int number_of_subjects = DataAndHeader.first;
  int subjectsPerFile = number_of_subjects / 6;

  // Save header (needed in all files)
  std::vector<std::string> header(DataAndHeader.second);

  // Create and populate vector for the whole data.
  std::vector<std::string> data;
  fillDataContainer(file, data);
  file.close();

  // shuffle container for random sampling
  std::random_shuffle(data.begin(), data.end());
  std::string database_name = "BP4D";
  std::string outputFileName;

  std::cout << "Generating new files..." << std::endl;
  // Generate new Files
  for(int i = 0; i < 6; ++i)
  {
      outputFileName = database_name;
      // Setup output file name
      outputFileName.append(std::to_string(i));
      outputFileName.append(".arff");
      std::string path("Output/");
      path.append(outputFileName);
      ofstream file(path);

      if(file.fail())
      {
        std::cout << "unable to open file: " << path <<std::endl;
      }

      // Write header
      for(const auto& line : header)
      {
        file << line << "\n";
      }
      int starting_point = subjectsPerFile * i;
      int ending_point = subjectsPerFile * (i + 1);
      for(int j = starting_point; j < ending_point; ++j)
      {
        file << data[j] << "\n";
      }
      file.close();
  }
}

// SubjectsInARFF calculates how many subjects there are in the ICIP arff file.
void Parser::SubjectsInARFF(ifstream& file, std::pair<int,std::vector<std::string>>& header_container)
{
  int num_lines;
  std::string line, split;

  // Skip header lines while saving them.
  for(int i = 0; i < 255; std::getline(file, line), header_container.second.push_back(line), ++i);

  header_container.first = std::count(std::istreambuf_iterator<char>(file),
                            std::istreambuf_iterator<char>(), '\n');
}

// fillDataContainer fills the vector that will contain all the data in the ARFF file.
void Parser::fillDataContainer(ifstream& file, std::vector<std::string>& lines)
{
  std::string line;
  // Skip header
  for(int i = 0; i < 255; std::getline(file, line),++i);
  // Parse data.
  for (; std::getline(file, line); lines.push_back(line));
}
