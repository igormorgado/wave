#ifndef _RICKER_H_
#define _RICKER_H_

#include <stdlib.h>

/* A ricker_wavelet, is a pure wavelet without any relationship with
 * a model or simulation. It stores only information about the wavelet.
 */
typedef struct ricker_wavelet
{
    double frequency;
    double period;
    double shift;
    size_t size;
    double *trace;
} ricker_wavelet;

/* A ricker_source, is a ricker wavelet associated with a velocity model
 * position and simulation parameters
 */
typedef struct ricker_source
{
    ricker_wavelet *wavelet;
    size_t x;                  /* Source X location in grid */
    size_t z;                  /* Source Z location in grid */
    double delay;               /* Source delay */
} ricker_source;


ricker_wavelet *ricker__create(double frequency);

ricker_source  *ricker__model(ricker_wavelet *wavelet, size_t x, size_t z, double delay);

size_t ricker__write_to_file(const char filename[], ricker_wavelet *wavelet);

ricker_wavelet * ricker__read_from_file(const char filename[], double frequency, double shift);

size_t ricker__create_trace(ricker_wavelet *wavelet, double time, double dt);

void ricker__destroy(ricker_wavelet *wavelet);

void ricker__destroy_source(ricker_source *source);

double ricker__points(double t, double f, double s);

double ricker__cnt2cut(double central);

double ricker__cut2cnt(double cut);

double ricker__period(double cnt_freq);

#endif
