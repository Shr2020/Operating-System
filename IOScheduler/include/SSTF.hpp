#pragma once
#include "IScheduler.hpp"
#include <iostream>
#include <queue>
#include <vector>
#include <list>

// Class that models SSTF scheduler 
class SSTF : public IScheduler {
public:
  void add_IORequest(IORequest io) { 
    run_q.push_back(io); 
  }

  IORequest get_next_request() {
    
    int min = 1000000;
    int min_seek_elem = 0;
    IORequest next_req;
    for (int i = 0; i < run_q.size(); i++) {
      if (last_track != -1) {
        int ms = get_min_seek(run_q.at(i).get_track());
        if (ms < min) {
          min = ms;
          min_seek_elem = i;
        }
      } 
    }
    
    next_req = run_q.at(min_seek_elem);
    update_dir(next_req.get_track());
    last_track = next_req.get_track();
    run_q.erase(run_q.begin() + min_seek_elem);
    return next_req;
  }

  bool has_next() { return !(run_q.size() == 0); }//


  void update_dir(int nt) {
    if (last_track != -1) {
      if (last_track > nt) {
        inc_direction = false;
      } else {
        inc_direction = true;
      }
    } else {
      inc_direction = true; 
    }
  }

  int get_min_seek(int track) {
    if (last_track == -1) {
      // Very First element
      return track;
    }
    if (last_track > track) {
      return last_track - track;
    } else {
      return track - last_track;
    }
  }

  bool get_direction() {return inc_direction;}

private:
  std::vector<IORequest> run_q;
  int last_track = -1;
  bool inc_direction = true; // if true head moves from 0 to n
};