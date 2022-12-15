#include "Process.hpp"
#include "RandomNumberGenerator.hpp"

// Process constructor
Process::Process(std::string n, int at, int tct, int mcb, int mib, int mprio) {
  set_params(n, at, tct, mcb, mib, mprio);
}

// Process member function
void Process::set_params(std::string n, int at, int tct, int mcb, int mib, int mprio) {
  pn = n;
  arrival_time = at;
  total_cpu_time = tct;
  max_cpu_burst = mcb;
  max_io_burst = mib;

  // remaining cpu burst
  rem_cb = 0;
  cpu_burst = 0;

  // remaining io burst
  rem_ib = 0;
  io_burst = 0;

  // cpu time completed
  cpu_time_completed = 0;
  
  // static priority of each process
  static_priority = calc_stat_prio(mprio);

  // set/reset dynamic priority
  reset_dynamic_priority();

  // time the process entered the state
  state_ts = 0;

  // current state of the process
  state = State::CREATED;
}

void Process::set_cb(int qt) { cpu_burst = qt; }

void Process::set_rem_cb(int qt) { rem_cb = qt; }

void Process::set_rem_ib(int qt) { rem_ib = qt; }

void Process::set_state(State s) { state = s; }

void Process::set_state_ts(int ts) { state_ts = ts; }

void Process::set_static_priority(int p) { static_priority = p; }

void Process::set_dynamic_priority (int p) { dynamic_priority = p; }

void Process::reset_dynamic_priority () { dynamic_priority = static_priority - 1; }

// Calculates the new cpu burst using the random numbers provided.
int Process::calc_new_cpu_burst() {
  cpu_burst =
      1 + RandomNumberGenerator::instance().get_next_randnum() % max_cpu_burst;
  if (cpu_burst > total_cpu_time - cpu_time_completed) {
    cpu_burst = total_cpu_time - cpu_time_completed;
  }
  return cpu_burst;
}

// Calculates the new io burst using the random numbers provided.
int Process::calc_new_io_burst() {
  io_burst =
      1 + RandomNumberGenerator::instance().get_next_randnum() % max_io_burst;
  return io_burst;
}

// calculate the remaining cpu burst
void Process::calc_rem_cb(int qt) {
  rem_cb = rem_cb - qt;
}

// Calculates the remaining io burst
void Process::calc_rem_ib(int qt) {
  rem_ib = rem_ib - qt;
}

// calculates the cpu time completed
void Process::calc_cpu_time_completed(int cpu_time) {
  cpu_time_completed = cpu_time_completed + cpu_time;
};

// calculates the static priority
int Process::calc_stat_prio(int prio) {
  int static_prio =
      1 + RandomNumberGenerator::instance().get_next_randnum() % prio;
  return static_prio;
};

// update the dynamic priority.
void Process::update_dynamic_prio() {
    dynamic_priority--;
}
