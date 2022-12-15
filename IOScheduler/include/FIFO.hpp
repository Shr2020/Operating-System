#pragma once
#include "IScheduler.hpp"
#include <iostream>
#include <queue>

// Class that models First Come First Serve scheduler 
class FIFO : public IScheduler {
public:
  void add_IORequest(IORequest io) { 
    run_q.push(io); 
  }

  IORequest get_next_request() {
    IORequest io= run_q.front();
    update_dir(io.get_track());
    last_track = io.get_track();
    run_q.pop();
    return io;
  }

  bool has_next() { return !run_q.empty(); }

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

  bool get_direction() {return inc_direction;}

private:
  std::queue<IORequest> run_q;
  bool inc_direction = true;
  int last_track = -1;
};