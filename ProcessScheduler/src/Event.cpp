#include "Event.hpp"
#include "Process.hpp"
#include <stdexcept>

// Event constructor
Event::Event(int ts, Process *p, State ns, State os) {
  timestamp_ = ts;
  process_ = p;
  new_state_ = ns;
  old_state_ = os;
};

// Transition of states
State Event::get_transition() {
  State state;
  if (old_state_ == State::CREATED && new_state_ == State::READY) {
    state = State::TRANS_TO_READY;
  } else if (old_state_ == State::READY && new_state_ == State::RUNNING) {
    state = State::TRANS_TO_RUNNING;
  } else if (old_state_ == State::RUNNING && new_state_ == State::READY) {
    state = State::TRANS_TO_PREEMPT;
  } else if (old_state_ == State::BLOCKED && new_state_ == State::READY) {
    state = State::TRANS_TO_READY;
  } else if (old_state_ == State::RUNNING && new_state_ == State::BLOCKED) {
    state = State::TRANS_TO_BLOCKED;
  } else if (old_state_ == State::RUNNING && new_state_ == State::DONE) {
    state = State::TRANS_TO_DONE;
  } else {
    throw std::runtime_error("Invalid state transition");
  }
  return state;
}