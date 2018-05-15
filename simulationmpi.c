#include "simulationmpi.h"

void simulationmpi__merge_wavefields(domain_info *domain) {
    for(int i=0; i < domain->world_size; i++) 
        fprintf(stderr, "Merging wavefield rank %d\n", i);

}

void simulationmpi__write(size_t it, wavefield *w, simulation_params *s, domain_info *d, int ticprt)
{
    
    FILE *fp;
    char filename[256];

    if( it % s->ntrec == 0)  {
        sprintf(filename, "wavefield.%04d.bin", d->world_rank);
        fp = fopen(filename, "wb+");
        fwrite(w->grid, sizeof(double), w->nx * w->nz, fp);
        //fprintf(stderr, "(%d) Saving iteration %7zu/%zu to %s\n", d->world_rank, it, s->steps, filename);
        fclose(fp);
        if(ticprt) {
            fprintf(stderr, "(%2d) Iteration step: %7zu/%7zu -- ", d->world_rank, it, s->steps);
            tic();
        }
        
    }
}
