#pragma once

#include "IScheduler.hpp"
#include <iostream>
#include <queue>
#include <vector>
#include <list>

// Class that models FLOOK scheduler 
class FLOOK : public IScheduler {
public:

  FLOOK() {
    add_q = std::make_unique<std::vector<IORequest>>();
    active_q = std::make_unique<std::vector<IORequest>>();
  }

  void add_IORequest(IORequest io) { 
    (*add_q).push_back(io);
    if (inc_direction && max_track < io.get_track()) {
      max_track = io.get_track();
    } else if (!inc_direction && min_track > io.get_track()) {
      min_track = io.get_track();
    }
  }

  IORequest get_next_request() {
    
    update_dir();
    int min_dist = 1000000;
    int closest_req_ind = 0;
    IORequest next_req;
    for (int i = 0; i < (*active_q).size(); i++) {
      if (last_track != -1) {
        int ms = get_min_seek((*active_q).at(i).get_track());
        if (ms >= 0 && ms < min_dist) {
          min_dist = ms;
          closest_req_ind = i;
        }
      } 
    }
    
    next_req = (*active_q).at(closest_req_ind);
    last_track = next_req.get_track();
    (*active_q).erase((*active_q).begin() + closest_req_ind);
    
    return next_req;
  }

  void update_min_max() {
    int min = 1000000;
    int max = -1;
    for (int i = 0; i < (*active_q).size(); i++) {
      int track = (*active_q).at(i).get_track();
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

  bool has_next() { 
      bool elements = false;
      if ((*active_q).size() > 0){
          elements = true;
      } else if ((*add_q).size() > 0 && (*active_q).size() == 0) {
          elements = true;
          auto temp = std::move(active_q);
          active_q = std::move(add_q);
          add_q = std::move(temp);
      }
      return elements;
    }
  
  void update_dir() {
    update_min_max();
    if ((inc_direction && max_track < last_track)  || (!inc_direction && min_track > last_track)) {
      inc_direction = !inc_direction;
    } 
  }

  int get_min_seek(int track) {
    if (inc_direction) {
      return track - last_track;
    } else {
      return last_track - track;
    }
  }

  bool get_direction() {return inc_direction;}

private:
  std::unique_ptr<std::vector<IORequest>> add_q;
  std::unique_ptr<std::vector<IORequest>> active_q;

  int last_track = -1;
  bool inc_direction = true; // if true head moves from 0 to n
  int max_track = -1;
  int min_track = 100000;
};