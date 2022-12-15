#pragma once

#include "Event.hpp"
#include <functional>
#include <iostream>
#include <queue>

// Wrapper for Event objects keeping track of the sequence in which Events arrive
// Useful for breaking ties among Events having the same timestamp
struct EventWrapper {
  Event event;
  int sequence_number;
};

// Custom comparator for EventWrapper
struct EventQCmp {
  bool operator()(EventWrapper const &e1, EventWrapper const &e2) {
    if (e1.event.get_timestamp() == e2.event.get_timestamp()) {
      return e1.sequence_number > e2.sequence_number;
    }
    
    return e1.event.get_timestamp() > e2.event.get_timestamp();
  }
};

// Discrete Event Simulator
class Des {

private:
  std::priority_queue<EventWrapper, std::vector<EventWrapper>, EventQCmp> event_q;
  int counter_ = 0;

public:
  bool has_events() { return !event_q.empty(); }

  Event get_event() {
    auto e = event_q.top();
    event_q.pop();
    return e.event;
  }

  void put_event(Event event) {
    event_q.push({event, counter_++});
  }

  void rm_event(Process* p) {
    std::priority_queue<EventWrapper, std::vector<EventWrapper>, EventQCmp> copyQ;
    while(!event_q.empty()) {
      auto e = event_q.top();
      event_q.pop();
      if(e.event.get_process() == p) {
        continue;
      } else {
        copyQ.push(e);
      }
    }
    event_q = copyQ;
  }

  Event peek_next_event() {
    auto e = event_q.top();
    return e.event;
  }
};