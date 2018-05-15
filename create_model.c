#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "velocity_model.h"

void print_tryhelp(const char command[]) {
    fprintf(stderr, "Try %s -help for more info\n", command);
}


void print_help(const char command[]) {
    fprintf(stderr, "%s: Writes a velocity field to a file..\n\n", command);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s [PARAMETERS] > OUTPUT.bin\n\n", command);
    fprintf(stderr, "*** Redirection to a output file or a pipe is mandatory. ***\n");
    fprintf(stderr, "\nGeneral Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-help",            "This help");
    fprintf(stderr, "%-18s %s\n", "-verbose",         "Verbose mode. Default: False");
    fprintf(stderr, "\nModel Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-nx  <int>",        "Number of cells in X axis. Default: 600");
    fprintf(stderr, "%-18s %s\n", "-nz  <int>",        "Number of cells in Z axis. Default: 600");
    fprintf(stderr, "%-18s %s\n", "-dx  <double>",     "Size of cells in X direction in meters. Default: 1");
    fprintf(stderr, "%-18s %s\n", "-dz  <double>",     "Size of cells in Z direction in meters. Default: 1");
    fprintf(stderr, "%-18s %s\n", "-vel <double>",     "Velocity. Default: 1");
}


int main(int argc, char *argv[]) {

    // DEFAULTS 
    size_t nx = 600;                // Model grid size X axis
    size_t nz = 600;                // Model grid size Z axis
    double dx = 1.;                 // Model cell size X
    double dz = 1.;                 // Model cell size Z
    double vel = 1500.;             // Default constant velocity
                                    // Maybe instead add a velocity model
                                    // as input data from stdin

    int verbose = 0;
    int opt = 0;
    int long_index = 0;

    static  struct option long_options[] = {
        {"nx",      required_argument,  0,  'x'},   // number of cells in X dir
        {"nz",      required_argument,  0,  'z'},   // number of cells in Y dir
        {"dx",      required_argument,  0,  's'},   // Size of grid in X dir
        {"dz",      required_argument,  0,  'a'},   // Size of grid in Y dir
        {"vel",     required_argument,  0,  'v'},   // Media constant velocity
        {"verbose", no_argument,        0,  'V'},   // verbose mode / with debug
        {"help",    no_argument,        0,  'h'},   // Call for help
        {0,         0,                  0,  0  }
    };

    while ((opt = getopt_long_only(argc, argv, "x:z:s:a:v:Vh", long_options, &long_index)) != -1)
    {
        switch(opt) {
            case 'x':
                nx = atoi(optarg); break;
            case 'z':
                nz = atoi(optarg); break;
            case 's':
                dx = atof(optarg); break;
            case 'a':
                dz = atof(optarg); break;
            case 'v':
                vel = atof(optarg); break;
            case 'V':
                verbose = 1; break;
            case 'h':
                print_help(argv[0]); exit(EXIT_FAILURE);
            default:
                print_tryhelp(argv[0]); exit(EXIT_FAILURE);
        }
    }

    if(verbose)
    {
        fprintf(stderr,"Command line parameters\n");
        fprintf(stderr,"%7s: %zu\n", "nx", nx);
        fprintf(stderr,"%7s: %zu\n", "nz", nz);
        fprintf(stderr,"%7s: %lf\n", "dx", dx);
        fprintf(stderr,"%7s: %lf\n", "dz", dz);
        fprintf(stderr,"%7s: %lf\n", "vel", vel);
        fprintf(stderr,"%7s: %d\n", "verbose", verbose);
    }

    if(isatty(fileno(stdout)))
    {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'.");
        print_tryhelp(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a  new wavelet
    velocity_model *model = velocity_model__create(nx, nz, dx, dz);
    velocity_model__constant_velocity(model, vel);

    fwrite(model->vel, sizeof *model->vel, model->nx * model->nz, stdout);

    velocity_model__destroy(model);

    return 0;
}
