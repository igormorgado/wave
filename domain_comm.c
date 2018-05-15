#include "domain_comm.h"


void decompose_domain(
        int world_size, 
        int world_rank, 
        size_t number_slices_x, 
        size_t number_slices_z, 
        size_t stencil_size_x, 
        size_t stencil_size_z,
        size_t nx,
        size_t nz,
        domain_info *domain)
{ 
    // These are relative position of subgrid in the whole grid
    // Are coordinates starting at 0,0 and going to 
    // [num_slices_x -1 , num_slices_z - 1]
    size_t relative_pos_x;
    size_t relative_pos_z;

    // ENUM for the position can be: (maybe not necessary anymore)
    // horizontal:  LEFT, RIGHT, MIDDLE      
    // vertical:    TOP, BOTTOM, MIDDLE
    int horizontal_position;
    int vertical_position;

    /*
    // These are the final coordinates of subgrid already with the ghost areas
    // inside the main grid
    
        0 |             nxa                                nxb     nx
          |                \    slice_x0                  /        \    X
        -------------------------------------------------------------------->
          |                 {   .                    .   }          |
          |     .           {   |---- slice_nx ----  |   }     .    |
          |      .          {   .                    .   }    .     |
          |       .         {   .                    .   }   .      |
    nza   |                 {   .                    .   }          |
       \  |                 {   .                    .   }          |
          |- - - - - - - -  X ~  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ X          |
 slicez0  |                 {   .                    .   }          |
        ` |........-........{...o_ _ _ _ _ _ _ _ _ _ o...}..........|
          |        |        {   :                    :   }          |
          |        |        {   :                    :   }          |
          |    slice_nz     {   :                    :   }  ...     |
          |        |        {   :                    :   }          |
          |        |        {   :                    :   }          |
          |........-........{...o_ _ _ _ _ _ _ _ _ _ o...}..........|
          |                 {   .                    .   }          |
          |                 {   .                    .   }  .       |
          |- - - - - - - - -X ~  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ X   .      |
        / |                 [   ]                    [   ]    .     |
     nzb  |      .              \                     /             |
          |     .                 stencil absorption                |
          |    .                                                    |
      nz  |                                                         |
        ` -----------------------------------------------------------
          |
          |
        Z |
          v

       X == ghost border boundaries
       o == regular slice boundaries
    */
    // Where the decomposed domain starts (without ghost borders)
    size_t slice_z0;
    size_t slice_x0;

    // Size of decomposed domain (withouth ghost borders)
    size_t slice_nx;
    size_t slice_nz;
    
    // Final positions with ghost borders
    size_t nxa, nxb, nza, nzb;

    // World_Rank  of neighboor tasks
    int upper_rank = -1;
    int lower_rank = -1;
    int left_rank = -1;
    int right_rank = -1;


    relative_pos_x = world_rank % number_slices_x;
    relative_pos_z = world_rank / number_slices_x;

    // Calculates the size of this sub domain
    slice_nx = nx/number_slices_x + ( relative_pos_x < (nx % number_slices_x));
    slice_nz = nz/number_slices_z + ( relative_pos_z < (nz % number_slices_z));
    
    // Calculates where this subdomain starts based on relative pos
    slice_x0 = relative_pos_x * (nx/number_slices_x);
    if (nx % number_slices_x) 
        if(relative_pos_x > (nx % number_slices_x))
            slice_x0 += nx%number_slices_x;
        else
            slice_x0 += relative_pos_x;

    slice_z0 = relative_pos_z * (nz/number_slices_z);
    if (nz % number_slices_z) 
        if(relative_pos_z > (nz % number_slices_z))
            slice_z0 += nz%number_slices_z;
        else
            slice_z0 += relative_pos_z;

    
    // Now based on stencil size and location find the exact coordinates
    // of polygon to start and stop the creation of the subdomain
    nxa = slice_x0;
    nxb = slice_x0 + slice_nx;

    nza = slice_z0;
    nzb = slice_z0 + slice_nz;


    // Find where on grid this domain is positioned 
    // Add the neighboors ranks
    // Apply offsets based on stencil
    if(relative_pos_x == 0) {
        horizontal_position = LEFT;
        right_rank = world_rank + 1;
        nxb += stencil_size_x;
    } else if (relative_pos_x == number_slices_x - 1 ) {
        horizontal_position = RIGHT;
        left_rank = world_rank - 1;
        nxa -= stencil_size_x;
    } else {
        horizontal_position = MIDDLE;
        right_rank = world_rank + 1;
        left_rank = world_rank - 1;
        nxa -= stencil_size_x;
        nxb += stencil_size_x;
    }

    if(relative_pos_z == 0) {
        vertical_position = TOP;
        lower_rank = world_rank + number_slices_x;
        nzb += stencil_size_z;
    } else if (relative_pos_z == number_slices_z - 1 ) {
        upper_rank = world_rank - number_slices_x;
        vertical_position = BOTTOM;
        nza -= stencil_size_z;
    } else {
        vertical_position = MIDDLE;
        upper_rank = world_rank - number_slices_x;
        lower_rank = world_rank + number_slices_x;
        nza -= stencil_size_z;
        nzb += stencil_size_z;
    }

    // Fill the datastructure
    domain->world_size = world_size;
    domain->world_rank = world_rank;
    domain->number_slices_x = number_slices_x;
    domain->number_slices_z = number_slices_z;
    domain->stencil_size_x = stencil_size_x;
    domain->stencil_size_z = stencil_size_z;
    domain->nx = nx;
    domain->nz = nz;
    domain->nxa = nxa;
    domain->nxb = nxb;
    domain->nza = nza;
    domain->nzb = nzb,
    domain->relative_pos_x = relative_pos_x;
    domain->relative_pos_z = relative_pos_z;
    domain->slice_x0 = slice_x0;
    domain->slice_z0 = slice_z0;
    domain->slice_nx = slice_nx;
    domain->slice_nz = slice_nz;
    domain->horizontal_position = horizontal_position;
    domain->vertical_position = vertical_position;
    domain->upper_rank = upper_rank;
    domain->lower_rank = lower_rank;
    domain->left_rank = left_rank;
    domain->right_rank = right_rank;
    // THIS IS DAMN UGLY BUT WILL WORK FOR NOW!!!!
}




