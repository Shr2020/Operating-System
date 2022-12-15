#pragma once
#include "Process.hpp"
#include "Frame.hpp"
#include "Pager.hpp"
#include <queue>
#include "RandomNumberGenerator.hpp"
#include <vector>


// Class that models Random Algorithm
class Random : public Pager {
public:
  
  Random(std::vector<Frame *> f_list, int ft_size) {
      frame_list = f_list;
      size = ft_size;
  }

  Frame *select_victim_frame() {
    int index = get_index_victim_frame();
    Frame *p = frame_list.at(index);
    return p;
  }

  int get_index_victim_frame() {
      int i = RandomNumberGenerator::instance().get_next_randnum() % size;
      return i;
  }

private:
  std::vector<Frame *> frame_list;
  int size = 0;
};