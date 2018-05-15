#!/bin/bash

make clean
make wave
make d2f
rm -f w.bin

./wave -x 600 -z 600 -time 1 > w.bin
./d2f < w.bin |xmovie n1=600 n2=600 d1=1.000000 d2=1.000000 clip=0.2 loop=2 
