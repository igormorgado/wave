#!/bin/bash

make clean
make create_model create_source get_dt wavempi

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
sx=1
sz=1

# Simulation params
t=2
dt=$(./get_dt -dx "${dx}" -dz "${dz}" -vmax "${vmax}" -vmin "${vmin}" -freq "${freq}")
sample=0.008
order=4

./create_model -nx "${nx}" -nz "${nz}" -dx "${dx}" -dz "${dz}" -vel "${vel}" > "${velmodel}"
./create_source -freq "${freq}" -time "${t}" -dt "${dt}" > "${wavelet}"


mpirun -hostfile lamhosts -n 9 \
./wavempi -time "${t}" -dt "${dt}" -sample "${sample}" -order "${order}" \
		  -nx "${nx}" -nz "${nz}" -dx "${dx}" -dz "${dz}" -vel "${vel}" -model "${velmodel}" \
		  -freq "${freq}" -sx "${sx}" -sz "${sz}" -source "${wavelet}" 




