#ifndef _WAVEFIELD_H_
#define _WAVEFIELD_H_

#include <stdio.h>

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


typedef struct laplacian_params {
    size_t order;
    size_t coef_len;
    size_t border_size;
    double dt;
    double dt_dx;
    double dt_dz;
    double dxdx;
    double dzdz;
    double *coef;
    double *coef_dx2;
    double *coef_dz2;
} laplacian_params;

wavefield * wavefield__create(size_t nx, size_t nz);

void wavefield__destroy(wavefield *w);

void wavefield__laplacian(wavefield *w, velocity_model *m, laplacian_params *lp);

void wavefield__swap(wavefield *wave);

laplacian_params * wavefield__laplacian_params_(double dx, double dz, size_t order, double dt);
laplacian_params * wavefield__laplacian_params(velocity_model *m, size_t order, double dt);

void wavefield__perfect_match_layer(wavefield *wave, velocity_model *model, laplacian_params *lp);

void wavefield__destroy_laplacian_params(laplacian_params *lp);

#endif
