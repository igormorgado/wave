#ifndef _SIMULATION_H_
#define _SIMULATION_H_

/* TODO:
 *  - Add simulation Time order parameters
 *  - Add simulation Spatial order parameters
 */

#include <stdlib.h>
#include <stdint.h>

#include "globals.h"
#include "ricker.h"
#include "velocity_model.h"
#include "wavefield.h"
#include "domain_comm.h"

typedef uint8_t SIMUL_STATUS;

#define SIMUL_OK            0
#define SIMUL_DISPERSIVE    1
#define SIMUL_UNSTABLE      2

typedef struct simulation_params 
{
    double time;            /* Simulation time */
    double dt;              /* Simulation timestep */
    double sample;          /* Sampling time */
    size_t steps;           /* Number of simulation steps */
    size_t ntrec;           /* Frames to be saved */
} simulation_params;


simulation_params * simulation__create();

void simulation__destroy(simulation_params *sp);

double stable_dt_from_param(double dx, double dz, double v_max);
double stable_dt(velocity_model *model);

SIMUL_STATUS isstable_from_param(double frequency, double dx, double dz, double v_min, double v_max, double dt);
SIMUL_STATUS isstable(ricker_source *source, velocity_model *model, simulation_params *simul);

void simulation__inject_source(wavefield *w, velocity_model *m, ricker_source *s, simulation_params *simul, size_t it);

void simulation__write(size_t it, wavefield *w, simulation_params *s, FILE *fd, int ticprt);
void simulation__write_mpi(size_t it, wavefield *w, simulation_params *s, domain_info *d, int ticprt);


#endif
