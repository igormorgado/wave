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

void wavefield__laplacian(wavefield *wave, velocity_model *model, double dt)
{
    // Finite Difference Coefficients - Second Derivative - nth order
    // const double cfd2nd_2th_order[2] = { -2., 1.};
    // const double cfd2nd_4th_order[3] = { -5./2, 4./3, -1./12};
    // const double cfd2nd_6th_order[4] = { -49./18, 3./2, -3./20, 1./90 };
    // const double cfd2nd_8th_order[5] = { -205./72, 8./5, -1./5, 8./315, -1./560 };

    //const double coef[] = { -205./72, 8./5, -1./5, 8./315, -1./560 };
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
        coef_dx2[i] = coef[i]/dxdx;
        coef_dz2[i] = coef[i]/dzdz;
    }


    for(size_t iz = border_size; iz < (model->nz - border_size); iz++) {
        for(size_t ix = border_size; ix < (model->nx - border_size); ix++) {

            // Position being calculated
            pos = wave->nx * iz + ix;

            // Stencil center
            lapx = coef_dx2[0] * wave->grid_o[pos];
            lapz = coef_dz2[0] * wave->grid_o[pos];

            // Stencil borders
            for(size_t ic=1; ic < coef_len ; ic++) {
                //x direction
                lapx += wave->grid_o[pos-ic];
                lapx += wave->grid_o[pos+ic];
                lapx *= coef_dx2[ic];

                // Z direction
                lapz += wave->grid_o[pos - (ic * wave->nx)];
                lapz += wave->grid_o[pos + (ic * wave->nx)];
                lapz *= coef_dz2[ic];
            }

            // Velocity in point being calculated
            vel = powf(model->cube[pos] * dt , 2);
            wave->grid[pos] = 2*wave->grid_o[pos] - wave->grid[pos] + vel * (lapx + lapz);

            // debug only. Remove later
            // if(verbose && pos == 125751)
            // {
            //     int off = (coef_len-1)/2;
            //     for(int i=-off; i <= off ; i++) {
            //         for(int j=-off; j <= off; j++) 
            //             fprintf(stderr, "(%6zu)%12.8lf ", pos + (i*model->nx) + j, wave->grid[pos+(i*model->nx) +j]);  
            //         fprintf(stderr,"\n");
            //     }
            //     fprintf(stderr, "lap_x %.10lf lap_z %.10lf vel_d2 %.10lf vel %.10lf\n\n", lapx, lapz, vel, model->cube[pos]);
            // }
            // END DEBUG
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

