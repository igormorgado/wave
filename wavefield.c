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

void wavefield__laplacian4(wavefield *wave, velocity_model *model, double dt)
{

    // Finite Difference Coefficients - Second Derivative - nth order
    // const double cfd2nd_2th_order[2] = { -2., 1.};
    // const double cfd2nd_4th_order[3] = { -5./2, 4./3, -1./12};
    // const double cfd2nd_6th_order[4] = { -49./18, 3./2, -3./20, 1./90 };
    // const double cfd2nd_8th_order[5] = { -205./72, 8./5, -1./5, 8./315, -1./560 };

    // 2nd Derivative - 4th order coefficients
    const double coef[] = { -5./2, 4./3, -1./12 };
    size_t coef_len = len(coef);
    size_t border_size = coef_len - 1;

    size_t pos;                                 // Position on flattened grid
    double lapx;                                // Calculated laplacian X axis
    double lapz;                                // Calculated laplacian Z axis
    double vel;                                 // Store temp. velocity for position with dt applied

    double dxdx = model->dx * model->dx;        // Change in x squared
    double dzdz = model->dz * model->dz;        // Change in z squared

    // Pre-calculate coefficients with the gradient
    // That should be created and calculated just once
    // How to achieve that?
    double coef_dx2[coef_len];
    double coef_dz2[coef_len];
    for(size_t i=0; i < coef_len; i++) {
         coef_dx2[i] = coef[i]/(dxdx);
         coef_dz2[i] = coef[i]/(dzdz);
    }

    // size_t check = 125751;

    for(size_t iz = border_size; iz < (model->nz - border_size); iz++) {
        for(size_t ix = border_size; ix < (model->nx - border_size); ix++) {
            pos = wave->nx * iz + ix;
            vel = powf(model->cube[pos] * dt , 2);

            // Stencil center
            lapx = coef_dx2[0] * wave->grid[pos];
            lapz = coef_dz2[0] * wave->grid[pos];
            
            // Stencil borders
            for(size_t ic=1; ic < coef_len ; ic++) {
                //x direction
                lapx += wave->grid[pos-ic];
                lapx += wave->grid[pos+ic];
                lapx *= coef_dx2[ic];

                // Z direction
                lapz += wave->grid[pos - (ic * wave->nx)];
                lapz += wave->grid[pos + (ic * wave->nx)];
                lapz *= coef_dz2[ic];
            }

            wave->grid[pos] = 2*wave->grid_o[pos] - wave->grid[pos] + vel * (lapx + lapz);

            // if(pos == check) {
            //     fprintf(stderr, "%32zu\n", pos + (-2 * wave->nx));
            //     fprintf(stderr, "%32zu\n", pos + (-1 * wave->nx));
            //     fprintf(stderr, "%10zu %10zu %10zu %10zu %10zu\n", pos-2, pos-1, pos, pos+1, pos+2); 
            //     fprintf(stderr, "%32zu\n", pos + (1 * wave->nx));
            //     fprintf(stderr, "%32zu\n\n", pos + (2 * wave->nx));

            //     fprintf(stderr, "%32lf\n", wave->grid[pos + (-2 * wave->nx)]);
            //     fprintf(stderr, "%32lf\n", wave->grid[pos + (-1 * wave->nx)]);
            //     fprintf(stderr, "%10lf %10lf %10lf %10lf %10lf\n", wave->grid[pos-2], wave->grid[pos-1], wave->grid[pos], wave->grid[pos+1], wave->grid[pos+2]); 
            //     fprintf(stderr, "%32lf\n", wave->grid[pos + ( 1 * wave->nx)]);
            //     fprintf(stderr, "%32lf\n\n", wave->grid[pos + ( 2 * wave->nx)]);
            //     fprintf(stderr, "lap x,z vel: %10lf %10lf %10lf\n\n", lapx, lapz, vel);
            // }
            
        }
    }
}

void wavefield__swap(wavefield *wave)
{
    double *tmp;

    tmp = wave->grid_o;
    wave->grid_o = wave->grid;
    wave->grid = tmp;
}

// 2. apply pml

