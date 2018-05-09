#include <stdlib.h>

#include "wavefield.h"

wavefield * wavefield__create(size_t nx, size_t nz) {
    wavefield *w = malloc(sizeof (wavefield));
    w->nx = nx;
    w->nz = nz;
    double w->grid   = calloc(nx * nz, sizeof (double));
    double w->grid_o = calloc(nx * nz, sizeof (double));

    return w;
}

void wavefield__destroy(wavefield *w) 
{
    free(w->grid);
    free(w->grid_o);
    free(w);
}


// functions

// 0. Iterate fourth order

// 1. Swap wave field

// 2. apply pml

//
    // // Iterate over timesteps THIS SHOULD GO SOMEWHERE ELSE
    // vel = powf(vel * dt, 2);

    // int ntrec = dtrec/dt;

    // // Allocate wavefield: MAYBE THIS GO TO SIMULATION LATER
    // double *P1[nz], *P2[nz], *Pt[nz];
    // for(int i = 0; i < nz; i++) {
    //     P1[i] = calloc( nx, sizeof **P1);
    //     P2[i] = calloc( nx, sizeof **P2);
    //     Pt[i] = calloc( nx, sizeof **Pt);
    // }

    // // Wave field debug
    // // for(int i = 0; i < nz; i++) {
    // //     for(int j = 0; j < nx; j++)
    // //         fprintf(stderr, "%2.1f ", P1[i][j]);
    // //     fprintf(stderr, "\n");
    // // }

    // // Mathematical parameters;
    // double lapx;
    // double lapz;

    // /* 2nd order derivative in space (already applied spatial squares)
    // double dxcoef2[2];
    // double dzcoef2[2];
    // for( int i=0; i< 2; i++)  {
    //     dxcoef2[i] = coef2[i]/(dx*dx);
    //     dzcoef2[i] = coef2[i]/(dz*dz);
    // }
    // */

    // /* 4th order derivative  in space  (already applied spatial squares)
    //  * should not be 4ths instead squares?
    // double dxcoef4[3];
    // double dzcoef4[3];
    // for( int i=0; i< 3; i++)  {
    //     dxcoef4[i] = coef4[i]/(dx*dx);
    //     dzcoef4[i] = coef4[i]/(dz*dz);
    // }
    // */

    // /* 8th  order derivative in space (already applied spatial squares)
    //  * should not be 8ths instead squares?
    // double coef8[] = { -49/18, 3/2, -3/20, 1/90 };
    // double dxcoef8[4];
    // double dzcoef8[4];
    // for( int i=0; i< 4; i++)  {
    //     dxcoef8[i] = coef8[i]/(dx*dx);
    //     dzcoef8[i] = coef8[i]/(dz*dz);
    // }
    // */
