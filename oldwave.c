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


float ricker(float t, float f, float s)
{
    /* return a ricker wavelet value.
     * Params:
     *  t: Time to evaluate the ricker wavelet
     *  f: central frequency
     *  s: timeshift
     *
     * For a *zero* phase set s to 0
     * For a *minimum* phase set s to -period
     */

    float p = powf(M_PI * f * (t+s), 2);
    return ( 1 - 2*p ) * exp(-p);
}

float ricker__cnt2cut(float central)
{
    /* Convert central frequency into cut frequency */
    return (central * M_PI)/sqrt(6);
}

float ricker__cut2cnt(float cut)
{
    /* Convert cut frequency into central frequency */
    return cut * (sqrt(6)/M_PI);
}

float ricker__period(float cnt_freq)
{
    /* Return the period of a ricker wavelet of central frequency given */
    return 6 / ( M_PI * cnt_freq * sqrt(2));
}

int main(int argc, char *argv[])
{
    if(isatty(fileno(stdout))) {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'\n");
        exit(1);
    }

    float t = .25;         /* Simulation time */
    float frequency = 20;   /* Ricker dominant frequency */
    int   nx = 500;         /* Number of X cells / columns */
    int   nz = 500;         /* Number of Z cells / lines */
    float dx = 1;           /* Size of X grid cell */  
    float dz = 1;           /* Size of Z grid cell */  
    float vel = 1500;       /* Water speed */
    int   sx = nx/2 + 1;          /* source X position */
    int   sz = nz/2 + 1;          /* source Z position */
    float   dtrec = 0.008;       /* save each 8ms */

    // Simulation parameters
    int steps;              /* Number of steps */
    float dt;               /* Simulation time step */
    
    // Ricker parameters
    float period;           /* Ricker wavelet period */

    // Mathematical parameters;
    float lapx;
    float lapz;
    float coef[] = { -5./2, 4./3, -1./12 };

    float dxcoef[3];
    float dzcoef[3];
    for( int i=0; i< 3; i++)  {
        dxcoef[i] = coef[i]/(dx*dx);
        dzcoef[i] = coef[i]/(dz*dz);
    }

    // TESTE DE ESTABILIDADE e DISPERSAO
    float mu = 5;
    float k = 5;
    float h = fmax(dx,dz);
    float freq_max = ricker__cnt2cut(frequency);
    float disp =  vel / (k*freq_max);
    float est = h / (mu * vel);
    dt = est;

    fprintf(stderr, "freq_dom: %f \t freq_max: %f\n", frequency, freq_max);

    fprintf(stderr, "h: %f <= [vel: %f / (k: %f * f: %f)] = disp: %f\n",
            h, vel, k, freq_max, disp);

    if (h > disp) {
        fprintf(stderr, "MODELAGEM DISPERSIVA h > disp\n");
        exit(1);
    }

    fprintf(stderr, "dt: %f <= [ h: %f / (mu: %f * vmax: %f) ] = est:  %f\n",
            dt, h, mu, vel, est);

    if (dt > est) {
        fprintf(stderr, "MODELAGEM INSTAVEL dt > est");
        exit(1);
    }

    steps = t/dt + 1;       /* Steps is a ratio between time and dt + 1 */
    period = ricker__period(frequency);

    float *ricker_wave = malloc(steps * sizeof *ricker_wave);

    // Create the ricker source array
    for(int i = 0; i < steps; i++)
        ricker_wave[i] = ricker(i * dt, frequency, -period);

    // Allocate wavefield 
    float *P1[nz], *P2[nz], *Pt[nz];
    for(int i = 0; i < nz; i++) {
        P1[i] = calloc(nx, sizeof (float));
        P2[i] = calloc(nx, sizeof (float));
        Pt[i] = calloc(nx, sizeof (float));
    }

    // Iterate over timesteps
    vel = powf(vel * dt, 2);

    int ntrec = dtrec/dt; 

    for(int it = 0; it < steps; it ++) {

        // Inject the source
        P1[sz][sx] += vel*ricker_wave[it];
        
        for(int iz=2; iz < nz-2; iz++) {
            for(int ix=2; ix < nx-2; ix++) {
                lapx = dxcoef[0] * P2[iz][ix];
                lapz = dzcoef[0] * P2[iz][ix];
                for(int c=1; c < 3; c++) {
                    lapx += dxcoef[c]*(P2[iz][ix-c]+P2[iz][ix+c]);     // X direction
                    lapz += dzcoef[c]*(P2[iz-c][ix]+P2[iz+c][ix]);     // Z direction
                }
                P1[iz][ix] = 2*P2[iz][ix] - P1[iz][ix] + vel * (lapx + lapz);
            }
        }

        // SWAP
        for(int iz=0; iz < nz; iz++) {
            for(int ix=0; ix < nx; ix++) {
                Pt[iz][ix] = P2[iz][ix];
                P2[iz][ix] = P1[iz][ix];
                P1[iz][ix] = Pt[iz][ix];
            }
        }

        // Save each ith steps
        if( it % ntrec == 0) {
            fprintf(stderr, "Iteration step: %7d/%7d  --  ", it, steps);
            tic();
            for(int iz=0; iz<nz; iz++) 
                fwrite(P1[iz], sizeof(float), nx, stdout);
        }
    }

    free(ricker_wave);
    for(int i = 0; i < nz; i++) {
        free(P1[i]);
        free(P2[i]);
        free(Pt[i]);
    }
    fflush(stdout);
    fflush(stderr);
    fprintf(stderr,"TOTAL ");
    tic();
    fprintf(stderr, "xmovie n1=%d n2=%d d1=%f d2=%f clip=0.5 loop=2 < \n", nx, nz, dx, dz);

    return 0;
}
