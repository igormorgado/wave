#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"
#include "velocity_model.h"

int main(void) {

    const char filename[] = "testmodel.bin";
    size_t nx = 600;
    size_t nz = 600;
    double dx = 1.;
    double dz = 1.;
    double cvel = 1500;

    double eps = 1e-32;

    verbose = 1;

    fprintf(stderr, "\nEntered data\n");
    fprintf(stderr,   "====================================\n");
    fprintf(stderr, "%-20s: %s\n", "filename", filename);
    fprintf(stderr, "%-20s: %zu\n", "nx", nx);
    fprintf(stderr, "%-20s: %zu\n", "nz", nz);
    fprintf(stderr, "%-20s: %lf\n", "dx", dx);
    fprintf(stderr, "%-20s: %lf\n", "dz", dz);
    fprintf(stderr, "%-20s: %lf\n", "cvel", cvel);

    // Create a  new wavelet
    velocity_model *model = velocity_model__create(nx, nz, dx, dz);

    fprintf(stderr, "\nGenerated Velocity Model information\n");
    fprintf(stderr,   "====================================\n");
    fprintf(stderr, "%-20s: %zu\n", "model->nx", model->nx);
    fprintf(stderr, "%-20s: %zu\n", "model->nz", model->nz);
    fprintf(stderr, "%-20s: %lf\n", "model->dx", model->dx);
    fprintf(stderr, "%-20s: %lf\n", "model->dz", model->dz);

    velocity_model__constant_velocity(model, cvel);

    fprintf(stderr, "\nGenerated Model data\n");
    fprintf(stderr,   "============================\n");
    fprintf(stderr, "%-20s: %lf\n", "model->v_min", model->v_min);
    fprintf(stderr, "%-20s: %lf\n", "model->v_max", model->v_max);

    for(size_t i = 0; i < 10; i++)
        fprintf(stderr, "%-16s[%02zu]: %lf\n", "model->vel", i, model->vel[i]);

    // Save model to disk
    fprintf(stderr, "\nWriting velocity model to file %s.\n", filename);
    size_t nelem = velocity_model__write_to_file(filename, model);
    fprintf(stderr, "It was written %zu velocity elements of total %zu velocity model size\n",
            nelem, model->nx * model->nz);

    sync();
    fprintf(stderr, "\nReading velocity model from file %s.\n", filename);
    velocity_model *model2 = velocity_model__read_from_file(filename, nx, nz, dx, dz);

    fprintf(stderr, "\nRead Velocity wavelet information and data\n");
    fprintf(stderr,   "========================================\n");
    fprintf(stderr, "%-20s: %zu\n", "model2->nx",    model2->nx);
    fprintf(stderr, "%-20s: %zu\n", "model2->nz",    model2->nz);
    fprintf(stderr, "%-20s: %lf\n", "model2->dx",    model2->dx);
    fprintf(stderr, "%-20s: %lf\n", "model2->dz",    model2->dz);
    fprintf(stderr, "%-20s: %lf\n", "model2->v_min", model2->v_min);
    fprintf(stderr, "%-20s: %lf\n", "model2->v_max", model2->v_max);

    // Compare if read model is the same as the created one
    for(size_t i = 0; i < nelem; i++) {
        if(fabs(model->vel[i] - model2->vel[i]) > eps) {
            fprintf(stderr, "(%zu) diverge %lf != %lf\n", i, model->vel[i], model2->vel[i]);
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "\nTestOK! All velocities are equal\n");
    unlink(filename);

    if(!isatty(fileno(stdout)))
        fwrite(model->vel, sizeof *model->vel, model->nx * model->nz, stdout);
    else 
        fprintf(stderr, "\nIf stdout is a file/pipe model will be written to it\n");

    velocity_model__destroy(model);
    velocity_model__destroy(model2);

    return 0;
}
