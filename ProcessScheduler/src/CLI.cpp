#include <cassert>
#include <cstring>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits>
#include <memory>
#include <map>

#include "Des.hpp"
#include "FCFS.hpp"
#include "LCFS.hpp"
#include "RR.hpp"
#include "PREPRIO.hpp"
#include "PRIO.hpp"
#include "SRTF.hpp"
#include "IScheduler.hpp"
#include "RandomNumberGenerator.hpp"
#include "Simulator.hpp"
#include "Summary.hpp"

using ProcessVec = std::vector<std::unique_ptr<Process>>;

struct Args {
  bool verbose = false;
  bool t = false;
  bool e = false;
  char sched;
  int quantum = std::numeric_limits<int>::max();
  int max_priority = 4;
  std::string input_file;
  std::string rand_file;
};

Args parse_args(int argc, char **argv) {
  bool verbose = false;
  bool t = false;
  bool e = false;
  char *s_value = nullptr;
  int c;

  opterr = 0;

  Args args;

  while ((c = getopt(argc, argv, "vtes:")) != -1) {
    switch (c) {
    case 'v':
      args.verbose = true;
      break;
    case 't':
      args.t = true;
      break;
    case 'e':
      args.e = true;
      break;
    case 's':
      s_value = optarg;
      sscanf(s_value, "%c%d:%d", &(args.sched), &(args.quantum),
             &(args.max_priority));
      break;
    case '?':
      if (optopt == 's') {
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint(optopt)) {
        fprintf(stderr, "Unknown option '-%c'.\n", optopt);
      } else {
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      throw std::runtime_error("Error parsing command line args");
    default:
      throw std::runtime_error("Error parsing command line args");
    }
  }

  if (optind + 2 != argc) {
      throw std::runtime_error("Can't parse cmd line args. Check non-option args");
  }
  args.input_file = argv[optind];
  args.rand_file = argv[optind + 1];

  return args;
}

// perform pre-simulation initializations:
// read input file, create initial Events
// returns vector of processes and per process StatisticsTrackers
std::pair<ProcessVec, std::map<std::string, StatisticsTracker>>
init(Des &des, std::string filename, int mprio) {
  std::fstream fi;
  fi.open(filename);

  std::string line;
  int i = 0;
  ProcessVec v;
  while (getline(fi, line)) {
    char *cstr = new char[line.length() + 1];
    strcpy(cstr, line.c_str());
    char *p = strtok(cstr, " \t\n");
    int at = std::stoi(std::string(p));
    p = strtok(NULL, " \t\n");
    int tct = std::stoi(std::string(p));
    p = strtok(NULL, " \t\n");
    int mcb = std::stoi(std::string(p));
    p = strtok(NULL, " \t\n");
    int mib = std::stoi(std::string(p));
    p = strtok(NULL, " \t\n");
    assert(p == 0);
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << i;
    std::string name = ss.str();
    auto pr = std::make_unique<Process>(name, at, tct, mcb, mib, mprio);
    v.push_back(std::move(pr));
    i++;
  }

  std::map<std::string, StatisticsTracker> stat_map;
  for (int j = 0; j < v.size(); j++) {
    Process *pr = v.at(j).get();
    pr->set_state_ts(pr->get_at());
    pr->set_state(State::READY);
    Event ev(pr->get_at(), pr, State::READY, State::CREATED);
    des.put_event(ev);
    stat_map[pr->get_name()] = StatisticsTracker(pr);
  }

  fi.close();

  return {std::move(v), stat_map};
}

// Returns the process stats in string format.
std::string get_process_stats(std::map<std::string, StatisticsTracker> const &m) {
  std::stringstream ss;
  for (auto &entry : m) {
    Process *proc = entry.second.get_proc();
    ss << entry.first << ": " << std::setw(4) << proc->get_at() << " "
       << std::setw(4) << proc->get_tct() << " " << std::setw(4)
       << proc->get_mcb() << " " << std::setw(4) << proc->get_mib()
       << " " << std::setw(1) << entry.second.get_sp() << " | "
       << std::setw(5) << entry.second.get_ft() << " " << std::setw(5)
       << entry.second.get_tt() << " " << std::setw(5)
       << entry.second.get_iot() << " " << std::setw(5)
       << entry.second.get_cwt() << " " << std::endl;
  }
  return ss.str();
}

int main(int argc, char **argv) {
  
  Args args = parse_args(argc, argv);
  RandomNumberGenerator::instance().init(args.rand_file);
  Des des;
  
  auto ret = init(des, args.input_file, args.max_priority);
  auto stat_map = ret.second;
  Summary summary;
  Simulator sim;
  std::unique_ptr<IScheduler> sched = nullptr;

  switch (args.sched) {
    case 'F':
      sched = std::make_unique<FCFS>();
      std::cout << "FCFS" << std::endl; 
      break;
    case 'L':
      sched = std::make_unique<LCFS>();
      std::cout << "LCFS" << std::endl; 
      break;
    case 'S':
      sched = std::make_unique<SRTF>();
      std::cout << "SRTF" << std::endl; 
      break;
    case 'P':
      sched = std::make_unique<PRIO>(args.max_priority);
      std::cout << "PRIO " << args.quantum << std::endl; 
      break;
    case 'R':
      sched = std::make_unique<RR>();
      std::cout << "RR " << args.quantum << std::endl; 
      break;
    case 'E':
      sched = std::make_unique<PREPRIO>(args.max_priority);
      std::cout << "PREPRIO " << args.quantum << std::endl; 
      break;
    default:
      throw std::runtime_error("Invalid scheduler specified");
  }
  
  // run the simulation
  sim.run(des, sched.get(), args.quantum, stat_map, summary);

  // stat_map now contains stats for each process
  std::cout << get_process_stats(stat_map) << std::endl;

  // prepare and print the summary of the simulation
  summary.calc_summary(stat_map);
  std::cout << summary.to_string() << std::endl;

  return 0;
}