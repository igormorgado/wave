#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

#include "globals.h"

#include "utils.h"
#include "tic.h"

#include "velocity_model.h"
#include "ricker.h"
#include "wavefield.h"
#include "simulation.h"


void print_tryhelp(const char command[]) {
    fprintf(stderr, "Try %s -help for more info\n", command);
}

void print_help(const char command[]) {
    fprintf(stderr, "%s: Simulate a 2d wave equation over a velocity field.\n\n", command);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s [PARAMETERS] > OUTPUT.bin\n\n", command);
    fprintf(stderr, "*** Redirection to a output file or a pipe is mandatory. ***\n");
    fprintf(stderr, "\nGeneral Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-help",            "This help");
    fprintf(stderr, "%-18s %s\n", "-verbose",         "Verbose mode. Default: False");
    fprintf(stderr, "%-18s %s\n", "-tic",             "Show processing step timings. Default: True");
    fprintf(stderr, "\nSimulation Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-time   <double>", "Simulation time in seconds. Default: .25s");
    fprintf(stderr, "%-18s %s\n", "-sample <double>", "Sampling frequency to output. Default 0.008s");
    fprintf(stderr, "\nWavelet Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-freq <double>",   "Ricker wavelet frequency in Hz. Default: 25Hz");
    fprintf(stderr, "\nModel Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-nx <int>",        "Number of cells in X axis. Default: 500");
    fprintf(stderr, "%-18s %s\n", "-nz <int>",        "Number of cells in Z axis. Default: 500");
    fprintf(stderr, "%-18s %s\n", "-dx <double>",     "Size of cells in X direction in meters. Default: 1");
    fprintf(stderr, "%-18s %s\n", "-dz <double>",     "Size of cells in Z direction in meters. Default: 1");
    fprintf(stderr, "\nSource Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-sx <int>",        "Source position in Grid X Component in units. Default: nx/2");
    fprintf(stderr, "%-18s %s\n", "-sz <int>",        "Source position in Grid Z Component in units. Default: nz/2");
}

