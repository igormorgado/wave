#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "globals.h"
#include "parsempi.h"

#include "utils.h"
#include "tic.h"

#include "velocity_model.h"
#include "ricker.h"
#include "wavefield.h"
#include "simulation.h"
#include "domain_comm.h"


int main(int argc, char *argv[])
{

    args *ap = arg_parse(argc, argv);
    

    if(isstable_from_param(ap->frequency, ap->dx, ap->dz, ap->vel, ap->vel, ap->dt) != SIMUL_OK)
    {
       fprintf(stderr, "Unstable model\n");
       exit(EXIT_FAILURE);
    }
    

    /* Create simulation */
    simulation_params *simulation = simulation__create();
    simulation->time = ap->time;
    simulation->sample = ap->sample;
    simulation->steps = simulation->time / simulation->dt + 1;
    simulation->ntrec = simulation->sample/simulation->dt;


    /* Pre calculate  Laplacian parameters */
    laplacian_params *lp = wavefield__laplacian_params_(ap->dx, ap->dz, ap->order, simulation->dt);
    
    /***************************************************
     *
     * MPI Stuff
     *
     ***************************************************/
    MPI_Init(NULL,NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0 && world_size % 4 != 0) {
        fprintf(stderr, "Right now only multiples of 4 workers are supported\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    size_t number_slices_x = 4;
    size_t number_slices_z = world_size/4;

    domain_info *domain = malloc(sizeof *domain);
    decompose_domain(world_size, world_rank,
            number_slices_x, number_slices_z,
            lp->border_size, lp->border_size,
            ap->nx, ap->nz,
            ap->sx, ap->sz,
            domain);

    ricker_wavelet *wavelet;
    ricker_source *ricker;
    // We need to reposition the source to the local domain sx and sz
    if(domain->has_source) {
        wavelet = ricker__read_from_file( ap->sourcefile, ap->frequency, 0);
        ricker = ricker__model(wavelet, ap->sx, ap->sz, 0);
        ricker->x = domain->lsx;
        ricker->z = domain->lsz;
    }

    // Create a model that is part of full model based on local domain grid
    velocity_model *model = velocity_model__create_submodel_from_file(
            ap->modelfile,
            ap->nx, ap->nz, ap->dx, ap->dz,
            domain->nxa, domain->nza, domain->nxb, domain->nzb);

    wavefield *P = wavefield__create(domain->lnx, domain->lnz);

    /* Iterate timesteps */
    for(size_t it = 0; it < simulation->steps; it++)
    {
        if(domain->has_source)
            simulation__inject_source(P, model, ricker, simulation, it);

        wavefield__laplacian(P, model, lp);
    //PROCEED WITH CARE
    //     wavefield__perfect_match_layer_by_domain(P, model, lp, domain);
    // SEND BORDERS TO BROTHERS
        wavefield__swap(P);
    //PROCEED WITH CARE
    //     simulation__write_mpi(it, P, simulation, domain);
    }

    /* Memory  cleanup */
    fflush(stdout);

    wavefield__destroy(P);
    velocity_model__destroy(model);
    if(domain->has_source) {
        ricker__destroy_source(ricker);
    }
    free(domain);

    MPI_Finalize();

    wavefield__destroy_laplacian_params(lp);
    simulation__destroy(simulation);
    free(ap);

    // proceed with care
    // simulation__merge_wavefields();
    
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
