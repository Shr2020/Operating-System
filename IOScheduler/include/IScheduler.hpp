#pragma once
#include "IORequest.hpp"

// Scheduler Interface
class IScheduler {
public:
  virtual void add_IORequest(IORequest io) = 0;
  virtual IORequest get_next_request() = 0;
  virtual bool has_next() = 0;
  virtual bool get_direction() = 0;
  virtual int reset() {return 0;}  
};