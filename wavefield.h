#ifndef _WAVEFIELD_H_
#define _WAVEFIELD_H_

#include "utils.h"
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

#endif
