#ifndef _WAVEFIELDMPI_H_
#define _WAVEFIELDMPI_H_

#include <mpi.h>
#include "domain_comm.h"
#include "wavefield.h"

void wavefieldmpi__share_border_by_domain(wavefield *wave, laplacian_params *lp, domain_info *d);

void wavefieldmpi__perfect_match_layer_by_domain( wavefield *wave, velocity_model *model, laplacian_params *lp, domain_info *domain);


#endif
