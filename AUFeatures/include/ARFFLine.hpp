#ifndef ARFFLINE_HPP
#define ARFFLINE_HPP
#include "AU.hpp"
#include <string>
#include <vector>
#include <iostream>

  struct ARFFLine{
    std::string emotion;
    AU au_values;
    std::vector<std::string> x_axis;
    std::vector<std::string> y_axis;
    std::vector<std::string> z_axis;
  };
#endif
