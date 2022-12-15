#pragma once

struct PTE {
  // class members initialization
  PTE() : present(0) , referenced(0) , modified(0), write_protect(0), pagedout(0), 
    frame_ind(0), filemapped(0), extrainfo(0)   {}
  unsigned present:1;
  unsigned referenced:1;
  unsigned modified:1;
  unsigned write_protect:1;
  unsigned pagedout:1;
  unsigned frame_ind:7;
  unsigned filemapped:1;
  unsigned extrainfo:19;

  void reset_PTE() {
    present = 0;
    referenced = 0;
    modified = 0;
    write_protect = 0;
    frame_ind = 0;
    filemapped = 0;
    extrainfo = 0;
  }
};