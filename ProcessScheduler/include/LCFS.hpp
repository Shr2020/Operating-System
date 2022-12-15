#pragma once

#include "IScheduler.hpp"
#include <stack>

// Class that models Last Come First Served scheduler 
class LCFS : public IScheduler {
public:
public:
  void add_process(Process *p) { 
    if (p->get_dynamic_priority() < 0) {
      p->reset_dynamic_priority();
    }
    run_q.push(p); 
  }

  Process *get_next_process() {
    if (!has_next()) {
      return nullptr;
    }
    Process *p = run_q.top();
    run_q.pop();
    return p;
  }

  bool has_next() { return !run_q.empty(); }

  bool preempt_running_proc() {
    return false;
  };

private:
  std::stack<Process *> run_q;
};