#ifndef AU_HPP
#define AU_HPP
#include <vector>
#include <string>
#include <iostream>

// This could be changed to ints if operations are needed, left as strings for faster parsing.
struct AU {
  std::string fullPath;
  std::vector<std::string> action_values;
  std::string frame;

  AU( std::string fp) : fullPath(fp) {};
  AU()
  {
    // number of AUs
    action_values.resize(0);
  }

  friend std::ostream& operator <<(std::ostream& os, const AU& object)
  {
    int i = 0;
    for(; i < object.action_values.size() - 1; ++i)
    {
      os << object.action_values[i] << ",";
    }
    os << object.action_values[i] << "\n";
    return os;
  }

};
#endif
