# Readme

To run on the linserv1.cims.nyu.edu machine of NYU, firstly switch the compiler version:

module load gcc-9.2



To compile the code:

make

The executable is "linker" and is under the current folder.



To clean it up:

make clean


## error case:

1. Fail to detect a usecount missing error.

################### input-25##################
###    Warning Diff:    .....
0a1
> Warning: Module 1: cde123 appeared in the uselist but was not actually used
###    Code/Error Diff: .....
1c1,2
< Parse Error line 2 offset 6: NUM_EXPECTED
---
> Symbol Table
> Memory Map
