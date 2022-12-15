#pragma once
#include "Process.hpp"
#include "Frame.hpp"
#include "Pager.hpp"
#include <queue>
#include "RandomNumberGenerator.hpp"
#include <vector>
#include <limits.h>


// Class that models Working Set Algorithm
class WorkingSet : public Pager {
public:
  WorkingSet(std::vector<Frame *> f_q, int size) {
    frame_q = f_q;
    list_size = size;
    counter = 0;
    instr_count = 0;
    TAU = 49;
  }
  
  Frame *select_victim_frame() {
    if (!has_next()) {
      return nullptr;
    }
    
    unsigned long min_diff = 0;
    int victim_index = -1;
    int min_diff_index = -1;

    for (int i = 0; i <= list_size; i++) {
        Frame* f = frame_q.at(counter % list_size);

        unsigned long age = instr_count - f->last_use_time;
        
        if (f->pte->referenced) {
            f->last_use_time = instr_count;
            f->pte->referenced = 0;
        } else {
            if (age > TAU) {
                victim_index = counter % list_size;
                break;
            } else {
                if (min_diff == age && min_diff_index == -1) {
                    min_diff = age;
                    min_diff_index = counter % list_size;
                } else if (min_diff < age) {
                    min_diff = age;
                    min_diff_index = counter % list_size;
                }
            }
        }
        counter++;
    }

    // set counter to victim frame index
    if (victim_index == -1) {
        counter = min_diff_index;
    } else {
        counter = victim_index;
    }  
    
    // victim frame
    Frame *p = frame_q.at(counter % list_size);

    // point to next frame
    counter++;
    return p;
  }

  void set_instruction_count(unsigned long count) {
    instr_count = count;
  }

  bool has_next() { return !frame_q.empty(); }
    
private:
  std::vector<Frame *> frame_q;
  unsigned long list_size = 0;
  unsigned long counter = 0;
  unsigned long instr_count = 0;
  unsigned long TAU = 49;
};