## Objective

1. Implement various process scheduling algorithms.
2. The system is to be implemented using Discrete Event Simulation (DES) (http://en.wikipedia.org/wiki/Discrete_event_simulation).
3. The scheduling algorithms to be simulated are:
FCFS, LCFS, SRTF, RR (RoundRobin), PRIO (PriorityScheduler) and PREemptive PRIO (PREPRIO).


## Building and running the scheduler simulator

Requirements: gcc 9.2

Run `make all` to compile the program. The executable `sched` is created in the `build` directory.
The `build` directory is created automatically if it does not exist.

```
% make all
% ./build/sched -v -s F ../inputs/input1 ../inputs/rfile
```

## Cleaning the workspace

```
% make clean
```