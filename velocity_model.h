#ifndef _VELOCITY_MODEL_H_
#define _VELOCITY_MODEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct velocity_model 
{
    size_t  nx;                 /* Number of X cells / columns */
    size_t  nz;                 /* Number of Z cells / lines */
    double  dx;                 /* Size of X grid cell */  
    double  dz;                 /* Size of Z grid cell */  
    double  v_min;              /* minimum velocity */
    double  v_max;              /* maximum velocity */
    double  *vel;              /* Velocity model flatenned array */
} velocity_model;


velocity_model * velocity_model__create(size_t nx, size_t nz, double dx, double dz);
void             velocity_model__destroy(velocity_model *model);


velocity_model * velocity_model__read_from_file( const char filename[], size_t nx, size_t nz, double dx, double dz);
size_t           velocity_model__write_to_file(const char filename[], velocity_model *model);

velocity_model *velocity_model__create_submodel(velocity_model *model, size_t nxa, size_t nza, size_t nxb, size_t nzb);
velocity_model *velocity_model__create_submodel_from_file(
                                        const char filename[],
                                        size_t nx,
                                        size_t nz,
                                        double dx,
                                        double dz,
                                        size_t nxa,
                                        size_t nza,
                                        size_t nxb,
                                        size_t nzb);

void             velocity_model__constant_velocity(velocity_model *model, double velocity);
void             velocity_model__cartesian_velocity(velocity_model *model);


#endif
