#pragma once

#include "Process.hpp"
#include <iostream>
#include <iomanip>

/**
Class that maintains process specific stats
*/
class StatisticsTracker {
private:
  Process *proc;
  int finishing_time = 0;
  int turnaround_time = 0;
  int io_time = 0;
  int stat_priority = 0;
  int cpu_waiting_time = 0;

public:

  // Constructors
  StatisticsTracker() {}
  StatisticsTracker(Process *pr) { 
    proc = pr;
    stat_priority = pr->get_static_priority();
  }

  void set_finishing_time(int t) { finishing_time = t; }

  void set_turnaround_time(int t) { turnaround_time = t; }

  void set_io_time(int t) { io_time = io_time + t; }

  void set_stat_priority(int t) { stat_priority = t; }

  void set_cpu_waiting_time(int t) { cpu_waiting_time = cpu_waiting_time + t; }

  int get_ft() const { return finishing_time; }
  int get_tt() const { return turnaround_time; }
  int get_iot() const { return io_time; }
  int get_sp() const { return stat_priority; }
  int get_cwt() const { return cpu_waiting_time; }
  Process *get_proc() const { return proc; }

  // Turnaround time is the time taken by a process to complete after its arrival.
  int calc_turnaround_time() const { return finishing_time - proc->get_at(); }
};