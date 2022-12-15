#pragma once

#include "StatisticsTracker.hpp"
#include <map>
#include <iomanip>
#include <sstream>

/**
Class that maintains summary stats for all processes.
*/ 
class Summary {
private:

  // time of completion of last process
  int ft_last = 0;

  // cpu utilization (in %)
  double cpu_utilization = 0.0;

  // io utilization (%)
  double io_utilization = 0.0;

  // avg turnarount time of each process
  double avg_tt = 0.0;

  // avg cpu wait time of each process
  double avg_cwt = 0.0;

  double throughput = 0.0;

  // total turnaround time
  int total_tt = 0;

  // total cpu wait time
  int total_cwt = 0;

  // total cpu time utilized
  int total_cpu = 0;

  // num of processes
  int num_proc = 0;

public:

  int get_ft_last() { return ft_last; }
  double get_cpu_ut() { return cpu_utilization; }
  double get_io_ut() { return (double(io_utilization)/ft_last)*100; }
  double get_avg_tt() { return avg_tt; }
  double get_avg_cwt() { return avg_cwt; }
  double get_throughput() { return throughput; }

  void set_io_utilization(double t) { io_utilization = t; }

  void calc_cpu_ut() { 
    cpu_utilization = (double(total_cpu) / ft_last) * 100.0; 
  }

  void calc_avg_tt() { 
    avg_tt = double(total_tt) / num_proc;
  }

  void calc_avg_cwt() { 
    avg_cwt = double(total_cwt) / num_proc;
  }

  void calc_throughput() { 
    throughput = (double(num_proc) / ft_last) * 100;
  }

  // Calculates components of summary stats
  void calc_summary(std::map<std::string, StatisticsTracker> const &m) {
    int ft = 0;
    num_proc = m.size();
    for (auto &entry : m) {
      ft = entry.second.get_ft();
      if (ft_last < ft) {
        ft_last = ft; 
      }
      total_tt += entry.second.get_tt();
      total_cwt += entry.second.get_cwt();
      total_cpu += entry.second.get_proc()->get_tct();
    }
    calc_avg_cwt();
    calc_avg_tt();
    calc_cpu_ut();
    calc_throughput();
  }

  // return the summary in string format.
  std::string to_string() {
    std::stringstream ss;
    ss << "SUM: " << get_ft_last() << " " << std::fixed 
              << std::setprecision(2) << get_cpu_ut() << " "
              << std::setprecision(2) << get_io_ut() << " "
              << std::setprecision(2) << get_avg_tt() << " "
              << std::setprecision(2) << get_avg_cwt() << " "
              << std::setprecision(3) << get_throughput();
    return ss.str();
  }
};