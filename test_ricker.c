#include <stdio.h>

#include "ricker.h"

int main(int argc, char *argv[]) {

    float cnt_freq = 20.0;
    float cut_freq = ricker__cut2cnt(cut_freq);
    float period = ricker__period(cnt_freq);

    float dt = 0.0001;
    int maxiter = period/dt+1;

    // fprintf(stderr, "cut_freq: %f\n", cut_freq);
    // fprintf(stderr, "cnt_freq: %f\n", cnt_freq);
    // fprintf(stderr, "period  : %f\n", period);

    // fprintf(stderr, "maxiter: %d\n", maxiter);
    // fprintf(stderr, "dt: %f\n", dt);

    
    for(int i=0; i<2*maxiter; i++)
    {
        fprintf(stdout,"%.20f\n", ricker(i*dt, cnt_freq, -period));
    }

    return 0;
}
