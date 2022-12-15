#pragma once

#include "Process.hpp"
#include "State.hpp"

// class that models Events for the Discrete Event Simulator 
class Event {
private:
  int timestamp_;
  Process *process_;
  State new_state_;
  State old_state_;

public:
  Event(int ts, Process *p = nullptr, State ns = State::CREATED,
        State os = State::CREATED);

  int get_timestamp() const { return timestamp_; }
  State get_new_state() const { return new_state_; }
  State get_old_state() const { return old_state_; }
  Process *get_process() const { return process_; }
  State get_transition();
};