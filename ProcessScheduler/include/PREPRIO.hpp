#pragma once

#include "IScheduler.hpp"
#include <vector>
#include "Process.hpp"
#include <queue>

// Class that models Preemptive Priority scheduler 
class PREPRIO : public IScheduler {
public:
  PREPRIO(int mprio) {
    activeQ = std::make_unique<ProcessQueues>();
    expiredQ = std::make_unique<ProcessQueues>();
    maxprio = mprio;
    for(int i = 0; i < mprio; i++) {
      (*activeQ).push_back({});
      (*expiredQ).push_back({});
    }
  }

  void add_process(Process *p) {
    int index = p->get_dynamic_priority();
    if (index > -1) {
      (*activeQ).at(index).push(p);
    } else {
      p->reset_dynamic_priority();
      (*expiredQ).at(p->get_dynamic_priority()).push(p);
    }
  }

  Process *get_next_process() {
    Process* proc = nullptr;
    proc = pop_highest_prio_process(*activeQ);

    // activeQ is empty. Swap it with expiredQ.
    if (proc == nullptr) {
      auto temp = std::move(activeQ);
      activeQ = std::move(expiredQ);
      expiredQ = std::move(temp);
      proc = pop_highest_prio_process(*activeQ);
    }

    return proc;
  }

  bool preempt_running_proc() {
    return true;
  };

  bool has_next() { return false; }

private:
  using ProcessQueues = std::vector<std::queue<Process*>>;
  std::unique_ptr<ProcessQueues> activeQ;
  std::unique_ptr<ProcessQueues> expiredQ;
  int maxprio;

  Process *pop_highest_prio_process(ProcessQueues &queues) {
    Process *p = nullptr;
    for (int i = queues.size() - 1; i >= 0; i--) {
      if (!queues.at(i).empty()) {
        p = queues.at(i).front();
        queues.at(i).pop();
        break;
      }
    }
    return p;
  }
};