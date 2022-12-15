#pragma once

#include "IScheduler.hpp"
#include <iostream>
#include <queue>
#include <vector>
#include <list>

// Class that models CLOOK scheduler 
class CLOOK : public IScheduler {
public:
  void add_IORequest(IORequest io) { 
    run_q.push_back(io);
    if (inc_direction && max_track < io.get_track()) {
      max_track = io.get_track();
    } else if (!inc_direction && min_track > io.get_track()) {
      min_track = io.get_track();
    }
  }

  IORequest get_next_request() {
    
    int min = 1000000;
    int min_seek_elem = 0;
    IORequest next_req;
    for (int i = 0; i < run_q.size(); i++) {
      if (last_track != -1) {
        int ms = get_min_seek(run_q.at(i).get_track());
        if (ms >= 0 && ms < min) {
          min = ms;
          min_seek_elem = i;
        }
      } 
    }
    
    next_req = run_q.at(min_seek_elem);
    last_track = next_req.get_track();
    run_q.erase(run_q.begin() + min_seek_elem);
    
    return next_req;
  }

  void update_min_max() {
    int min = 1000000;
    int max = 0;
    for (int i = 0; i < run_q.size(); i++) {
      int track = run_q.at(i).get_track();
      if (track > max) {
        max = track;
      } 

      if (track < min) {
        min = track;
      }
    }

    max_track = max;
    min_track = min;
  }

  bool has_next() { return !(run_q.size() == 0); }
  
  int get_min_seek(int track) {
    if (inc_direction) {
      return track - last_track;
    } else {
      return last_track - track;
    }
  }

  bool get_direction() {return inc_direction;}

  int reset() {
      if (last_track == max_track) {
          update_min_max();
          int shift = last_track - min_track;
          last_track = 0;
          return shift;
      }

      return 0;
  }

private:
  std::vector<IORequest> run_q;
  int last_track = -1;
  bool inc_direction = true; // if true head moves from 0 to n
  int max_track = -1;
  int min_track = 100000;
  bool hand_reset = false;
};


