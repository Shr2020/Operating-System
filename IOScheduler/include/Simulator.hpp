#pragma once

#include "IScheduler.hpp"
#include "IORequest.hpp"
#include <memory>
#include <queue>
#include <map>
#include "StatisticsTracker.hpp"
#include "Summary.hpp"


// Class that models the simulation.
class Simulator {
public:
  Simulator(IScheduler *isched);
  void run(std::queue<IORequest> &ior, std::vector<StatisticsTracker> &stats, Summary &summary);
  void get_head_mvmt(int pt, int nt);

private:
  IScheduler *sched;
  bool right_direction = true;
};