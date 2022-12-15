#pragma once
#include "Process.hpp"
#include "Frame.hpp"
#include "Pager.hpp"
#include <queue>
#include <vector>



// Class that models FIFO Algorithm
class FIFO : public Pager {
public:
  FIFO(std::vector<Frame *> f_q, int size) {
    frame_q = f_q;
    list_size = size;
    counter = 0;
  }

  Frame *select_victim_frame() {
    if (!has_next()) {
      return nullptr;
    }
    Frame *p = frame_q.at(counter % frame_q.size());
    counter++;
    return p;
  }

  bool has_next() { return !frame_q.empty(); }

  

private:
  std::vector<Frame *> frame_q;
  unsigned long list_size = 0;
  unsigned long counter = 0;
};