#include <math.h>
#include <stdio.h>

#include "globals.h"
#include "simulation.h"
#include "tic.h"

simulation_params * simulation__create(void)
{
    simulation_params *sp = malloc(sizeof *sp);
    return sp;
}

void simulation__destroy(simulation_params *sp)
{
    free(sp);
}

double stable_dt_from_param(double dx, double dz, double v_max)
{
    double mu = 5.;          // Magic parameter
    double h = fmax(dx, dz); 

    return h / (mu * v_max);
}


double stable_dt(velocity_model *model) 
{
    /* Returns a stable dT based in velocity model parameters */
    return stable_dt_from_param(model->dx, model->dx, model->v_max);
}


SIMUL_STATUS isstable_from_param(double frequency, 
        double dx, double dz, 
        double v_min, double v_max,
        double dt)
{

    SIMUL_STATUS simul_status = SIMUL_OK;

    double k  = 5.;                     // Dispersive constant k
    double mu = 5.;                     // Stability constant Mu

    double h = fmax(dx, dz);
    double f_max = ricker__cnt2cut(frequency);

    /* First checks dispersion against grid cell size */
    if(fmax(dx, dz) > (v_min / (k*f_max)))
    {
        fprintf(stderr, "Grid cell is too large, velocity model minimum ");
        fprintf(stderr, "velocity is too low or source frequency is too high.");
        fprintf(stderr, "fix one of those.\n");
        fprintf(stderr, "h: %lf\tV_min: %lf\tFreq: %lf\n", h, v_min, f_max);
        simul_status &= SIMUL_DISPERSIVE;
    }

    if(dt > (h / (mu * v_max)))
    {
        fprintf(stderr, "Simulation dT is too high, Grid size is too small or");
        fprintf(stderr, "model maximum velocity is too low, fix one of those.\n");
        fprintf(stderr, "dt: %lf\th: %lf\tV_max: %lf\n", dt, h, v_max);
        simul_status &= SIMUL_UNSTABLE;
    }

    return simul_status;
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

    return isstable_from_param(source->wavelet->frequency,
            model->dx, model->dz, model->v_min, model->v_max, simul->dt);
} 


void simulation__inject_source(wavefield *w, velocity_model *m, ricker_source *s, simulation_params *simul, size_t it)
{
    // Get position of injection in the flattened array
    size_t pos = w->nx * s->z + s->x;
    double vel_dt2 = powf(m->vel[pos] * simul->dt, 2); 

    w->grid[pos] += vel_dt2 * s->wavelet->trace[it];
}


void simulation__write(size_t it, wavefield *w, simulation_params *s, FILE *fd, int ticprt)
{


    if( it % s->ntrec == 0)  {
        fwrite(w->grid, sizeof(double), w->nx * w->nz, fd);

        if(ticprt) {
            fprintf(stderr, "Iteration step: %7zu/%7zu -- ", it, s->steps);
            tic();
        }
        
    }
}
