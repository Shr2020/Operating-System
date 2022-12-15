#pragma once

#include <string>

// Different states of the Processes. Also includes the Transition states for the process.
enum class State {
  CREATED = 0,
  READY,
  RUNNING,
  BLOCKED,
  DONE,

  // BLOCKED -> READY or CREATED -> READY 
  TRANS_TO_READY,

  // READY -> RUNNING
  TRANS_TO_RUNNING,

  // RUNNING -> BLOCKED
  TRANS_TO_BLOCKED,

  //RUNNING -> READY
  TRANS_TO_PREEMPT,

  // RUNNING -> DONE
  TRANS_TO_DONE
};

inline std::string make_string(State state) {
  switch (state) {
  case State::CREATED:
    return "CREATED";
  case State::READY:
    return "READY";
  case State::RUNNING:
    return "RUNNING";
  case State::BLOCKED:
    return "BLOCKED";
  case State::DONE:
    return "DONE";
  default:
    return "State::TRANS*";
  }
}