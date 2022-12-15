#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include "RandomNumberGenerator.hpp"

void RandomNumberGenerator::init(std::string randfile) {
  std::fstream fi;
  fi.open(randfile);

  if (!fi.is_open()) {
    throw std::runtime_error(std::string("Error opening randfile: ") +
                             randfile);
  }
  ofs_ = 0;
  std::string line;

  getline(fi, line); /// ignore first line because it contains the number of
                     /// randoms that follow

  while (!fi.eof()) {
    while (getline(fi, line)) {
      char *cstr = new char[line.length() + 1];
      strcpy(cstr, line.c_str());
      char *p = strtok(cstr, " \t\n");
      while (p != 0) {
        rnums_.push_back(std::stoi(std::string(p)));
        p = strtok(NULL, " \t\n");
      }
    }
  }
  fi.close();
}

int RandomNumberGenerator::get_next_randnum() {
  int rand = rnums_.at(ofs_ % rnums_.size());
  ofs_++;
  return rand;
}