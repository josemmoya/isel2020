#!bin/bash

spin -a model.pml
gcc -o pan pan.c
./pan -a -f -N $1
spin -t model.pml
