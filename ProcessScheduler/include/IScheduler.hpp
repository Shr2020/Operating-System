#pragma once
#include "Process.hpp"

// Scheduler Interface
class IScheduler {
public:
  virtual void add_process(Process *p) = 0;
  virtual Process *get_next_process() = 0;
  virtual bool preempt_running_proc() = 0;
};