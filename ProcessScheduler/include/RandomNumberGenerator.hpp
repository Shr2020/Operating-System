#pragma once
#include <string>
#include <vector>

// utility class to generate the random numbers using the random file. 
class RandomNumberGenerator {

private:
  RandomNumberGenerator() : ofs_(0) {}

  RandomNumberGenerator(RandomNumberGenerator const &) = delete;
  void operator=(RandomNumberGenerator const &) = delete;

  std::vector<int> rnums_;
  int ofs_;

public:
  static RandomNumberGenerator &instance() {
    static RandomNumberGenerator rng;
    return rng;
  }

  void init(std::string randfile);
  int get_next_randnum();
};