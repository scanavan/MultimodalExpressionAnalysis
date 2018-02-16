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
Parser::Parser(std::string _sdp, std::string request) : sourceDirectoryPath(_sdp) {

      if (request == "mapauto3d") {
          ParseData(&Parser::MapAUto3D, &Parser::writeMappedAUtoBND);
      } else if(request == "create_subset")
      {
        ParseData(&Parser::create_subsets, &Parser::writeNumberFramesPerExpression);
      } else if(request == "make_arffs")
      {
        ParseData(&Parser::getALLdata, &Parser::writeARFF_ALL_BND_AU);
      }

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

void Parser::ParseData(std::function<void(Parser *, std::vector<std::string>&, size_t)> func_parser, std::function<void(Parser *)> func_writer) {

        std::vector<std::thread> threads;
        std::vector<std::string> files_to_parse;

        // directoryPath variable is the path to the source directory of data. This variable gets initilized in the parser constructor.
        for (auto& f : directory_iterator(sourceDirectoryPath)) {
                files_to_parse.push_back(f.path().string());
        }
        std::cout << "Running threahs...\r" << std::flush;

        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
                // The function that we pass to parser is the only thing that needs to be changed depending on which one we want to execute.
                threads.push_back(std::move(std::thread(func_parser, this,
                                                        std::ref(files_to_parse),
                                                        files_to_parse.size())));
         }
        for (auto& t : threads) {
                t.join();
        }
        func_writer(this);
};

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
                std::string path_to_3D_Data = "Data/BP4D+/3DFeatures/BND/DataSubset/";

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

                        // Add all AU's to struct (skips frame since we already know)
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
    int percentage(0);
    std::ofstream file;
    for(int i = 0; i < filenameAndAus.size(); ++i){
      percentage = (i * 100)/ filenameAndAus.size();
      std::cout << "Writting to BND files (" << percentage << "%)\r";
      std::cout.flush();
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
    std::cout << std::endl;
}
// ------------------------------------------------------------------------------------------------------------------------
void Parser::create_subsets(std::vector<std::string>& filesToParse, size_t vecSize) {
        while (readPos < vecSize) {
              std::string filename;
                {
                        std::lock_guard<std::mutex> lock(vectorLock);
                        if (readPos < vecSize) {
                                  filename = filesToParse[readPos];
                        } else {
                                break;
                        }
                        readPos++;
                }
                std::vector<std::string> split;

                // we need to get the emotion from Data/BP4D+/3DFeatures/BND/DataSubset/FXXX_TX_XXXX.bndpluss This will break if path is different to this.
                boost::split(split, filename, boost::is_any_of("/"));
                // we need to get the expression from: FXXX_TX_XXXX.bndpluss
                std::string file = split[5];
                boost::split(split, file, boost::is_any_of("_"));

                std::string expression = split[1];

                // Dummies for lazy string appending lol.
                std::string path_to_copy = "Output/Subsets/DataSubset_";
                path_to_copy.append(expression);
                path_to_copy.append("/");
                path_to_copy.append(file);

                copy_file(filename,path_to_copy,copy_option::overwrite_if_exists);
        }
};

void Parser::writeNumberFramesPerExpression()
{
  std::cout << "writing resutls.." << std::endl;
  std::ofstream file;
  std::string filename = "Output/Counts.txt";
  std::string expression = "T";
  std::string path = "Output/Subsets/DataSubset_";
  file.open(filename);

  for(int i = 1; i < 9; ++i, expression = "T", path = "Output/Subsets/DataSubset_")
  {
    expression.append(std::to_string(i));
    path.append(expression);
    int count = std::distance(directory_iterator(path), directory_iterator{});
    file << expression << " has " << count << " frames." << '\n';
  }
  file << '\n';
  int count = std::distance(directory_iterator(sourceDirectoryPath), directory_iterator{});
  file << "Whole subset folder has " << count << " frames." << "\n\n";

  // Count how many files we should have from frame list.
  std::ifstream frame_list_file;
  frame_list_file.open("Output/allFrameFiles.txt");
  std::vector<std::string> split;
  int counts[8] = {0};
  for(std::string line; std::getline(frame_list_file, line);)
  {
    boost::split(split, line, boost::is_any_of("_"));
    std::string target = split[1];
    int index = target[1] - '0';
    counts[index - 1] += 1;

  }
  frame_list_file.close();
  expression = "T";
  for(int i = 0; i < 8; ++i, expression = "T")
  {
    std::string target = expression.append(std::to_string(i + 1));

    file << "Frame list has " << counts[i] << " frames for " << expression << ".\n";
  }
  int total = 0;
  // Count total number of frames
  for(const auto& x : counts)
  {
    total += x;
  }
  file << "\nTotal number of frames in frame list is " << total << '\n';
  file.close();
}
// ------------------------------------------------------------------------------------------------------------------------

