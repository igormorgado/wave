#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"
#include "velocity_model.h"

int main(int argc, char *argv[]) {

    const char filename[] = "testmodel.bin";
    size_t nx = 30;
    size_t nz = 20;
    double dx = 1.;
    double dz = 1.;

    int xlen = ceil(log10(nx));
    int zlen = ceil(log10(nz));
    size_t nxa = 10;
    size_t nza = 7;
    size_t nxb = 20;
    size_t nzb = 14;

    verbose = 1;

    fprintf(stderr, "\nEntered data\n");
    fprintf(stderr,   "====================================\n");
    fprintf(stderr, "%-20s: %s\n", "filename", filename);
    fprintf(stderr, "%-20s: %zu\n", "nx", nx);
    fprintf(stderr, "%-20s: %zu\n", "nz", nz);
    fprintf(stderr, "%-20s: %lf\n", "dx", dx);
    fprintf(stderr, "%-20s: %lf\n", "dz", dz);

    // Create a  new wavelet
    velocity_model *model = velocity_model__create(nx, nz, dx, dz);

    fprintf(stderr, "\nGenerated Velocity Model information\n");
    fprintf(stderr,   "====================================\n");
    fprintf(stderr, "%-20s: %zu\n", "model->nx", model->nx);
    fprintf(stderr, "%-20s: %zu\n", "model->nz", model->nz);
    fprintf(stderr, "%-20s: %lf\n", "model->dx", model->dx);
    fprintf(stderr, "%-20s: %lf\n", "model->dz", model->dz);

    velocity_model__cartesian_velocity(model);

    fprintf(stderr, "\nGenerated Model data\n");
    fprintf(stderr,   "============================\n");
    fprintf(stderr, "%-20s: %lf\n", "model->v_min", model->v_min);
    fprintf(stderr, "%-20s: %lf\n", "model->v_max", model->v_max);


    fprintf(stderr, "\nSource model\n");
    fprintf(stderr,   "============================\n");
    for(size_t iz = 0; iz < model->nz; iz++)  {
        for(size_t ix = 0; ix < model->nx; ix++) 
            fprintf(stderr, "%0*.*lf ", xlen+zlen+1, xlen, model->vel[ix + iz * model->nx]);

        fprintf(stderr, "\n");
    }

    // Save model to disk
    fprintf(stderr, "\nWriting velocity model to file %s.\n", filename);
    size_t nelem = velocity_model__write_to_file(filename, model);
    fprintf(stderr, "It was written %zu velocity elements of total %zu velocity model size\n",
            nelem, model->nx * model->nz);


    fprintf(stderr, "\nReading sub velocity model from file %s.\n", filename);
    velocity_model *sub = velocity_model__read_sub_from_file(filename, nx, nz, dx, dz, nxa, nza, nxb, nzb);

    fprintf(stderr, "\nRead Velocity wavelet information and data\n");
    fprintf(stderr,   "========================================\n");
    fprintf(stderr, "%-20s: %zu\n", "sub->nx",    sub->nx);
    fprintf(stderr, "%-20s: %zu\n", "sub->nz",    sub->nz);
    fprintf(stderr, "%-20s: %lf\n", "sub->dx",    sub->dx);
    fprintf(stderr, "%-20s: %lf\n", "sub->dz",    sub->dz);
    fprintf(stderr, "%-20s: %lf\n", "sub->v_min", sub->v_min);
    fprintf(stderr, "%-20s: %lf\n", "sub->v_max", sub->v_max);

    fprintf(stderr, "\nSub model x:[%zu,%zu)  z:[%zu,%zu)\n", nxa, nxb, nza, nzb);
    fprintf(stderr,   "=========================================================\n");
    for(size_t iz = 0; iz < sub->nz; iz++)  {
        for(size_t ix = 0; ix < sub->nx; ix++) 
            fprintf(stderr, "%0*.*lf ", xlen+zlen+1, xlen, sub->vel[ix + iz * sub->nx]);

        fprintf(stderr, "\n");
    }

    unlink(filename);

    if(!isatty(fileno(stdout)))
        fwrite(sub->vel, sizeof *sub->vel, sub->nx * sub->nz, stdout);
    else 
        fprintf(stderr, "\nIf stdout is a file/pipe submodel will be written to it\n");

    velocity_model__destroy(model);
    velocity_model__destroy(sub);

    return 0;
}
