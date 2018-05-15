#include "velocity_model.h"

/*
 * Create velocity models and operates on them
 */


velocity_model * velocity_model__create(size_t nx, size_t nz, double dx, double dz)
{
    /* 
     * Creates the velocity_model structure without any data associated
     */
    velocity_model *model = malloc(sizeof *model);

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

    // Allocate the velocity model
    velocity_model *model;
    size_t nelem = bytes / sizeof *model->vel;

    // If dimensions do not match return an ugly stuff.
    if (nx * nz != nelem) {
        return NULL;
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


size_t velocity_model__write_to_file(const char filename[], velocity_model *model)
{
    /*
     * Write the velocity_model 'model' to a file 'filename'. Return the number
     * of elements  of sizeof *model->vel  written
     */

    FILE *fp = fopen(filename, "wb");
    size_t nelem = fwrite(model->vel, sizeof *model->vel, model->nx * model->nz, fp);

    // Flush is needed if you will read RIGHT AFTER write. Boring. 
    fflush(fp);

    return nelem;
}


velocity_model *velocity_model__create_submodel(velocity_model *model, size_t nxa, size_t nza, size_t nxb, size_t nzb)
{
    /*
     * Returns a sub velocity model from a given model based on limits
     * [nxa,nxb] x [nza,nzb]
     *
     * The sub velocity model keeps the same grid distance
     */
    velocity_model *sub = malloc(sizeof *sub);
    sub->nx = nxb - nxa;
    sub->nz = nzb - nza;
    sub->dx = model->dx;
    sub->dz = model->dz;
    sub->vel = malloc(sub->nx * sub->nz * sizeof *sub->vel);

    // Copying the sub cube
    size_t i = 0;
    sub->v_min = model->vel[nxa + model->nx * nza];
    sub->v_max = model->vel[nxa + model->nx * nza];
    for(size_t iz = nza; iz < nzb; iz++) {
        for(size_t ix = nxa; ix < nxb; ix++) {
            sub->v_min=fmin(sub->v_min, sub->vel[i]);
            sub->v_max=fmax(sub->v_max, sub->vel[i]);
            sub->vel[i++] = model->vel[ix + iz * model->nx];
        }
    }
    return sub;
}


velocity_model *velocity_model__create_submodel_from_file(
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
     *
     * Where:
     *  nx:     Number of X cells in file
     *  nz:     Number of Z cells in file
     *  dx:     Size of grid in X direction
     *  dz:     Size of grid in Z direction
     *  nxa:    Lower bound in X axis to create the sub velocity model
     *  nxb:    Upper bound in X axis to create the sub velocity model
     *  nza:    Lower bound in Z axis to create the sub velocity model
     *  nzb:    Upper bound in Z axis to create the sub velocity model
     *
     *  Returns: velocity_model
     *
     * Unless I have some serious optimization (read from file directly to
     * the sub model, this function is pointless. But it was done.
     */

    velocity_model *model = velocity_model__read_from_file(filename, nx, nz, dx, dz);
    velocity_model *sub = velocity_model__create_submodel(model, nxa, nza, nxb, nzb);
    free(model);
    return sub;
}


void velocity_model__constant_velocity(velocity_model *model, double velocity)
{
    /*
     * Fill a velocity model with a grid of constant  velocity
     */
    model->v_min = velocity;
    model->v_max = velocity;
    model->vel = malloc(model->nx * model->nz * sizeof (double));

    for(size_t i = 0; i < model->nx * model->nz; i++)
        model->vel[i] = velocity;
}


void velocity_model__cartesian_velocity(velocity_model *model)
{
    /*
     * Fill a velocity model with a "cartisian" velocity. Each value can be
     * used as a reference to the position on velocity grid. Mostly used for
     * debug.
     *
     * Each velocity value on grid is on double format where
     *
     * z.x
     *
     * z is the position on Z axis and x is the position on X axis (starting 0)
     *
     * Is also a gradient velocity, changing abruptaly in vertical direction
     * and smoothly in horizontal direction.
     */

    // Is the length of decimal places to fit correctly the coordinates
    double ith = pow(10, ceil(log10(model->nx)));

    model->vel = malloc(model->nx * model->nz * sizeof *model->vel);

    for(size_t iz = 0; iz < model->nz; iz++) 
        for(size_t ix = 0; ix < model->nx; ix++) 
            model->vel[ix + iz * model->nx] = iz + ix/ith;

    model->v_min = 0;
    model->v_max = model->vel[model->nx * model->nz - 1];
}

