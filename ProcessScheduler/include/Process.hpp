#pragma once

#include "State.hpp"
#include <string>

// Class the models the process.
class Process {
private:
  std::string pn;
  int arrival_time;
  int total_cpu_time;
  int max_cpu_burst;
  int max_io_burst;
  int cpu_burst;
  int io_burst;
  int rem_cb;
  int rem_ib;
  int cpu_time_completed;
  State state;
  int state_ts;
  int static_priority;
  int dynamic_priority;
  

public:
  Process(std::string n, int at, int tct, int mcb, int mib, int mprio);
  void set_params(std::string n, int at, int tct, int mcb, int mib, int mprio);
  
  
  void set_cb(int qt);
  void set_rem_cb(int qt);
  void set_rem_ib(int qt);
  void set_state(State s);
  void set_state_ts(int t);
  void set_dynamic_priority(int p);
  void set_static_priority(int p);

  int calc_new_cpu_burst();
  int calc_new_io_burst();
  void calc_cpu_time_completed(int cpu_time);
  void calc_rem_cb(int qt);
  void calc_rem_ib(int qt);
  int calc_stat_prio(int prio);
  void update_dynamic_prio();
  void reset_dynamic_priority();

  int get_at() { return arrival_time; }
  int get_tct() { return total_cpu_time; }
  int get_mcb() { return max_cpu_burst; }
  int get_mib() { return max_io_burst; }
  int get_cb() { return cpu_burst; }
  int get_ib() { return io_burst; }
  int get_cpu_time_completed() { return cpu_time_completed; }
  int get_rem_cb() { return rem_cb;  }
  int get_rem_ib() { return rem_ib;  }
  State get_state() { return state; }
  int get_state_ts() { return state_ts; }
  int get_dynamic_priority() { return dynamic_priority; }
  int get_static_priority() { return static_priority; }
  std::string get_name() { return pn; }
};