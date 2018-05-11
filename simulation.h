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
} simulation_params;


double stable_dt(velocity_model *model, ricker_source *source);

SIMUL_STATUS isstable(ricker_source *source, velocity_model *model, simulation_params *simul);

void simulation__inject_source(wavefield *w, velocity_model *m, ricker_source *s, simulation_params *simul, size_t it);

void simulation__write(size_t it, wavefield *w, simulation_params *s, FILE *fd);

#endif
