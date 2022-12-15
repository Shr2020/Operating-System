#pragma once
#include <vector>
#include <map>
#include <sstream>
#include "StatisticsTracker.hpp"

class Summary {
  private:

  // total simulation time
  int total_time = 0;

  // total movement
  int total_movement = 0;

  // max wait time
  int max_wt = 0;

  // avg turnarount time of each io request
  double avg_turnaround_time = 0.0;

  // avg wait time of each io request
  double avg_wt = 0.0;

  // total turnaround time
  int total_tt = 0;

  // total wait time
  int total_wt = 0;

  // num of io req
  int num_io = 0;

public:

  int get_total_time() { return total_time; }

  int get_max_wt() { return max_wt;}
  
  int get_total_movement() { return total_movement;}
  
  double get_avg_turnaround_time() { return avg_turnaround_time; }
  
  double get_avg_wt() { return avg_wt; }
  
  void calc_avg_turnaround_time() { 
    avg_turnaround_time = double(total_tt) / num_io;
  }

  void calc_avg_wt() { 
    avg_wt = double(total_wt) / num_io;
  }

  void set_total_movement(int mvt) {
      total_movement = mvt;
  }

  // Calculates components of summary stats
  void calc_summary(std::vector<StatisticsTracker> const &m) {
    int et = 0;
    int wt = 0;
    num_io = m.size();
    for (int i = 0; i < m.size(); i++) {
      et = m.at(i).get_end_time();
      if (total_time < et) {
        total_time = et; 
      }

      wt = m.at(i).get_start_time() - m.at(i).get_at();
      if (max_wt < wt) {
        max_wt = wt; 
      }
      total_wt += wt;

      total_tt += m.at(i).get_end_time() - m.at(i).get_at();
    }
    calc_avg_wt();
    calc_avg_turnaround_time();
  }

  // return the summary in string format.
  std::string to_string() {
    std::stringstream ss;
    ss << "SUM: " << get_total_time() << " " 
              << get_total_movement() << " " << std::fixed 
              << std::setprecision(2) << get_avg_turnaround_time() << " "
              << std::setprecision(2) << get_avg_wt() << " "
              << get_max_wt();
    return ss.str();
  }

};