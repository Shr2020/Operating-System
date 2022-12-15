#include "Simulator.hpp"
#include "Des.hpp"
#include "FCFS.hpp"
#include "IScheduler.hpp"
#include "RandomNumberGenerator.hpp"
#include "StatisticsTracker.hpp"
#include "cassert"
#include <iostream>
#include <map>
#include <string>

namespace {
int new_ts = 0;

// when the process will transition to blocked state. (Goes into IO)
void running_to_block(Des &des, IScheduler *sched, Event &ev, Process *proc,
                      int curr) {
  // IO blocking
  new_ts = curr + proc->get_cb();
  if (proc->get_cpu_time_completed() + proc->get_cb() == proc->get_tct()) {
    des.put_event({new_ts, proc, State::DONE, State::RUNNING});
  } else {
    des.put_event({new_ts, proc, State::BLOCKED, State::RUNNING});
  }
}

// when the process will transition to ready state. (Premption case)
void running_to_ready(Des &des, IScheduler *sched, Event &ev, Process *proc,
                      int curr, int quantum) {
  //Preemption
  new_ts = curr + quantum;
  if (proc->get_cpu_time_completed() == proc->get_tct()) {
    des.put_event({new_ts, proc, State::DONE, State::RUNNING});
  } else {
    des.put_event({new_ts, proc, State::READY, State::RUNNING});
  }
}
} // namespace

void Simulator::run(Des &des, IScheduler *sched, int quantum,
                    std::map<std::string, StatisticsTracker> &stat_map, Summary &sum) {
    
  bool CALL_SCHEDULER = false;
  Process *running_process = nullptr;
  int new_timestamp = 0;

  // to keep accout of io_utilization
  int io_flag = 0;
  int io_ut = 0;
  int io_time = 0; // time to track

  while (des.has_events()) {
    Event ev = des.get_event();
    Process *proc = ev.get_process();
    int current_time = ev.get_timestamp();
    int time_in_previous_state = current_time - proc->get_state_ts();

    switch (ev.get_transition()) {

    // BLOCKED -> READY or CREATED -> READY. Process is ready to run.
    case State::TRANS_TO_READY:
      sched->add_process(proc);
      CALL_SCHEDULER = true;

      // process has come after completing IO
      if (proc->get_state() == State::BLOCKED) {
        (stat_map.at(proc->get_name())).set_io_time(time_in_previous_state);
        
        io_flag--;
        if (io_flag == 0) {
          io_ut += current_time - io_time;
          io_time = current_time;
        }
      }

      // set the state of process and the corresponding time_stamp
      proc->set_state(State::READY);
      proc->set_state_ts(ev.get_timestamp());

      // check the priority of this process with currently running process and 
      // preempt it if the process has more priority.
      if(sched->preempt_running_proc() && running_process != nullptr && 
          running_process->get_dynamic_priority() < proc->get_dynamic_priority()) {
            
            int running_proc_terminate_time = running_process->get_tct() - running_process->get_cpu_time_completed();
            int running_proc_run_time = current_time - running_process->get_state_ts();
            
            if (running_proc_terminate_time == running_proc_run_time 
                || running_proc_run_time == running_process->get_cb()){
              break;
            }
            des.rm_event(running_process);
            des.put_event({current_time, running_process, State::READY, State::RUNNING});
            running_process=nullptr;
      }
      break;

    // READY -> RUNNING. Process has started running.
    case State::TRANS_TO_RUNNING:
      proc->set_state(State::RUNNING);
      proc->set_state_ts(ev.get_timestamp());

      (stat_map.at(proc->get_name())).set_cpu_waiting_time(time_in_previous_state);

      if (!(proc->get_cpu_time_completed() == proc->get_tct()) &&
          proc->get_rem_cb() == 0) {
        proc->calc_new_cpu_burst();
        proc->set_rem_cb(proc->get_cb());
      }
      
      // if quantum is less the the current burst then the process will be preempted by CPU. 
      // Otherwise it will be blocked for IO after finishing its burst.
      if (quantum < proc->get_cb()) {
        running_to_ready(des, sched, ev, proc, current_time, quantum);
      } else {
        running_to_block(des, sched, ev, proc, current_time);
      }
      break;

    // RUNNING -> BLOCKED. Process is blocked by IO.
    case State::TRANS_TO_BLOCKED:

      // The Running Process is blocked now. so no process currently running. Set the pointer to null.
      running_process = nullptr;

      // Update the process
      proc->set_state(State::BLOCKED);
      proc->set_state_ts(ev.get_timestamp());
      proc->calc_cpu_time_completed(proc->get_cb());
      proc->calc_rem_cb(proc->get_cb());
      proc->set_cb(proc->get_rem_cb());

      // Check if the Process has reached the completeion time. IF not, then calculate new io_burst.
      if (!(proc->get_cpu_time_completed() == proc->get_tct()) &&
          proc->get_rem_ib() == 0) {
        proc->calc_new_io_burst();
        proc->set_rem_ib(proc->get_ib());
      }
      if (io_flag == 0) {
        io_time = current_time;
      }
      io_flag++;

      // Event details for when the process will be ready again.
      new_timestamp = current_time + proc->get_ib();
      proc->calc_rem_ib(proc->get_ib());
      des.put_event({new_timestamp, proc, State::READY, State::BLOCKED});
      proc->reset_dynamic_priority();
      CALL_SCHEDULER = true;
      break;

    // RUNNING -> READY. Process is ready to run.
    case State::TRANS_TO_PREEMPT:

      // update the process.
      proc->set_state(State::READY);
      proc->set_state_ts(ev.get_timestamp());
      proc->calc_cpu_time_completed(time_in_previous_state);
      proc->calc_rem_cb(time_in_previous_state);
      proc->set_cb(proc->get_rem_cb());
      running_process = nullptr;

      // decrease dynamic prio as the process has been preempted.
      proc->update_dynamic_prio();
      sched->add_process(proc);
      CALL_SCHEDULER = true;
      break;

    // RUNNING -> DONE. Process completed.
    case State::TRANS_TO_DONE:
      (stat_map.at(proc->get_name())).set_finishing_time(ev.get_timestamp());
      (stat_map.at(proc->get_name())).set_turnaround_time(stat_map.at(proc->get_name()).calc_turnaround_time());
      running_process = nullptr;
      CALL_SCHEDULER = true;
      break;

    default:
      throw std::runtime_error("Invalid state transition");
    }
    if (CALL_SCHEDULER) {
      if (des.has_events() &&
          des.peek_next_event().get_timestamp() == current_time) {
        continue;
      }
      CALL_SCHEDULER = false;
      if (running_process == nullptr) {
        running_process = sched->get_next_process();
        if (running_process == nullptr) {
          continue;
        }
        des.put_event({current_time, running_process, State::RUNNING, State::READY});
      }
    }
  }
  sum.set_io_utilization(double(io_ut));
}