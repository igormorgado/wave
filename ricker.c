#include <math.h>

#include "ricker.h"
// #include "simulation.h"

ricker_wavelet *ricker__create(double frequency)
{
    // TODO: Maybe merge create trace here for a single memory allocation
    /* Return a ricker_wavelet structure without any traces */
    ricker_wavelet *wavelet = malloc(sizeof (ricker_wavelet));
    wavelet->frequency = frequency;
    wavelet->period = ricker__period(frequency);
    wavelet->shift = 0.;

    return wavelet;
}

ricker_source *ricker__model(ricker_wavelet *wavelet, size_t x, size_t z, double delay)
{
    ricker_source *source = malloc(sizeof (ricker_source));
    source->x = x;
    source->z = z;
    source->delay = delay;
    source->wavelet = wavelet;

    return source;
}


void ricker__create_trace(ricker_wavelet *wavelet, double time, double dt)
{
    /* Create the traces to ricker wavelet with shift 's' seconds
     * based on a time 't' and time discretization 'dt' seconds.
     */

    int steps = time / dt + 1;
    wavelet->trace = malloc(steps * sizeof (double));     // TODO: CHANGE FOR real tupe

    for(int i = 0; i < steps; i++)
        wavelet->trace[i] = ricker__points(i * dt, wavelet->frequency, -wavelet->period);
}


// FIXME: Using this function breaks compilation, since it has a self-refering
// function between  ricker and simulation.
// void ricker__create_trace_from_simulation(ricker_wavelet *wavelet, simulation_params *simulation)
// {
//     ricker__create_trace(wavelet, simulation->time, simulation->dt);
// }


void ricker__destroy(ricker_wavelet *wavelet)
{
    free(wavelet->trace);
    free(wavelet);
}


void ricker__destroy_source(ricker_source *source)
{
    ricker__destroy(source->wavelet);
    free(source);
}


double ricker__points(double t, double f, double s)
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

    double p = powf(M_PI * f * (t+s), 2);
    return ( 1 - 2*p ) * exp(-p);
}


double ricker__cnt2cut(double central)
{
    /* Convert central frequency into cut frequency */
    return (central * M_PI)/sqrt(6);
}

double ricker__cut2cnt(double cut)
{
    /* Convert cut frequency into central frequency */
    return cut * (sqrt(6)/M_PI);
}

double ricker__period(double cnt_freq)
{
    /* Return the period of a ricker wavelet of central frequency given */
    return 6 / ( M_PI * cnt_freq * sqrt(2));
}