void Parser::getALLdata(std::vector<std::string>& filesToParse, size_t vecSize) {
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
                ARFFLine placeholder;

                // Parse out the emotion from the filename.
                std::vector<std::string> filename_placeholder;
                // Data/BP4D+/3DFeatures/BND/DataSubset/FXXX_TX_XXXX.bndplus
                boost::split(filename_placeholder, filename, boost::is_any_of("/"));
                boost::split(filename_placeholder, filename_placeholder[5], boost::is_any_of("_"));
                placeholder.emotion = filename_placeholder[1];

                // Loops through everyline in the BND file to get the 83 landmarks
                for (int landmark = 0; landmark < 83; ++landmark) {
                        std::string line;
                        std::vector<std::string> split;
                        std::getline(ss, line);
                        boost::split(split, line, boost::is_any_of(","));
                        if(split.size() < 3)
                        {
                          std::cout << "Empty file: " << filename << std::endl;

                        }
                        placeholder.x_axis.push_back(split.at(0));
                        placeholder.y_axis.push_back(split.at(1));
                        placeholder.z_axis.push_back(split.at(2));
                }
                 // Parse action values out of file, last line.
                std::string line;
                std::vector<std::string> split;
                std::getline(ss, line);
                boost::split(split, line, boost::is_any_of(","));

                for(const auto& au : split)
                {
                  placeholder.au_values.action_values.push_back(au);
                }
                mmap.close();
                {
                        // lock access from other threads
                        std::lock_guard<std::mutex> lock(vectorLock);
                        // Add a new line to our ARFF file
                        dataARFF.push_back(placeholder);
                }
        }
};
void Parser::writeARFF_ALL_BND_AU()
{
  ofstream outputfile;
  outputfile.open("Output/ARFF/ALL_BND_AU.arff");
  writeARFF_Header(outputfile,true,true, true ,"");

  for(int i = 0; i < dataARFF.size(); ++i)
  {
    for(int j = 0; j < 83; ++j)
    {
      outputfile << dataARFF[i].x_axis[j] << "," << dataARFF[i].y_axis[j] << "," << dataARFF[i].z_axis[j] << ",";
    }

    for(int k = 0; k < (dataARFF[i].au_values.action_values.size()); ++k)
    {
      outputfile << dataARFF[i].au_values.action_values[k] << ",";
    }
    // Physiological data needs to be added here.
    outputfile << dataARFF[i].emotion << '\n';
  }
  outputfile.close();
  writeARFF_ALL_BND();
  writeARFF_ALL_AU();
  writeARFF_Separete_Emotions();
  validate_ARFF_FILES();
}
void Parser::writeARFF_ALL_BND()
{
  ofstream outputfile;
  outputfile.open("Output/ARFF/ALL_BND.arff");

  writeARFF_Header(outputfile,false,true,true, "");
  for(int i = 0; i < dataARFF.size(); ++i)
  {
    for(int j = 0; j < 83; ++j)
    {
      outputfile << dataARFF[i].x_axis[j] << "," << dataARFF[i].y_axis[j] << "," << dataARFF[i].z_axis[j] << ",";
    }
    outputfile << dataARFF[i].emotion << '\n';
  }
  outputfile.close();
}
void Parser::writeARFF_ALL_AU()
{
  ofstream outputfile;
  outputfile.open("Output/ARFF/ALL_AU.arff");

  writeARFF_Header(outputfile,true,false, true, "");
  for(int i = 0; i < dataARFF.size(); ++i)
  {
    for(int k = 0; k < (dataARFF[i].au_values.action_values.size()); ++k)
    {
      outputfile << dataARFF[i].au_values.action_values[k] << ",";
    }
    outputfile << dataARFF[i].emotion << '\n';
  }
  outputfile.close();
}
void Parser::writeARFF_Separete_Emotions()
{
  std::vector<std::string> classes {"T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8"};

  std::vector<std::vector<ARFFLine>> placeholder(8);
  // separete emotions.
  for(int i = 0; i < dataARFF.size(); ++i)
  {
    std::string emotion = dataARFF[i].emotion;
    int index = boost::lexical_cast<int>(emotion[1]) - 1;
    placeholder[index].push_back(dataARFF[i]);
  }
  // 3 types of arff
  std::vector<std::string> types_arff {"BND_AU.arff", "BND.arff", "AU.arff"}; // can add more in the future

  // create arffs for every emotion.
  for(auto & emotion : classes)
  {

    std::string filename = "Output/ARFF/" + emotion + "_" + types_arff[0];
    ofstream ofile(filename);
    int index = boost::lexical_cast<int>(emotion[1]) - 1;
    write_separete(true, true, ofile, emotion, placeholder[index]);
    filename = "Output/ARFF/" + emotion + "_" + types_arff[1];
    ofile.open(filename);
    write_separete(true, false,ofile, emotion, placeholder[index]);
    filename = "Output/ARFF/" + emotion + "_" + types_arff[2];
    ofile.open(filename);
    write_separete(false, true,ofile, emotion, placeholder[index]);
  }
};