int main(int argc, char *argv[])
{

    /* Default values */
    double time = .25;               // Simulation time
    double sample = 0.008;          // Sampling rate to save

    size_t nx = 500;                // Model grid size X axis
    size_t nz = 500;                // Model grid size Z axis
    double dx = 1.;                 // Model cell size X
    double dz = 1.;                 // Model cell size Z
    double vel = 1500.;             // Default constant velocity
                                    // Maybe instead add a velocity model
                                    // as input data from stdin

    double freq = 20;               // Ricker wavelet frequency

    double delay = 0.;              // Ricker source start delay
    size_t sx = nx/2 + 1;           // Ricker source position x
    size_t sz = nz/2 + 1;           // Ricker source position z

    verbose = 0;
    ticprt = 1;

    velocity_model *model = velocity_model__create(nx, nz, dx, dz);

    ricker_wavelet *wavelet = ricker__create(freq);
    ricker_source  *source  = ricker__model(wavelet, sx, sz, delay); //model is awful name

    // Maybe create a function and work as a pointer... maybe...
    simulation_params simulation;
    simulation.time = time;
    simulation.sample = sample;


    /***************************************************
     *
     * Parse Arguments
     *
     **************************************************/
    int opt = 0;
    int long_index = 0;

    static  struct option long_options[] = {
        {"time",    required_argument,  0,  't'},   // simumation time in seconds
        {"sample",  required_argument,  0,  'd'},   // Wave simulation sampling
        {"freq",    required_argument,  0,  'f'},   // ricker wavelet frequency
        {"nx",      required_argument,  0,  'x'},   // number of cells in X dir
        {"nz",      required_argument,  0,  'z'},   // number of cells in Y dir
        {"dx",      required_argument,  0,  's'},   // Size of grid in X dir
        {"dz",      required_argument,  0,  'a'},   // Size of grid in Y dir
        {"sx",      required_argument,  0,  'w'},   // Source X position
        {"sz",      required_argument,  0,  'q'},   // Source Y position
        {"vel",     required_argument,  0,  'v'},   // Media constant velocity
        {"tic",     no_argument,        0,  'i'},   // Show tic steps
        {"verbose", no_argument,        0,  'V'},   // verbose mode / with debug
        {"help",    no_argument,        0,  'h'},   // Call for help
        {0,         0,                  0,  0  }
    };

    while ((opt = getopt_long_only(argc, argv, "t:d:f:x:z:s:a:v:w:q:ih", long_options, &long_index)) != -1)
    {
        switch(opt) {
            case 't':
                simulation.time = atof(optarg);
                break;
            case 'd':
                simulation.sample = atof(optarg);
                break;
            case 'f':
                source->wavelet->frequency = atof(optarg);
                break;
            case 'w':
                source->x = atoi(optarg);
                break;
            case 'q':
                source->z = atoi(optarg);
                break;
            case 'x':
                model->nx = atoi(optarg);
                break;
            case 'z':
                model->nz = atoi(optarg);
                break;
            case 's':
                model->dx = atof(optarg);
                break;
            case 'a':
                model->dz = atof(optarg);
                break;
            case 'v':
                vel = atof(optarg);
                break;
            case 'V':
                verbose = 1;
                break;
            case 'i':
                ticprt = 1;
                break;
            case 'h':
                print_help(argv[0]);
                exit(EXIT_FAILURE);
            default:
                print_tryhelp(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(verbose)
    {
        fprintf(stderr,"Command line parameters\n");
        fprintf(stderr,"%7s: %lf\n", "time", simulation.time);
        fprintf(stderr,"%7s: %lf\n", "sample", simulation.sample);
        fprintf(stderr,"%7s: %zu\n", "nx", model->nx);
        fprintf(stderr,"%7s: %zu\n", "nz", model->nz);
        fprintf(stderr,"%7s: %lf\n", "dx", model->dx);
        fprintf(stderr,"%7s: %lf\n", "dz", model->dz);
        fprintf(stderr,"%7s: %lf\n", "vel", vel);
        fprintf(stderr,"%7s: %lf\n", "freq", wavelet->frequency);
        fprintf(stderr,"%7s: %zu\n", "sx", source->x);
        fprintf(stderr,"%7s: %zu\n", "sz", source->z);
        fprintf(stderr,"%7s: %d\n", "verbose", verbose);
        fprintf(stderr,"%7s: %d\n", "tic", ticprt);
    }

    if(isatty(fileno(stdout)))
    {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'.");
        print_tryhelp(argv[0]);
        exit(EXIT_FAILURE);
    }


    /***************************************************
     *
     * Create velocity model
     *
     **************************************************/
    velocity_model__constant_velocity(model, vel);


    /***************************************************
     *
     * Model/Test stability
     *
     **************************************************/
    simulation.dt = stable_dt(model, source);
    simulation.steps = simulation.time / simulation.dt + 1;
    simulation.ntrec = simulation.sample/simulation.dt;

    // Test stability
    if(isstable(source, model, &simulation) != SIMUL_OK)
    {
       fprintf(stderr, "Unstable model\n");
       exit(EXIT_FAILURE);
    }

    
    /***************************************************
     *
     * Create data
     *
     **************************************************/
    ricker__create_trace(wavelet, simulation.time, simulation.dt);

    wavefield *P = wavefield__create(model->nx, model->nz);

    /***************************************************
     *
     * Laplacian parameters 
     *
     **************************************************/
    laplacian_params *lp = wavefield__laplacian_params(model, 4, simulation.dt);

    
    /***************************************************
     *
     * Iterate timesteps
     *
     **************************************************/
    for(size_t it = 0; it < simulation.steps; it++)
    {

        simulation__inject_source(P, model, source, &simulation, it);
        wavefield__laplacian(P, model, lp);
    //  wavefield__perfect_match_layer(P, *model);
        wavefield__swap(P);
        simulation__write(it, P, &simulation, stdout);
    }


    /***************************************************
     *
     * Memory  cleanup
     *
     **************************************************/
    fflush(stdout);
    wavefield__destroy_laplacian_params(lp);
    wavefield__destroy(P);
    ricker__destroy_source(source);
    velocity_model__destroy(model);


    /***************************************************
     *
     * Finishing
     *
     **************************************************/
    if(ticprt) {
        fprintf(stderr, "TOTAL ");
        tic();
    }

    if(verbose)
        fprintf(stderr, "xmovie n1=%zu n2=%zu d1=%lf d2=%lf clip=0.5 loop=2\n", nx, nz, dx, dz);

    fflush(stderr);

    return 0;
}
