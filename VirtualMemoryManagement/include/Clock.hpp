#pragma once
#include "Process.hpp"
#include "Frame.hpp"
#include "Pager.hpp"
#include <queue>
#include <vector>


// Class that models clock Algorithm 
class Clock : public Pager {
public:
  Clock(std::vector<Frame *> f_q, int size) {
    frame_q = f_q;
    list_size = size;
    counter = 0;
  }

  Frame *select_victim_frame() {
    if (!has_next()) {
      return nullptr;
    }

    while (frame_q.at(counter % list_size)->pte->referenced) {
        // it got the second chance. set it to zero.
        frame_q.at(counter % list_size)->pte->referenced = 0;
        counter++;
    }
    Frame *p = frame_q.at(counter % list_size);

    // point to next frame
    counter++;
    return p;
  }

  bool has_next() { return !frame_q.empty(); }



private:
  std::vector<Frame *> frame_q;
  unsigned long list_size = 0;
  unsigned long counter = 0;
};