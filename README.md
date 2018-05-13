Wave Phenomena Studies
======================

Some tools to study wave phenomena computational and mathematical implications


TO INSTALL
==========

1. Clone repository

```
$ git clone https://github.com/igormorgado/wave
```

2. Build
```
$ cd wave
# make wave
```

3. If you have Seismic Unix correctly installed. Test with:

```
$ ./run_test.sh
```


RECOMENDATIONS
==============

Is good to install seismic unix for visualization tool


HOW TO USE
==========

**wave**: creates a wavefield.

**oldwave**: Creates a wavefield (deprecated).

**d2a**: convert binary double data to ascii data (to fill empty disks)

**d2f**: convert binary double data to binary float data (necessary to use seismic unix visualization tools

**bfdiff**: calculate the difference between two binary float files 

**bddiff**: calculate the difference between two binary double files 

For more infor check command helps.


USE CASES
=========

1. Testing residuals from 8th order and 2th order

```
./wave -order 8 -t 0.25 > wave8th.bin
./wave -order 2 -t 0.25 > wave2th.bin
./bddiff wave2th.bin < wave8th.bin | ./d2f | xmovie n1=500 n2=500 d1=1 d2=1 clip=0.01 loop=2
```
