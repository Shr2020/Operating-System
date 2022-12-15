## Objective

To implement/simulate the scheduling and optimization of I/O operations. Applications submit their IO requests to the IO subsystem (potentially via the filesystem and page-cache), where they are maintained in an IO-queue until the disk device is ready for servicing another request. The IO-scheduler then selects a request from the IO-queue and submits it to the disk device. This selection is commonly known as the strategy()routine in operating systems. On completion, another request can be taken from the IO-queue and submitted to the disk. The scheduling policies will allow for some optimization as to reduce disk head movement or overall wait time in the system. 

The schedulers to be implemented are **FIFO (i), SSTF (j), LOOK (s), CLOOK (c), and FLOOK (f)** (the letters in bracket define which parameter must be given in the –s program flag).

## Building and running the iosched simulator

Requirements: gcc 9.2

Run `make all` to compile the program. The executable `iosched` is created in the `build` directory.
The `build` directory is created automatically if it does not exist.

```
% make all
% ./build/iosched -si input1 
```

## Cleaning the workspace

```
% make clean
```
