#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

double tic(void)
{
    static clock_t time = 0;
    clock_t oldtime;
    oldtime = time;

    if (time == 0)
    {
        fprintf(stderr, "TIME: %.3lf elapsed: %.3lf seconds\n", time/(double)CLOCKS_PER_SEC, (time-oldtime)/(double)CLOCKS_PER_SEC);
        time = clock();
    }
    else
    {
        time = clock();
        fprintf(stderr, "TIME: %.3lf elapsed: %.3lf seconds\n", time/(double)CLOCKS_PER_SEC, (time-oldtime)/(double)CLOCKS_PER_SEC);
    }
    return time/(double)CLOCKS_PER_SEC;
}


double ricker(double t, double f, double s)
{
    /* return a ricker wavelet value.
     * Params:
     *  t: Time to evaluate the ricker wavelet
     *  f: central freq
     *  s: timeshift
     *
     * For a *zero* phase set s to 0
     * For a *minimum* phase set s to -period
     */

    double p = powf(M_PI * f * (t+s), 2);
    return ( 1 - 2*p ) * exp(-p);
}

double ricker__cnt2cut(double central)
{
    /* Convert central freq into cut freq */
    return (central * M_PI)/sqrt(6);
}

double ricker__period(double cnt_freq)
{
    /* Return the period of a ricker wavelet of central freq given */
    return 6 / ( M_PI * cnt_freq * sqrt(2));
}

// **************************************************************************
// CODES ARE EXACLTY THE SAME TILL HERE
// **************************************************************************
int main(int argc, char *argv[])
{
    /***************************************************
     *
     * Default parameters 
     *
     **************************************************/
    double time = .25;          /* Simulation time */
    double sample = 0.008;      /* save each 8ms */
    size_t steps;               /* Number of steps */
    double dt;                  /* Simulation time step */

    // Model Parameters
    int   nx = 500;             /* Number of X cells / columns */
    int   nz = 500;             /* Number of Z cells / lines */
    double dx = 1;              /* Size of X grid cell */  
    double dz = 1;              /* Size of Z grid cell */  
    double vel = 1500;          /* Water speed */

    // Source Parameters
    double freq = 20;           /* Ricker dominant freq */
    double  period = 0.;         /* Ricker wavelet period */
    size_t  sx = nx/2 + 1;      /* source X position */
    size_t  sz = nz/2 + 1;      /* source Z position */

    size_t ntrec;               /* Save wavefield interval */
    int verbose = 0;            /* verbose running */


    /***************************************************
     *
     * Parse Arguments
     *
     **************************************************/
    if(isatty(fileno(stdout))) {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'\n");
        exit(1);
    }

    
    /***************************************************
     *
     * Model/Test stability
     *
     **************************************************/
    double mu = 5.;
    double k = 5.;
    double h = fmax(dx,dz);
    double freq_max = ricker__cnt2cut(freq);
    double disp = vel / (k*freq_max);
    double est = h / (mu * vel);

    // stable_dt()
    dt = est;
    steps = time/dt + 1;       /* Steps is a ratio between time and dt + 1 */
    ntrec = sample/dt; 

    period = ricker__period(freq);

    // isstable()
    if (h > disp) {
        fprintf(stderr, "Unstable model\n");
        exit(1);
    }


    /***************************************************
     *
     * Create data
     *
     **************************************************/

    // ricker__create_trace()
    double *ricker_wave = malloc(steps * sizeof *ricker_wave);
    for(int i = 0; i < steps; i++)
        ricker_wave[i] = ricker(i * dt, freq, -period);


    // wavefield__create()
    double *P1[nz], *P2[nz], *Pt[nz];
    for(int i = 0; i < nz; i++) {
        P1[i] = calloc(nx, sizeof (double));
        P2[i] = calloc(nx, sizeof (double));
        Pt[i] = calloc(nx, sizeof (double));
    }


    /***************************************************
     *
     * Laplacian parameters 
     *
     **************************************************/


    // wavefield__laplacian_params()
    double lapx;
    double lapz;
    double coef[] = { -5./2, 4./3, -1./12 };

    size_t border_size = 2;
    size_t coef_len = 3;

    double dxdx = dx*dx;
    double dzdz = dz*dz;

    double coef_dx2[coef_len];
    double coef_dz2[coef_len];
    for(size_t i=0; i< coef_len; i++)  {
        coef_dx2[i] = coef[i]/dxdx;
        coef_dz2[i] = coef[i]/dzdz;
    }


    /***************************************************
     *
     * Iterate timesteps
     *
     **************************************************/
    double vel_dt2;
    for(size_t it = 0; it < steps; it ++) {

        // Calculate (vel/dt)^2
        vel_dt2 = powf(vel * dt, 2);

        // simulation__inject_source;
        P1[sz][sx] += vel_dt2*ricker_wave[it];
        
        // wavefield__laplacian
        for(size_t iz=border_size; iz < nz-border_size; iz++) {
            for(size_t ix=border_size; ix < nx-border_size; ix++) {

                // Stencil center
                lapx = coef_dx2[0] * P2[iz][ix];
                lapz = coef_dz2[0] * P2[iz][ix];

                // Stencil borders
                for(size_t ic=1; ic < coef_len; ic++) {
                    lapx += coef_dx2[ic]*(P2[iz][ix-ic]+P2[iz][ix+ic]);     // X direction
                    lapz += coef_dz2[ic]*(P2[iz-ic][ix]+P2[iz+ic][ix]);     // Z direction
                }

                // Second order timestep
                P1[iz][ix] = 2*P2[iz][ix] - P1[iz][ix] + vel_dt2 * (lapx + lapz);
            }
        }

        // wavefield__swap
        for(size_t iz=0; iz < nz; iz++) {
            for(size_t ix=0; ix < nx; ix++) {
                Pt[iz][ix] = P2[iz][ix];
                P2[iz][ix] = P1[iz][ix];
                P1[iz][ix] = Pt[iz][ix];
            }
        }

        // simulation__write
        if( it % ntrec == 0) {
            for(int iz=0; iz<nz; iz++) 
                fwrite(P1[iz], sizeof(double), nx, stdout);

            fprintf(stderr, "Iteration step: %7zu/%7zu  --  ", it, steps);
            tic();
        }
    }


    /***************************************************
     *
     * Memory  cleanup
     *
     **************************************************/
    fflush(stdout);
    free(ricker_wave);
    for(int i = 0; i < nz; i++) {
        free(P1[i]);
        free(P2[i]);
        free(Pt[i]);
    }


    /***************************************************
     *
     * Finishing
     *
     **************************************************/
    fprintf(stderr,"TOTAL ");
    tic();

    if(verbose)
        fprintf(stderr, "xmovie n1=%d n2=%d d1=%lf d2=%lf clip=0.5 loop=2 < \n", nx, nz, dx, dz);

    fflush(stderr);

    return 0;
}
