#include "domain_comm.h"

int main(void) {

    domain_info *d = malloc(sizeof *d);
    
    int world_size = 9;
    size_t number_slices_x = 4;
    size_t number_slices_z = 3;
    size_t stencil_size_x = 4;
    size_t stencil_size_z = 4;
    size_t nx = 1200;
    size_t nz = 900;

    // fprintf(stderr,"#Slices      X: %5zu         Z: %5zu\n", d->number_slices_x, d->number_slices_z);
    // fprintf(stderr,"Stencil      X: %5zu         Z: %5zu\n", d->stencil_size_x, d->stencil_size_z);
    // fprintf(stderr,"# grid       X: %5zu         Z: %5zu\n", d->nx, d->nz);
    for(int world_rank=0; world_rank< world_size; world_rank++) {
        decompose_domain(world_size, world_rank, 
                         number_slices_x, number_slices_z, 
                         stencil_size_x, stencil_size_z, 
                         nx, nz, 
                         d);

        fprintf(stderr,"Environment info\n");
        fprintf(stderr,"=============================================\n");
        fprintf(stderr,"MPI       size: %5d      rank: %5d\n", d->world_size, d->world_rank);

        fprintf(stderr,"\nSubdomain info\n");
        fprintf(stderr,"=============================================\n");
        fprintf(stderr,"RelPos      X: %5d         Z: %5d\n", d->relative_pos_x, d->relative_pos_z);
        fprintf(stderr,"Slice_0     X: %5d         Z: %5d\n", d->slice_x0, d->slice_z0);
        fprintf(stderr,"Slice_N     X: %5d         Z: %5d\n", d->slice_nx, d->slice_nz);
        fprintf(stderr,"Positin     X: %5d         Z: %5d\n", d->horizontal_position, d->vertical_position);
        fprintf(stderr,"nx          a: %5d         b: %5d\n", d->nxa, d->nxb);
        fprintf(stderr,"nz          a: %5d         b: %5d\n", d->nza, d->nzb);
        fprintf(stderr,"Ranks       U: %5d         B: %5d\n", d->upper_rank, d->lower_rank);
        fprintf(stderr,"Ranks       L: %5d         R: %5d\n", d->left_rank, d->right_rank);
        fprintf(stderr,"\n");
    }

}




