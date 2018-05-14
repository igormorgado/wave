#include "globals.h"
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

// Create velocity that values can be used as coordnates (For debug purposes)
void velocity_model__cartesian_velocity(velocity_model *model)
{

    double ith = pow(10, ceil(log10(model->nx)));

    model->vel = malloc(model->nx * model->nz * sizeof *model->vel);

    for(size_t iz = 0; iz < model->nz; iz++) 
        for(size_t ix = 0; ix < model->nx; ix++) 
            model->vel[ix + iz * model->nx] = iz + ix/ith;

    model->v_min = 0;
    model->v_max = model->vel[model->nx * model->nz - 1];
}


size_t velocity_model__write_to_file(const char filename[], velocity_model *model)
{
    /* Write a velocity model to file */
    FILE *fp = fopen(filename, "wb");

    size_t nelem = fwrite(model->vel, sizeof *model->vel, model->nx * model->nz, fp);

    fflush(fp);
    return nelem;
}

velocity_model *velocity_model__read_sub_from_file(
                                                   const char filename[],
                                                   size_t nx,
                                                   size_t nz,
                                                   double dx,
                                                   double dz,
                                                   size_t nxa,
                                                   size_t nza,
                                                   size_t nxb,
                                                   size_t nzb)
{
    /* This functions read a subset of a velocity model, for all purposes the
     * it returns a regular velocity model.
     * If nxa = 0, nza = 0, nxb = nx and nzb = nz is used the returned model
     * is equivalent to the full one.
     */

    /* Read the size of source cube */
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 0L, SEEK_END);
    long bytes = ftell(fp);
    rewind(fp);

    fprintf(stderr, "Number of bytes read: %ld\n", bytes);

    // Allocate the velocity model
    velocity_model *model;

    size_t nelem = bytes / sizeof *model->vel;

    if (nx * nz != nelem) {
        fprintf(stderr, "Number of elements %zu does not match source model dimensions %zu x %zu.\n", nelem, nx, nz);
        exit(EXIT_FAILURE);
    }

    model = malloc(sizeof *model);
    model->nx = nx;
    model->nz = nz;

    model->vel = malloc(nelem * sizeof *model->vel);
    fread(model->vel, sizeof *model->vel, nelem, fp);

    fclose(fp);

    // Allocates the memory to sub velocity model
    velocity_model *sub = malloc(sizeof *sub);
    sub->nx = nxb - nxa;
    sub->nz = nzb - nza;
    sub->dx = dx;
    sub->dz = dz;
    sub->vel = malloc(sub->nx * sub->nz * sizeof *sub->vel);

    // Copying the sub cube
    size_t c = 0;
    sub->v_min = model->vel[nxa + nx * nza];
    sub->v_max = model->vel[nxa + nx * nza];
    for(size_t iz = 0; iz < model->nz; iz++) {
        for(size_t ix = 0; ix < model->nx; ix++) {
            if(ix >= nxa && ix < nxb && iz >= nza && iz < nzb) {
                sub->vel[c] = model->vel[ix + iz * model->nx];
                sub->v_min=fmin(sub->v_min, sub->vel[c]);
                sub->v_max=fmax(sub->v_max, sub->vel[c]);
                c++;
            }
        }
    }

    free(model);
    return sub;
}
    

velocity_model * velocity_model__read_from_file(
        const char filename[],
        size_t nx,
        size_t nz,
        double dx,
        double dz)
{
    
    /* Read a velocity model from a filename and check its size*/
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 0L, SEEK_END);
    long bytes = ftell(fp);
    rewind(fp);

    fprintf(stderr, "Number of bytes read: %ld\n", bytes);

    // Allocate the velocity model
    velocity_model *model;
    size_t nelem = bytes / sizeof *model->vel;

    if (nx * nz != nelem) {
        fprintf(stderr, "Number of elements %zu does not match model dimensions %zu x %zu.\n", nelem, nx, nz);
        exit(EXIT_FAILURE);
    }

    // Allocates the memory to velocity model
    model = malloc(sizeof *model);
    model->vel = malloc(nelem * sizeof *model->vel);

    fread(model->vel, sizeof *model->vel, nelem, fp);
    fclose(fp);

    // Finding minimum and maximul velocity
    model->v_min = model->vel[0];
    model->v_max = model->vel[0];
    for(size_t i=1; i< nelem; i++) {
        model->v_min=fmin(model->v_min, model->vel[i]);
        model->v_max=fmax(model->v_max, model->vel[i]);
    }

    model->nx = nx;
    model->nz = nz;
    model->dx = dx;
    model->dz = dz;

    return model;
}

void velocity_model__destroy(velocity_model *model)
{
    free(model->vel);
    free(model);
}
