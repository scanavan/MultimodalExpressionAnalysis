#include "parser.hpp"

// argv = source directory path , function to do
int main(int argc, char *argv[]) {

  if(argc < 2)
  {
    std::cout << "Not enough arguments, please enter source directory path and function to execute: " << std::endl;
    std::cout << "Function names: " << std::endl;
    std::cout << "(3): mapauto3d " << std::endl;
    std::cout << "(5): create_subset [split data into folders for each expression]" << std::endl;
    std::cout << "(6): make_arffs [creates all possible ARFF files]" <<std::endl;
    exit(EXIT_FAILURE);
  } else {
  // Data Source path must be inserted below
  Parser p(argv[1], argv[2]);
  }
}
