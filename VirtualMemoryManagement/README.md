## Objective

To implement/simulate the operation of an Operating System’s Virtual Memory Manager which maps the virtual address spaces of multiple processes onto physical frames using page table translation. Multiple processes are assumed, each with its own virtual address space of exactly 64 virtual pages (small compared to the 1M entries for a full 32-address architecture but the principle counts). As the sum of all virtual pages in all virtual address spaces may exceed the number of physical frames of the simulated system, paging needs to be implemented. The number of physical page frames varies and is specified by a program option; support up to 128 frames should be provided.

Following page replacement algorithms are to be implemented (letter indicates program option (see below)):
1. FIFO(F)
2. Random(R)
3. Clock(C)
4. Enhanced Second Chance / NRU (E)
5. Aging (A)
6. Working Set (W)


#### INPUT SPECIFICATION:
The input to your program will be a comprised of:
1. the number of processes (processes are numbered starting from 0)
2. a specification for each process’ address space is comprised of
 1. the number of virtual memory areas / segments (aka VMAs) 
 2. specification for each said VMA comprised of 4 numbers: “starting_virtual_page ending_virtual_page write_protected[0/1] filemapped[0/1]”

```
Sample:
#process/vma/page reference generator
#procs=2 #vmas=2 #inst=100 pages=64 %read=75.000000 lambda=1.000000
#holes=1 wprot=1 mmap=1 seed=19200
2
####process 0
2
0 42 0 0
43 63 1 0
####process 1
3
0 17 0 1
20 60 1 0
62 63 0 0
```

## Building and running the mmu simulator

Requirements: gcc 9.2

Run `make all` to compile the program. The executable `mmu` is created in the `build` directory.
The `build` directory is created automatically if it does not exist.

```
% make all
% ./build/mmu -f4 -ac -oOPFS ../inputs/input1 ../inputs/rfile
```

## Cleaning the workspace

```
% make clean
```