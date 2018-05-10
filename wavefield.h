#ifndef _WAVEFIELD_H_
#define _WAVEFIELD_H_

#include <stdio.h>

#include "utils.h"
#include "globals.h"
#include "velocity_model.h"

// cfd: Central Finite Difference

typedef struct wavefield
{
    size_t nx;
    size_t nz;
    double *grid;
    double *grid_o;
} wavefield;

wavefield * wavefield__create(size_t nx, size_t nz);

void wavefield__destroy(wavefield *w);

void wavefield__laplacian2(wavefield *w, velocity_model *m);
void wavefield__laplacian4(wavefield *w, velocity_model *m);
void wavefield__laplacian6(wavefield *w, velocity_model *m);
void wavefield__laplacian8(wavefield *w, velocity_model *m);

#endif
