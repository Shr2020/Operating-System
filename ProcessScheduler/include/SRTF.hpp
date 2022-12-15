#pragma once

#include "IScheduler.hpp"

struct ProcessWrapper {
  Process *proc;
  int sequence_number;
};

struct ProcessQCmp {
  bool operator()(ProcessWrapper const &p1, ProcessWrapper const &p2) {
    int p1_rem_cpu_time = p1.proc->get_tct() - p1.proc->get_cpu_time_completed();
    int p2_rem_cpu_time = p2.proc->get_tct() - p2.proc->get_cpu_time_completed();
    if (p1_rem_cpu_time == p2_rem_cpu_time) {
      return p1.sequence_number > p2.sequence_number;
    }
    
    return p1_rem_cpu_time > p2_rem_cpu_time;
  }
};

// Class that models Shortest Remaining Time First scheduler 
class SRTF : public IScheduler {

private:
  std::priority_queue<ProcessWrapper, std::vector<ProcessWrapper>, ProcessQCmp> run_q;
  int counter_ = 0;

public:
  bool has_next() { return !run_q.empty(); }

  Process *get_next_process() {
    if (!has_next()) {
      return nullptr;
    }
    auto p = run_q.top();
    run_q.pop();
    return p.proc;
  }

  void add_process(Process *p) {
    if (p->get_dynamic_priority() < 0) {
      p->reset_dynamic_priority();
    }
    run_q.push({p, counter_++});
  }

  bool preempt_running_proc() {
    return false;
  };
};