#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "globals.h"
#include "parse.h"

#include "utils.h"
#include "tic.h"

#include "velocity_model.h"
#include "ricker.h"
#include "wavefield.h"
#include "simulation.h"


int main(int argc, char *argv[])
{
    args *ap = arg_parse(argc, argv, stdout);
    
    /* Create velocity model */
    velocity_model *model = velocity_model__create(ap->nx, ap->nz, ap->dx, ap->dz);
    velocity_model__constant_velocity(model, ap->vel);

    /* Create the source */
    ricker_wavelet *wavelet = ricker__create(ap->frequency);
    ricker_source  *source  = ricker__model(wavelet, ap->sx, ap->sz, ap->sd); 

    /* Create simulation */
    simulation_params *simulation = simulation__create();
    simulation->time = ap->time;
    simulation->sample = ap->sample;

    /* Create simulation */
    simulation->dt = stable_dt(model, source);
    simulation->steps = simulation->time / simulation->dt + 1;
    simulation->ntrec = simulation->sample/simulation->dt;

    /* Test stability */
    if(isstable(source, model, simulation) != SIMUL_OK)
    {
       fprintf(stderr, "Unstable model\n");
       exit(EXIT_FAILURE);
    }
    
    /* Create data */
    ricker__create_trace(wavelet, simulation->time, simulation->dt);
    wavefield *P = wavefield__create(model->nx, model->nz);

    /* Pre calculate  Laplacian parameters */
    laplacian_params *lp = wavefield__laplacian_params(model, ap->order, simulation->dt);
    
    /* Iterate timesteps */
    for(size_t it = 0; it < simulation->steps; it++)
    {
        simulation__inject_source(P, model, source, simulation, it);
        wavefield__laplacian(P, model, lp);
        wavefield__perfect_match_layer(P, model, lp);
        wavefield__swap(P);
        simulation__write(it, P, simulation, stdout);
    }

    /* Memory  cleanup */
    fflush(stdout);
    simulation__destroy(simulation);
    wavefield__destroy_laplacian_params(lp);
    wavefield__destroy(P);
    ricker__destroy_source(source);
    velocity_model__destroy(model);

    /* Finishing */
    if(ticprt) {
        fprintf(stderr, "TOTAL ");
        tic();
    }

    if(verbose)
        fprintf(stderr, "xmovie n1=%zu n2=%zu d1=%lf d2=%lf clip=0.5 loop=2\n", 
                ap->nx, ap->nz, ap->dx, ap->dz);

    fflush(stderr);

    return 0;
}
