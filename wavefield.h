#ifndef _WAVEFIELD_H_
#define _WAVEFIELD_H_

#include "utils.h"
#include "velocity_model.h"

// cfd: Central Finite Difference

// 2nd derivative
// TODO: Programaticaly find the coefficients for any derivative in any order
double cfd2nd_2th_order[2] = { -2, 1};
double cfd2nd_4th_order[3] = { -5/2, 4/3, -1/12};
double cfd2nd_6th_order[4] = { -49/18, 3/2, -3/20, 1/90 };
double cfd2nd_8th_order[5] = { -205/72, 8/5, -1/5, 8/315, -1/560 };

typedef struct wavefield
{
    size_t nx;
    size_t nz;
    double *grid;
    double *grid_o;
} wavefield;

wavefield * wavefield__create(size_t nx, size_t nz);
void * wavefield__destroy(wavefield *w);

#endif
