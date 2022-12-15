#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>

/**
Class that maintains process specific stats
*/
class StatisticsTracker {
private:
    int io_id = 0;
    int arrival = 0; 
    int start_time = 0; 
    int end_time = 0; 

public:

  // Constructors
  StatisticsTracker() {}
  StatisticsTracker(int id, int at) { 
    io_id = id;
    arrival = at; 
    start_time = 0;
    end_time = 0;
  }

  int get_id() const {
      return io_id;
  }

  int get_at() const {
      return arrival;
  }

  int get_start_time() const {
      return start_time;
  }

  int get_end_time() const {
      return end_time;
  }

  void set_start_time(int st) {
      start_time = st;
  }

  void set_end_time(int et) {
      end_time = et;
  }

  std::string to_string() const {
    std::stringstream ss;
    
    ss << std::setw(5) << io_id << ": " << std::setw(5) << arrival 
    << " " << std::setw(5) << start_time << " " << std::setw(5) << end_time << std::endl;
    return ss.str();
  }
};