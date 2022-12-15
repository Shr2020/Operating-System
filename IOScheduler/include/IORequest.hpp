#pragma once

#include <array>
#include <iostream>
#include <string>


// Class the models the IORequest.
class IORequest {

private:
  int io_id = 0;
  int arrival_time = 0;
  int track_num = 0;

public:
  IORequest() {
    io_id = -1;
    arrival_time = 0;
    track_num = 0;
  }
  
  IORequest(int name, int at, int track) {
    io_id = name;
    arrival_time = at;
    track_num = track;
  }

  int get_id() { return io_id; }

  int get_at() {return arrival_time;}

  int get_track() {return track_num;}

  void to_string() {
    std::cout <<  "IO: " << io_id << " arrival: " << arrival_time 
    << " track: " << track_num << std::endl;
  }
};