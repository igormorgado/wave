#include <math.h>
#include <stdio.h>

#include "globals.h"
#include "simulation.h"
#include "tic.h"

double stable_dt(velocity_model *model, ricker_source *source) 
{
    /* Returns a stable dT based in velocity model parameters */

    double mu = 5.;          // Magic parameter
    double h = fmax(model->dx, model->dz); 

    return h / (mu * model->v_max);
}


SIMUL_STATUS isstable(ricker_source *source, 
                      velocity_model *model, 
                      simulation_params *simul)
{
    /* Returns if a given source is stable under a velocity model. Checks for
     * both, dispersion and stability.
     *
     * Returns:
     *  SIMUL_UNSTABLE | SIMUL_DISPERSIVE
     */

    SIMUL_STATUS simul_status = SIMUL_OK;

    double k  = 5.;                     // Dispersive constant k
    double mu = 5.;                     // Stability constant Mu

    double h = fmax(model->dx, model->dz);
    double f_max = ricker__cnt2cut(source->wavelet->frequency);

    /* First checks dispersion against grid cell size */
    if(fmax(model->dx, model->dz) > (model->v_min / (k*f_max)))
    {
        fprintf(stderr, "Grid cell is too large, Velocity Model minimum");
        fprintf(stderr, "velocity is too low or Source frequency is too high.");
        fprintf(stderr, "fix one of those.\n");
        fprintf(stderr, "h: %lf\tV_min: %lf\tFreq: %lf\n", h, model->v_min, f_max);
        simul_status &= SIMUL_DISPERSIVE;
    }

    if(simul->dt > (h / (mu * model->v_max)))
    {
        fprintf(stderr, "Simulation dT is too high, Grid size is too small or");
        fprintf(stderr, "model maximum velocity is too low, fix one of those.\n");
        fprintf(stderr, "dt: %lf\th: %lf\tV_max: %lf\n", simul->dt, h, model->v_max);
        simul_status &= SIMUL_UNSTABLE;
    }

    return simul_status;
} 


void simulation__inject_source(wavefield *w, velocity_model *m, ricker_source *s, size_t it)
{
    // Get position of injection in the flattened array
    size_t pos = w->nx * s->z + s->x;

    w->grid[pos] -= m->cube[pos] * s->wavelet->trace[it];

    if(verbose)
        fprintf(stderr, "%010zu injected @ [%zu,%zu] (pos)%zu %lf\n", it, w->nx, w->nz, pos, s->wavelet->trace[it]);
}


void simulation__write(size_t it, wavefield *w, simulation_params *s, FILE *fd)
{

    size_t ntrec = s->sample/s->dt;

    if( it % ntrec == 0)  {
        fwrite(w->grid, sizeof(double), w->nx * w->nz, fd);
        if(verbose) 
            fprintf(stderr, "Iteration step: %zu/%zu -- ", it, s->steps);

        if(ticprt)
            tic();
        
    }
}
