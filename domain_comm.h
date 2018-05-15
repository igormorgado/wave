#ifndef _DOMAIN_COMM_H_
#define _DOMAIN_COMM_H_

#include <stdio.h>
#include <stdlib.h>

enum RELATIVE_X_POSITION { LEFT, RIGHT, CENTER };
enum RELATIVE_Z_POSITION { TOP, BOTTOM, MIDDLE };

typedef struct domain_info {
    int world_size;
    int world_rank;
    size_t number_slices_x; 
    size_t number_slices_z; 
    size_t stencil_size_x; 
    size_t stencil_size_z;
    size_t nx;
    size_t nz;
    size_t nxa;
    size_t nxb;
    size_t nza;
    size_t nzb;
    size_t relative_pos_x;
    size_t relative_pos_z;
    size_t slice_z0;
    size_t slice_x0;
    size_t slice_nx;
    size_t slice_nz;
    int horizontal_position;
    int vertical_position;
    int upper_rank;
    int lower_rank;
    int left_rank;
    int right_rank;
} domain_info;

void decompose_domain(
        int world_size, int world_rank, 
        size_t number_slices_x, size_t number_slices_z, 
        size_t stencil_size_x, size_t stencil_size_z,
        size_t nx, size_t nz,
        domain_info *domain);
#endif
