#!/bin/bash

make clean
make create_model create_source get_dt wavempi  d2f d2a wave

# VMODEL PARAMS
nx=600
nz=600
vmin=1500
vmax=1500
vel=1500
dx=1
dz=1
velmodel="const.bin"

# Source params
freq=20
wavelet="ricker20.bin"
sx=50
sz=50

# Simulation params
t=1
dt=$(./get_dt -dx "${dx}" -dz "${dz}" -vmax "${vmax}" -vmin "${vmin}" -freq "${freq}")
sample=0.008
order=4

./create_model -nx "${nx}" -nz "${nz}" -dx "${dx}" -dz "${dz}" -vel "${vel}" > "${velmodel}"
./create_source -freq "${freq}" -time "${t}" -dt "${dt}" > "${wavelet}"


#./wave  -time "${t}" -sample "${sample}" -order "${order}" \
#		  -nx "${nx}" -nz "${nz}" -dx "${dx}" -dz "${dz}" -vel "${vel}" \
#		  -freq "${freq}" -sx "${sx}" -sz "${sz}" -verbose > w2.bin
#
mpirun -hostfile lamhosts -n 4 \
./wavempi -time "${t}" -dt "${dt}" -sample "${sample}" -order "${order}" \
		  -nx "${nx}" -nz "${nz}" -dx "${dx}" -dz "${dz}" -vel "${vel}" -model "${velmodel}" \
		  -freq "${freq}" -sx "${sx}" -sz "${sz}" -source "${wavelet}"  -verbose




