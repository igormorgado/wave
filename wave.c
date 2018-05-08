#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

#include "tic.h"

#include "simulation.h"
#include "velocity_model.h"
#include "ricker.h"

void print_usage(const char command[]) {
    fprintf(stderr, "%s: Simulate a 2d wave equation over a velocity field.\n\n", command);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s [PARAMETERS] > OUTPUT.bin\n\n", command);
    fprintf(stderr, "*** Redirection to a output file or a pipe is mandatory. ***\n");
    fprintf(stderr, "\nGeneral Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-help",            "This help");
    fprintf(stderr, "%-18s %s\n", "-verbose",         "Verbose mode. Default: False");
    fprintf(stderr, "\nSimulation Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-time   <double>", "Simulation time in seconds. Default: 1s");
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
    double t = 1.;                  // Simulation time
    double sample = 0.008;          // Sampling rate to save

    size_t nx = 500;                // Model grid size X axis
    size_t nz = 500;                // Model grid size Z axis
    double dx = 1.;                 // Model cell size X
    double dz = 1.;                 // Model cell size Z
    double vel = 1500.;             // Default constant velocity
                                    // Maybe instead add a velocity model
                                    // as input data from stdin

    double freq = 25;               // Ricker wavelet frequency

    double delay = 0.;              // Ricker source start delay
    size_t sx = nx/2 + 1;           // Ricker source position x
    size_t sz = nz/2 + 1;           // Ricker source position z

    int verbose = 0;                // != 0  -> verbose


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
        {"verbose", no_argument,        0,  'V'},   // verbose mode / with debug
        {"help",    no_argument,        0,  'h'},   // Call for help
        {0,         0,                  0,  0  }
    };

    /* 
     * Create and initialize structures
     * (data is created after, since we need correctly malloc)
     */
    velocity_model *model = velocity_model__create(nx, nz, dx, dz);

    ricker_wavelet *wavelet = ricker__create(freq);
    ricker_source  *source  = ricker__model(wavelet, sx, sz, delay); //model is awful name

    simulation_params simulation;
    simulation.time = t;
    simulation.sample = sample;

    // if(argc == 1) {
    //     print_usage(argv[0]);
    //     exit(EXIT_FAILURE);
    // }

    int opt = 0;
    int long_index = 0;
    while ((opt = getopt_long_only(argc, argv, "t:d:f:x:z:s:a:v:w:q:h", long_options, &long_index)) != -1)
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
                source->sx = atoi(optarg);
                break;
            case 'q':
                source->sz = atoi(optarg);
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
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(isatty(fileno(stdout))) {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'. If unsure use %s -help\n",argv[0]);
        exit(1);
    }


    /* Now all parameters are correctly set, we can create
     * the data inside the structures and check for stability
     */

    // Generate constant velocity model
    velocity_model__constant_cube(model, vel);

    simulation.dt = stable_dt(model, source);
    simulation.steps = simulation.time / simulation.dt + 1;

    // HERE I CAN CREATE RICKER TRACE
    ricker__create_trace(wavelet, simulation.time, simulation.dt);
    

    for(int i=0; i< simulation.steps; i++)
        fprintf(stderr, "%20.17lf\n", source->wavelet->trace[i]);

    // // HERE EVERYHING IS CREATE SIMULATION IS OK TO START
    //
    // // Allocate wavefield: MAYBE THIS GO TO SIMULATION LATER
    // double *P1[nz], *P2[nz], *Pt[nz];
    // for(int i = 0; i < nz; i++) {
    //     P1[i] = calloc( nx, sizeof **P1);
    //     P2[i] = calloc( nx, sizeof **P2);
    //     Pt[i] = calloc( nx, sizeof **Pt);
    // }

    // // Wave field debug
    // // for(int i = 0; i < nz; i++) {
    // //     for(int j = 0; j < nx; j++)
    // //         fprintf(stderr, "%2.1f ", P1[i][j]);
    // //     fprintf(stderr, "\n");
    // // }




    // // Mathematical parameters;
    // double lapx;
    // double lapz;

    // /* 2nd order derivative in space (already applied spatial squares)
    // double coef2[] = { -2, 1 };
    // double dxcoef2[2];
    // double dzcoef2[2];
    // for( int i=0; i< 2; i++)  {
    //     dxcoef2[i] = coef2[i]/(dx*dx);
    //     dzcoef2[i] = coef2[i]/(dz*dz);
    // }
    // */

    // /* 4th order derivative  in space  (already applied spatial squares)
    //  * should not be 4ths instead squares?
    // double coef4[] = { -5/2, 4/3, -1/12 };
    // double dxcoef4[3];
    // double dzcoef4[3];
    // for( int i=0; i< 3; i++)  {
    //     dxcoef4[i] = coef4[i]/(dx*dx);
    //     dzcoef4[i] = coef4[i]/(dz*dz);
    // }
    // */

    // /* 8th  order derivative in space (already applied spatial squares)
    //  * should not be 8ths instead squares?
    // double coef8[] = { -49/18, 3/2, -3/20, 1/90 };
    // double dxcoef8[4];
    // double dzcoef8[4];
    // for( int i=0; i< 4; i++)  {
    //     dxcoef8[i] = coef8[i]/(dx*dx);
    //     dzcoef8[i] = coef8[i]/(dz*dz);
    // }
    // */

    // // Iterate over timesteps THIS SHOULD GO SOMEWHERE ELSE
    // vel = powf(vel * dt, 2);

    // int ntrec = dtrec/dt;

    // for(int it = 0; it < simulation.steps; it++) {
    //     simulation__inject_source(*wavefield, *model, *source, it); // Right now only one source
    //     simulation__laplacian(*wavefield, *model);
    //     simulation__perfect_match_layer(*wavefield, *model);
    //     simulation__swap(wavefield, wavefield_t);
    //     simulation__save_step(wavefield);   // Only saves if matches sampling rate
    // }

    //     // Ricker should be multiplied by velocity in spatial position
    //     P1[sz][sx] -= vel*ricker_wave[it];
    //     //fprintf(stderr, "%010d Injected %lf\n", it, P1[sx][sz]);
    //
    //     // 8th order in space
    //     // for(int iz=3; iz < nz-3; iz++) {
    //     //     for(int ix=3; ix < nx-3; ix++) {
    //     //         lapx = dxcoef8[0] * P2[iz][ix];
    //     //         lapz = dzcoef8[0] * P2[iz][ix];
    //     //         for(int c=1; c < 4; c++) {
    //     //             lapx += dxcoef8[c]*(P2[iz-c][ix]+P2[iz+c][ix]);     // X direction
    //     //             lapz += dzcoef8[c]*(P2[iz][ix-c]+P2[iz][ix+c]);     // Z direction
    //     //         }
    //     //         P1[iz][ix] = 2*P2[iz][ix] - P1[iz][ix] + vel * (lapx + lapz);
    //     //     }
    //     // }
    //
    //     // 4th order in space
    //     // for(int iz=2; iz < nz-2; iz++) {
    //     //     for(int ix=2; ix < nx-2; ix++) {
    //     //         lapx = dxcoef4[0] * P2[iz][ix];
    //     //         lapz = dzcoef4[0] * P2[iz][ix];
    //     //         for(int c=1; c < 3; c++) {
    //     //             lapx += dxcoef4[c]*(P2[iz-c][ix]+P2[iz+c][ix]);     // X direction
    //     //             lapz += dzcoef4[c]*(P2[iz][ix-c]+P2[iz][ix+c]);     // Z direction
    //     //         }
    //     //         P1[iz][ix] = 2*P2[iz][ix] - P1[iz][ix] + vel * (lapx + lapz);
    //     //     }
    //     // }

    //     // SWAP
    //     for(int iz=0; iz < nz; iz++) {
    //         for(int ix=0; ix < nx; ix++) {
    //             Pt[iz][ix] = P2[iz][ix];
    //             P2[iz][ix] = P1[iz][ix];
    //             P1[iz][ix] = Pt[iz][ix];
    //         }
    //     }

    //     // Save each ith steps
    //     if( it % ntrec == 0) {
    //         fprintf(stderr, "Iteration step: %d/%d -- ", it,steps);
    //         tic();
    //         for(int iz=0; iz<nz; iz++)
    //             fwrite(P1[iz], sizeof(double), nx, stdout);
    //     }
    // }

    ricker__destroy_source(source);
    velocity_model__destroy(model);

    // fprintf(stderr, "xmovie n1=%d n2=%d d1=%lf d2=%lf clip=0.5 loop=2\n", nx, nz, dx, dz);
    // fprintf(stderr, "TOTAL ");
    // tic();

    fflush(stdout);
    fflush(stderr);

    return 0;
}
