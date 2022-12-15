#pragma once

#include "Des.hpp"
#include "IScheduler.hpp"
#include "StatisticsTracker.hpp"
#include "Summary.hpp"

#include <map>

// Class that models the simulation.
class Simulator {
public:

  void run(Des &des, IScheduler *sched, int quantum,
           std::map<std::string, StatisticsTracker> &stat_map, Summary &sum);
};