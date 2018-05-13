#!/bin/bash

make clean
make wave
make d2f
rm -f w.bin

./wave -time 1 > w.bin
./d2f < w.bin |xmovie n1=500 n2=500 d1=1.000000 d2=1.000000 clip=0.5 loop=2 
