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

#include "Process.hpp"
#include "VMA.hpp"
#include "Pager.hpp"
#include "FIFO.hpp"
#include "Simulator.hpp"
#include "Clock.hpp"
#include "NRU.hpp"
#include "Aging.hpp"
#include "WorkingSet.hpp"
#include "Random.hpp"
#include "Summary.hpp"
#include "StatisticsTracker.hpp"

using ProcessVec = std::vector<Process*>;
using InstructVec = std::vector<std::pair<std::string, int>>;

struct Args {
  char algo;
  bool out = false;
  bool po = false;
  bool fto = false;
  bool summary = false;
  bool print_cpt = false;
  bool pt_allproc = false;
  bool print_ft = false;
  bool aging_info = false;
  int num_frames = 0;
  std::string input_file;
  std::string rand_file;
};

Args parse_args(int argc, char **argv) {
  std::string o_value;
  char *a_value = nullptr;
  char *f_value = nullptr;
  int c;

  opterr = 0;

  Args args;

  while ((c = getopt(argc, argv, "f:a:o:")) != -1) {
    switch (c) {
    case 'f':
      f_value = optarg;
      sscanf(f_value, "%d", &(args.num_frames));
      break;
    case 'a':
      a_value = optarg;
      sscanf(a_value, "%c", &(args.algo));
      break;
    case 'o':
      o_value = std::string(optarg);
      for (int i = 0; i < o_value.size(); i++) {
        if (o_value[i] == 'O') {
          args.out = true;
        }
        else if (o_value[i] == 'P') {
          args.po = true;
        }
        else if (o_value[i] == 'F') {
          args.fto = true;
        }
        else if (o_value[i] == 'S') {
          args.summary = true;
        }
        else if (o_value[i] == 'x') {
          args.print_cpt = true;
        }
        else if (o_value[i] == 'y') {
          args.pt_allproc = true;
        }
        else if (o_value[i] == 'f') {
          args.print_ft = true;
        }
        else if (o_value[i] == 'a') {
          args.aging_info = true;
        } else {
          throw std::runtime_error("Incorrect option for -o. Choose again.");
        }
      } 
      break;
    case '?':
      if (optopt == 'f' || optopt == 'a') {
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
// read input file
// returns vector of processes 
void init(std::string filename, ProcessVec &pv, InstructVec &insts) {
  std::fstream fi;
  fi.open(filename);
  std::string line;
  int i = 0;
  int num_proc = 0;
  while (getline(fi, line)) {
    if (line[0] == '#') {
      continue;
    } else {
      num_proc = std::stoi(line);
      break;
    }
  }
  int name = 0;
  while (num_proc > 0 && getline(fi, line)) {
    if (line[0] == '#') {
      continue;
    }
    char *cstr = new char[line.length() + 1];
    strcpy(cstr, line.c_str());
    char *p = strtok(cstr, " \t\n");
    auto pr = new Process(name);
    int num_vma = std::stoi(std::string(p));
    while (num_vma > 0 && getline(fi, line)) {
      if (line[0] == '#') {
        continue;
      }
      char *cstr2 = new char[line.length() + 1];
      strcpy(cstr2, line.c_str());
      p = strtok(cstr2, " \t\n");
      int sv = std::stoi(std::string(p));
      p = strtok(NULL, " \t\n");        
      int ev = std::stoi(std::string(p));
      p = strtok(NULL, " \t\n");
      int wp = std::stoi(std::string(p));
      p = strtok(NULL, " \t\n");
      int fm = std::stoi(std::string(p));
      p = strtok(NULL, " \t\n");
      assert(p == 0);

      // create vma object
      VMA vma(sv, ev, wp, fm);

      // put it in vma_list vector
      pr->add_vma(vma);
      num_vma--;
    } 
    
    pv.push_back(pr);
    num_proc--;
    name++;
  }

  while(getline(fi, line)) {
    if (line[0] == '#') {
      continue;
    }
    char *cstr = new char[line.length() + 1];
    strcpy(cstr, line.c_str());
    char *p = strtok(cstr, " \t\n");
    std::string c = std::string(p);
    p = strtok(NULL, " \t\n");        
    int i = std::stoi(std::string(p));
    insts.push_back(std::make_pair(c, i));
  }
  
  fi.close();
}


int main(int argc, char **argv) {

  Args args = parse_args(argc, argv);
  RandomNumberGenerator::instance().init(args.rand_file);
  ProcessVec pv;
  InstructVec instructions;
  init(args.input_file, pv, instructions);
  std::unique_ptr<Pager> pager = nullptr;

  // init global frame table
  std::vector<Frame*> frame_table_global;
  for (int i = 0; i < args.num_frames; i++) {
    frame_table_global.push_back(new Frame(i));
  }

  switch (args.algo) {
    case 'f':
      pager = std::make_unique<FIFO>(frame_table_global, args.num_frames);
      break;
    case 'r':
      pager = std::make_unique<Random>(frame_table_global, args.num_frames); 
      break;
    case 'c':
      pager = std::make_unique<Clock>(frame_table_global, args.num_frames);
      break;
    case 'e':
      pager = std::make_unique<NRU>(frame_table_global, args.num_frames);
      break;
    case 'a':
      pager = std::make_unique<Aging>(frame_table_global, args.num_frames);
      break;
    case 'w':
      pager = std::make_unique<WorkingSet>(frame_table_global, args.num_frames);
      break;
    default:
      throw std::runtime_error("Invalid memory management algorithm specified");
  }

  std::vector<StatisticsTracker> stats;

  for (int i = 0; i < pv.size(); i++) {
    stats.push_back({i});
  }

  Simulator sim(pager.get(), args.num_frames, frame_table_global);
  Summary sum(frame_table_global, pv);
  sim.run(instructions, pv, stats, sum); 
  if (args.po) {
    std::cout << sum.print_page_table() << std::endl;
  }

  if (args.fto) {
    std::cout << sum.print_frame_table() << std::endl;
  }
  
  for(int i = 0; i < stats.size(); i++) {
    std::cout << stats.at(i).to_string() << std::endl;
  }

  sum.calculate_total_cost(stats);

  if (args.summary) {
    std::cout << sum.print_total_cost() << std::endl;
  }

  for (int i = 0; i < pv.size(); i++) {
    delete(pv.at(i));
  }

  for (int i = 0; i < frame_table_global.size(); i++) {
    delete(frame_table_global.at(i));
  }
}