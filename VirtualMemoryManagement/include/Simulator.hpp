#pragma once

#include "Frame.hpp"
#include "Pager.hpp"
#include "Process.hpp"
#include "PTE.hpp"
#include <memory>
#include <queue>
#include <vector>
#include "StatisticsTracker.hpp"
#include "Summary.hpp"


using ProcessVec = std::vector<Process*>;
using InstructVec = std::vector<std::pair<std::string, int>>;

// Class that models the simulation.
class Simulator {
public:
  Simulator(Pager *pager, int num_frames, std::vector<Frame*> frame_table);
  Frame *get_frame();
  int get_free_frame_from_list();
  void run(InstructVec insts, ProcessVec pv, std::vector<StatisticsTracker> &stats, Summary &summary);

private:
  std::vector<Frame*> frame_table;
  std::queue<int> free_list;
  int num_frames = 0;
  Pager *pager;
};