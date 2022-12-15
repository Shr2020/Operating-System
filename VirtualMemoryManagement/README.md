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