void Parser::write_separete(bool BND, bool AU,ofstream& output_file, std::string& _emotion, std::vector<ARFFLine>& data_each_emotion)
{
  writeARFF_Header(output_file,AU,BND, false, _emotion);

  for(int i = 0; i < data_each_emotion.size(); ++i)
  {
    if(BND)
    {
      for(int j = 0; j < 83; ++j)
      {
        output_file << data_each_emotion[i].x_axis[j] << "," << data_each_emotion[i].y_axis[j] << "," << data_each_emotion[i].z_axis[j] << ",";
      }
    }
    if(AU)
    {
      for(int k = 0; k < (data_each_emotion[i].au_values.action_values.size()); ++k)
      {
        output_file << data_each_emotion[i].au_values.action_values[k] << ",";
      }
    }
    output_file << data_each_emotion[i].emotion << '\n';
  }
  output_file.close();
};

void Parser::writeARFF_Header(ofstream& file, bool AU, bool BND, bool all_classes, std::string emotion)
{
  file << "@relation 'multimodalExpression'\n\n";
  if(BND)
  {
    for(int i = 0; i < 83; ++i)
    {
      file << "@attribute X" << (i + 1) << " numeric\n";
      file << "@attribute Y" << (i + 1) << " numeric\n";
      file << "@attribute Z" << (i + 1) << " numeric\n";
    }
  }
  if(AU)
  {
    int au_numers[35] = {1,2,4,5,6,7,9,10,11,12,13,14,15,16,17,18,19,20,22,23,24,27,28,29,30,31,32,33,34,35,36,37,38,39,99};
    for(int j = 0; j < 35;++j)
    {
      file << "@attribute AU" << au_numers[j]<< " numeric\n";
    }
  }
  if(all_classes)
  {
    file << "@attribute class {T1, T2, T3, T4, T5, T6, T7, T8}\n\n";
  } else {
    file << "@attribute class {" + emotion + "}\n\n";
  }
  file << "@data\n\n";
};

void Parser::validate_ARFF_FILES()
{
  // Get all ARFF files
  std::string path_to_arff = "Output/ARFF/";
  std::string validationFile = "Output/ValidateARFF.txt";
  ofstream outputfile;
  outputfile.open(validationFile);
  for (auto& f : directory_iterator(path_to_arff)) {
          std::string path = f.path().string();
          std::vector<std::string> split;
          boost::split(split, path, boost::is_any_of("/"));
          std::string filename = split[2];

          ifstream inputfile;
          inputfile.open(path);

          // skip header
          bool done_with_header = false;
          int linecount = 0;
          for (std::string line; std::getline(inputfile, line);)
          {
            if(line == "@data")
            {
              // skip next line
              std::getline(inputfile, line);
              done_with_header = true;
            }
            else if(done_with_header)
            {
              ++linecount;
            }
          }
          outputfile << filename << " has " << linecount << " frames. \n";
          inputfile.close();
  }
  outputfile.close();

}
// ------------------------------------------------------------------------------------------------------------------------
