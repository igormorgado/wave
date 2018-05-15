#include "wavefieldmpi.h"


void wavefieldmpi__perfect_match_layer_by_domain(
        wavefield *wave, 
        velocity_model *model, 
        laplacian_params *lp, 
        domain_info *domain)
{
    size_t pos;

    // Left
    if(domain->horizontal_position == LEFT) {
        for(size_t iz = 0; iz < wave->nz; iz++) {
            for(size_t ix = 0; ix < lp->border_size; ix++) {
                pos = wave->nx * iz + ix;
                wave->grid_o[pos] = wave->grid[pos] + 
                    model->vel[pos] * lp->dt_dx * (wave->grid[pos+1] - wave->grid[pos]);
            }
        }
    }

    // Right
    if(domain->horizontal_position == RIGHT) {
        for(size_t iz = 0; iz < wave->nz; iz++) {
            for(size_t ix = (wave->nx - lp->border_size); ix < wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                wave->grid_o[pos] = wave->grid[pos] - 
                    model->vel[pos] * lp->dt_dx * (wave->grid[pos] - wave->grid[pos-1]);
            }
        }
    }

    // Bottom
    if(domain->vertical_position == BOTTOM) {
        for(size_t iz = (wave->nz - lp->border_size); iz < wave->nz; iz++) {
            for(size_t ix = 0; ix < wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                wave->grid_o[pos] = wave->grid[pos] - 
                    model->vel[pos] * lp->dt_dx * (wave->grid[pos] - wave->grid[pos-model->nx]);
            }
        }
    }
}

void wavefieldmpi__share_border_by_domain(wavefield *wave, laplacian_params *lp, domain_info *d)
{
    MPI_Status status;
    size_t border_size;
    double *buf;
    size_t nelem;
    size_t i;
    size_t pos;

    if(d->left_rank != -1) {
        nelem = d->stencil_size_x * wave->nz;
        buf = malloc(nelem * sizeof(double));

        //send border to left
        i = 0;
        for(size_t iz = 0; iz < wave->nz; iz++) {
            for(size_t ix=0; ix< d->stencil_size_x; ix++) {
                pos = wave->nx * iz + ix;
                buf[i++] = wave->grid[pos];
            }
        }
        MPI_Send(buf, nelem, MPI_DOUBLE, d->left_rank, 0, MPI_COMM_WORLD);

        //expect border from left
        i = 0;
        MPI_Recv(buf, nelem, MPI_DOUBLE, d->left_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(size_t iz = 0; iz < wave->nz; iz++) {
            for(size_t ix=0; ix< d->stencil_size_x; ix++) {
                pos = wave->nx * iz + ix;
                wave->grid_o[pos] = buf[i++];
            }
        }
    }

    if(d->right_rank != -1) {
        nelem = d->stencil_size_x * wave->nz; 
        buf = malloc(nelem * sizeof(double));

        //expect border from right
        i = 0;
        MPI_Recv(buf, nelem, MPI_DOUBLE, d->right_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(size_t iz = 0 ; iz < wave->nz; iz++) {
            for(size_t ix = wave->nx - d->stencil_size_x ; ix < wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                wave->grid_o[pos] = buf[i++];
            }
        }
        
        //send border to right
        i = 0;
        for(size_t iz = 0 ; iz < wave->nz; iz++) {
            for(size_t ix = wave->nx - d->stencil_size_x ; ix< wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                buf[i++] = wave->grid[pos];
            }
        }
        MPI_Send(buf, nelem, MPI_DOUBLE, d->right_rank, 0, MPI_COMM_WORLD);
    }

    if(d->upper_rank != -1) {
        nelem = d->stencil_size_z * wave->nx;
        buf = malloc(nelem * sizeof(double));

        //send border to up
        i = 0;
        for(size_t iz=0 ; iz < d->stencil_size_z; iz++) {
            for(size_t ix = 0; ix < wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                buf[i++] = wave->grid[pos];
            }
        }
        MPI_Send(buf, nelem, MPI_DOUBLE, d->upper_rank, 0, MPI_COMM_WORLD);

        //expect border from up
        i = 0;
        MPI_Recv(buf, nelem, MPI_DOUBLE, d->upper_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(size_t iz=0 ; iz < d->stencil_size_z; iz++) {
            for(size_t ix = 0; ix < wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                wave->grid_o[pos] = buf[i++];
            }
        }
    }

    if(d->lower_rank != -1) {
        nelem = d->stencil_size_z * wave->nx;
        buf = malloc(nelem * sizeof(double));

        //send boerder to down
        i = 0;
        for(size_t iz= wave->nz - d->stencil_size_z; iz < wave->nz; iz++) {
            for(size_t ix = 0; ix < wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                buf[i++] = wave->grid[pos];
            }
        }
        MPI_Send(buf, nelem, MPI_DOUBLE, d->lower_rank, 0, MPI_COMM_WORLD);

        //expect border from down
        i = 0;
        MPI_Recv(buf, nelem, MPI_DOUBLE, d->lower_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(size_t iz= wave->nz - d->stencil_size_z; iz < wave->nz; iz++) {
            for(size_t ix = 0; ix < wave->nx; ix++) {
                pos = wave->nx * iz + ix;
                wave->grid_o[pos] = buf[i++];
            }
        }
    }

    free(buf);
}
