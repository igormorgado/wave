#include "velocity_model.h"

// Create a empty velocitu model specs
velocity_model * velocity_model__create(size_t nx, size_t nz, double dx, double dz) {
    velocity_model *model = malloc(sizeof *model);
    model->nx = nx;
    model->nz = nz;
    model->dx = dx;
    model->dz = dz;

    return model;
}

//  Create and reserve memory to VM cube
void  velocity_model__constant_cube(velocity_model *model, double velocity) {
    model->v_min = velocity;
    model->v_max = velocity;
    model->cube = malloc(model->nx * model->nz * sizeof (double));
    for(int i = 0; i < model->nx * model->nz; i++)
        model->cube[i] = velocity;
}

void velocity_model__destroy(velocity_model *model)
{
    free(model->cube);
    free(model);
}
