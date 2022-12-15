#pragma once
#include "Process.hpp"
#include "Frame.hpp"
#include "Pager.hpp"
#include <queue>
#include "RandomNumberGenerator.hpp"
#include <vector>
#include <limits.h>


// Class that models Aging algorithm 
class Aging : public Pager {
public:
  Aging(std::vector<Frame *> f_q, int size) {
    frame_q = f_q;
    list_size = size;
    counter = 0;
  }
  
  Frame *select_victim_frame() {
    if (!has_next()) {
      return nullptr;
    }

    unsigned long max_age = ULONG_MAX;
    unsigned long victim_index = 0;
    for (int i = 0; i < list_size; i++) {
        Frame* f = frame_q.at(counter % list_size);
        f->age = f->age >> 1;
        if (f->pte->referenced) {
            f->age = f->age | 0x80000000;
            f->pte->referenced = 0;
        }
        if (max_age > f->age) {
            max_age = f->age;
            victim_index = counter;
        }
        counter++;
    }

    // set counter to victim frame index
    counter = victim_index;
    
    // victim frame
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