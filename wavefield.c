#include <stdlib.h>
#include <math.h>

#include "wavefield.h"

wavefield * wavefield__create(size_t nx, size_t nz) {
    wavefield *w = malloc(sizeof (wavefield));
    w->nx = nx;
    w->nz = nz;
    w->grid   = calloc(nx * nz, sizeof (double));
    w->grid_o = calloc(nx * nz, sizeof (double));

    return w;
}

void wavefield__destroy(wavefield *w) 
{
    free(w->grid);
    free(w->grid_o);
    free(w);
}


laplacian_params * wavefield__laplacian_params(velocity_model *m, size_t order, double dt)
{
    laplacian_params * lp = malloc(sizeof (laplacian_params));

    // Finite Difference Coefficients - Second Derivative - nth order
    // TODO: Find algorithimicaly 
    switch(order) {
        case 2:
            lp->coef_len = 2;
            lp->coef = malloc(lp->coef_len * sizeof(double)); 
            lp->coef[0] = -2.;
            lp->coef[1] = 1.;
            break;
        case 4:
            lp->coef_len = 3;
            lp->coef = malloc(lp->coef_len * sizeof(double)); 
            lp->coef[0] = -5./2;
            lp->coef[1] = 4./3.;
            lp->coef[2] = -1./12;
            break;
        case 6:
            lp->coef_len = 4;
            lp->coef = malloc(lp->coef_len * sizeof(double)); 
            lp->coef[0] = -49.18;
            lp->coef[1] = 3./2;
            lp->coef[2] = -3./20;
            lp->coef[3] = 1./90;
            break;
        case 8:
            lp->coef_len = 5;
            lp->coef = malloc(lp->coef_len * sizeof(double)); 
            lp->coef[0] = -205./72;
            lp->coef[1] = 8./5;
            lp->coef[2] = -1./5;
            lp->coef[3] = 8./315;
            lp->coef[4] = -1./560;
            break;
    }

    lp->order = order;
    lp->border_size = lp->coef_len - 1;      // Size of border depends on stencil

    lp->dt = dt;
    lp->dt_dx = lp->dt/m->dx;
    lp->dt_dz = lp->dt/m->dz;
    lp->dxdx = m->dx * m->dx;               // Change in x squared
    lp->dzdz = m->dz * m->dz;               // Change in z squared
    
    lp->coef_dx2 = malloc(lp->coef_len * sizeof(double));
    lp->coef_dz2 = malloc(lp->coef_len * sizeof(double));
    for(size_t i=0; i < lp->coef_len; i++) {
        lp->coef_dx2[i] = lp->coef[i]/lp->dxdx;
        lp->coef_dz2[i] = lp->coef[i]/lp->dzdz;
    }
    return lp;
}

void wavefield__destroy_laplacian_params(laplacian_params *lp)
{
    free(lp->coef_dx2);
    free(lp->coef_dz2);
    free(lp->coef);
    free(lp);
}

void wavefield__laplacian(wavefield *wave, velocity_model *model, laplacian_params *lp)
{

    size_t pos;                                 // Position on flattened grid
    double lapx;                                // Calculated laplacian X axis
    double lapz;                                // Calculated laplacian Z axis
    double vel_dt2;                             // (vel/dt)^2

    for(size_t iz = lp->border_size; iz < (model->nz - lp->border_size); iz++) {
        for(size_t ix = lp->border_size; ix < (model->nx - lp->border_size); ix++) {

            // Position being iterated over in the flatenned array
            pos = wave->nx * iz + ix;

            // Calculate (vel/dt)^2 in given position
            vel_dt2 = powf(model->vel[pos] * lp->dt, 2);

            // Stencil center
            lapx = lp->coef_dx2[0] * wave->grid_o[pos];
            lapz = lp->coef_dz2[0] * wave->grid_o[pos];

            // Stencil borders
            for(size_t ic=1; ic < lp->coef_len; ic++) {
                //x direction
                lapx += lp->coef_dx2[ic] * (
                        wave->grid_o[pos-ic] +
                        wave->grid_o[pos+ic]);

                // Z direction
                lapz += lp->coef_dz2[ic] * (
                        wave->grid_o[pos - (ic * wave->nx)] +
                        wave->grid_o[pos + (ic * wave->nx)]);
            }

            // Second order timestep
            wave->grid[pos] = 2*wave->grid_o[pos] - wave->grid[pos] + vel_dt2 * (lapx + lapz);

            /* **************************************************************
             *
             * DEBUG PURPOSES
             *  It will plot the stencil around the area of signal injection
             *
             ***************************************************************/
            // size_t mid = 125751;
            // if(pos == mid)
            // {
            //     int off = (lp->coef_len-1)/2;
            //     //fprintf(stderr, "coeflen %zu off: %d\n", lp->coef_len, off);
            //     for(int i=-off; i <= off ; i++) {
            //         for(int j=-off; j <= off; j++) 
            //             fprintf(stderr, "(%6zu)%12.8lf ", pos + (i*model->nx) + j, wave->grid[pos+(i*model->nx) +j]);  
            //         fprintf(stderr,"\n");
            //     }
            //     fprintf(stderr, "lap_x %14.10lf lap_z %14.10lf vel_dt2 %14.10lf vel %14.10lf\n\n", lapx, lapz, vel_dt2, model->vel[pos]);
            // }
            // END DEBUG
        }
    }
}

void wavefield__perfect_match_layer(wavefield *wave, velocity_model *model, laplacian_params *lp)
{
    size_t pos;

    // Left
    for(size_t iz = 0; iz < wave->nz; iz++) {
        for(size_t ix = 0; ix < lp->border_size; ix++) {
            pos = wave->nx * iz + ix;
            wave->grid_o[pos] = wave->grid[pos] + 
                model->vel[pos] * lp->dt_dx * (wave->grid[pos+1] - wave->grid[pos]);
        }
    }

    // Right
    for(size_t iz = 0; iz < wave->nz; iz++) {
        for(size_t ix = (wave->nx - lp->border_size); ix < wave->nx; ix++) {
            pos = wave->nx * iz + ix;
            wave->grid_o[pos] = wave->grid[pos] - 
                model->vel[pos] * lp->dt_dx * (wave->grid[pos] - wave->grid[pos-1]);
        }
    }

    // Bottom
    for(size_t iz = (wave->nz - lp->border_size); iz < wave->nz; iz++) {
        for(size_t ix = 0; ix < wave->nx; ix++) {
            pos = wave->nx * iz + ix;
            wave->grid_o[pos] = wave->grid[pos] - 
                model->vel[pos] * lp->dt_dx * (wave->grid[pos] - wave->grid[pos-model->nx]);
        }
    }

    // DEBUG IF NEEDED
    // fprintf(stderr, "o[%zu]: %lf + v[%zu]: %lf * dt/dx %lf * (o[%zu] %lf - o[%zu] %lf)\n",
    //         pos, wave->grid_o[pos], 
    //         pos, model->vel[pos], lp->dt_dx,
    //         pos+1, wave->grid_o[pos+1], 
    //         pos, wave->grid_o[pos]);

}

void wavefield__swap(wavefield *wave)
{
    double *tmp;

    tmp = wave->grid_o;
    wave->grid_o = wave->grid;
    wave->grid = tmp;
}

