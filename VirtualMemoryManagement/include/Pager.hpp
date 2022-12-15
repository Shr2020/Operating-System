#pragma once
#include "PTE.hpp"
#include "Frame.hpp"


// Pager Interface
class Pager {
public:
  virtual Frame* select_victim_frame() = 0;
  virtual void set_instruction_count(unsigned long count) {};
};