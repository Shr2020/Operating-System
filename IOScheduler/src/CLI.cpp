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
#include <vector>

#include "IORequest.hpp"
#include "IScheduler.hpp"
#include "FIFO.hpp"
#include "SSTF.hpp"
#include "Simulator.hpp"
#include "FLOOK.hpp"
#include "LOOK.hpp"
#include "CLOOK.hpp"
#include "Summary.hpp"
#include "StatisticsTracker.hpp"

struct Args {
  bool verbose = false;
  bool f = false;
  bool q = false;
  char sched;
  std::string input_file;
};

Args parse_args(int argc, char **argv) {
  bool verbose = false;
  bool f = false;
  bool q = false;
  char *s_value = nullptr;
  int c;

  opterr = 0;

  Args args;

  while ((c = getopt(argc, argv, "vfqs:")) != -1) {
    switch (c) {
    case 'v':
      args.verbose = true;
      break;
    case 'f':
      args.f = true;
      break;
    case 'q':
      args.q = true;
      break;
    case 's':
      s_value = optarg;
      sscanf(s_value, "%c", &(args.sched));
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

  args.input_file = argv[optind];

  return args;
}

// perform pre-simulation initializations:
// read input file
// returns queue of io req 
void init(std::string filename, std::queue<IORequest> &ior, std::vector<StatisticsTracker> &stats) {
  std::fstream fi;
  fi.open(filename);
  std::string line;
  int io_num = 0;
  while (getline(fi, line)) {
    if (line[0] == '#') {
      continue;
    } else {
      char *cstr = new char[line.length() + 1];
      strcpy(cstr, line.c_str());
      char *p = strtok(cstr, " \t\n");
      int at = std::stoi(std::string(p));
      p = strtok(NULL, " \t\n");        
      int track = std::stoi(std::string(p));
      p = strtok(NULL, " \t\n");  
      assert(p == 0);
      ior.push(IORequest(io_num, at, track));
      stats.push_back(StatisticsTracker(io_num, at));
      io_num++;
    }
  }  
  fi.close();
}

int main(int argc, char **argv) {

  Args args = parse_args(argc, argv);
  std::queue<IORequest> ioReqs;
  std::vector<StatisticsTracker> stats;
  init(args.input_file, ioReqs, stats);
  std::unique_ptr<IScheduler> isched = nullptr;

  switch (args.sched) {
    case 'i':
      isched = std::make_unique<FIFO>();
      break;
    case 'j':
      isched = std::make_unique<SSTF>(); 
      break;
    case 's':
      isched = std::make_unique<LOOK>();
      break;
    case 'c':
      isched = std::make_unique<CLOOK>();
      break;
    case 'f':
      isched = std::make_unique<FLOOK>();
      break;
    default:
      throw std::runtime_error("Invalid memory management algorithm specified");
  }

  Summary sum;

  Simulator sim(isched.get());
  
  sim.run(ioReqs, stats, sum); 
  
  for (int i = 0; i < stats.size(); i++) {
    std::cout << stats.at(i).to_string();
  }

  sum.calc_summary(stats);

  std::cout << sum.to_string() << std::endl;
}