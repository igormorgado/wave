#include "velocity_model.h"

// Create a empty velocitu model specs
velocity_model * velocity_model__create(size_t nx, size_t nz, double dx, double dz)
{
    velocity_model *model = malloc(sizeof *model);

    model->nx = nx;
    model->nz = nz;
    model->dx = dx;
    model->dz = dz;

    return model;
}

//  Create and reserve memory to VM velocity
void velocity_model__constant_velocity(velocity_model *model, double velocity)
{
    model->v_min = velocity;
    model->v_max = velocity;
    model->vel = malloc(model->nx * model->nz * sizeof (double));

    if(verbose)
    {
        fprintf(stderr, "Creating constant velocity model. Params:\n");
        fprintf(stderr,"%9s: %lf\n", "Velocity", velocity);
        fprintf(stderr,"%9s: %lf\n", "V_min", model->v_min);
        fprintf(stderr,"%9s: %lf\n", "V_max", model->v_max);
    }

    for(size_t i = 0; i < model->nx * model->nz; i++)
        model->vel[i] = velocity;
}

void velocity_model__destroy(velocity_model *model)
{
    free(model->vel);
    free(model);
}
