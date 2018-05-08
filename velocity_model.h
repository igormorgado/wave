#ifndef _VELOCITY_MODEL_H_
#define _VELOCITY_MODEL_H_

#include <stdlib.h>

// #include "simulation.h"
// #include "ricker.h"

typedef struct velocity_model 
{
    size_t  nx;                 /* Number of X cells / columns */
    size_t  nz;                 /* Number of Z cells / lines */
    double  dx;                 /* Size of X grid cell */  
    double  dz;                 /* Size of Z grid cell */  
    double  v_min;              /* minimum velocity */
    double  v_max;              /* maximum velocity */
    double  *cube;              /* Velocity model flatenned array */
} velocity_model;

velocity_model * velocity_model__create(size_t nx, size_t nz, double dx, double dz);

void  velocity_model__constant_cube(velocity_model *vm, double velocity);

void velocity_model__destroy(velocity_model *model);

#endif
