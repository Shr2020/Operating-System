#pragma once

#include <array>
#include <iostream>
#include <string>
#include <vector>
#include "VMA.hpp"
#include "PTE.hpp"

// Class the models the process.
class Process {

private:
  int pn;
  std::vector<VMA> vma_list;
  static const int MAX_VPAGES = 64;

public:

  std::array<PTE, MAX_VPAGES> page_table;

  Process(int name): pn(name) {}

  int get_id() { return pn; }
  std::vector<VMA> get_vma_list() { return vma_list; }

  void add_vma(VMA vma) {
    vma_list.push_back(vma);
  }

  std::array<PTE, MAX_VPAGES> get_array() {
    return page_table;
  }

  void print_list() {
    for (int i = 0; i < vma_list.size(); i++) {
      vma_list[i].print_vma();
    }
  }

  bool has_vpage(int vp) {
    for(int i = 0; i < MAX_VPAGES; i++) {
      if (page_table[i].present == 1) {
        return true;
      }
    }
    return false;
  }

  void to_string() {
    std::cout <<  "process: " << pn << std::endl;
    print_list();
  }
};