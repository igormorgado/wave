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

}
