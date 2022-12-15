#pragma once
#include "PTE.hpp"
#include "Process.hpp"

class Frame {
public:
  Frame(int i) {
      index = i;
  }

  Process* process = nullptr;
  PTE* pte = nullptr;
  int vpage = -1;
  int index= -1;  // index in the global Frame table
  unsigned age = 0;
  unsigned long last_use_time = 0;
};