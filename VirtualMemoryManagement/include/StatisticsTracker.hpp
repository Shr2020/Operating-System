#pragma once

#include "Process.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

/**
Class that maintains process specific stats
*/
class StatisticsTracker {
private:
    int pid = 0;
    unsigned long unmaps = 0; 
    unsigned long maps = 0; 
    unsigned long ins = 0; 
    unsigned long outs = 0;
    unsigned long fins = 0; 
    unsigned long fouts = 0; 
    unsigned long zeros = 0;
    unsigned long segv = 0; 
    unsigned long segprot = 0;

public:

  // Constructors
  StatisticsTracker() {}
  StatisticsTracker(int p_id) { 
    pid = p_id;
    unmaps = 0; 
    maps = 0; 
    ins = 0; 
    outs = 0;
    fins = 0; 
    fouts = 0; 
    zeros = 0;
    segv = 0; 
    segprot = 0;
  }

  unsigned long get_unmaps() {
      return unmaps;
  }

  unsigned long get_maps() {
      return maps;
  }

  unsigned long get_ins() {
      return ins;
  }

  unsigned long get_outs() {
      return outs;
  }

  unsigned long get_fins() {
      return fins;
  }

  unsigned long get_fouts() {
      return fouts;
  }

  unsigned long get_zeros() {
      return zeros;
  }

  unsigned long get_segv() {
      return segv;
  }

  unsigned long get_segprot() {
      return segprot;
  }

  void increment_unmaps() {
      unmaps++;
  }

  void increment_maps() {
      maps++;
  }

  void increment_ins() {
      ins++;
  }

  void increment_outs() {
      outs++;
  }

  void increment_fins() {
      fins++;
  }

  void increment_fouts() {
      fouts++;
  }

  void increment_zeros() {
      zeros++;
  }

  void increment_segv() {
      segv++;
  }

  void increment_segprot() {
      segprot++;
  }

  std::string to_string() {
    std::stringstream ss;
    
    ss << "PROC[" << pid << "]: " << "U=" << unmaps 
    << " M=" << maps << " I=" << ins << " O=" << outs 
    << " FI=" << fins << " FO=" << fouts << " Z=" << zeros 
    << " SV=" << segv << " SP=" << segprot;

    return ss.str();
  }
};