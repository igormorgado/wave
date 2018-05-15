#ifndef _SIMULATIONMPI_H_
#define _SIMULATIONMPI_H_

#include "domain_comm.h"
#include "wavefield.h"
#include "simulation.h"
#include "tic.h"

void simulationmpi__merge_wavefields(domain_info *domain);
void simulationmpi__write(size_t it, wavefield *w, simulation_params *s, domain_info *d, int ticprt);

#endif
