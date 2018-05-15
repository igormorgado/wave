#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "ricker.h"

int main(void) {

    const char filename[] = "testricker.bin";
    double frequency = 20.0;
    double dt = 0.008;
    double eps = 1e-32;

    fprintf(stderr, "\nEntered data\n");
    fprintf(stderr,   "====================================\n");
    fprintf(stderr, "%-20s: %s\n", "filename", filename);
    fprintf(stderr, "%-20s: %lf\n", "frequency", frequency);
    fprintf(stderr, "%-20s: %lf\n", "dt", dt);
    fprintf(stderr, "%-20s: %0.15lf\n", "eps", eps);

    // Create a  new wavelet
    ricker_wavelet *wavelet = ricker__create(frequency);

    fprintf(stderr, "\nGenerated Ricker wavelet information\n");
    fprintf(stderr,   "====================================\n");
    fprintf(stderr, "%-20s: %lf\n", "wavelet->frequency", wavelet->frequency);
    fprintf(stderr, "%-20s: %lf\n", "wavelet->period", wavelet->period);
    fprintf(stderr, "%-20s: %lf\n", "wavelet->shift", wavelet->shift);
    fprintf(stderr, "%-20s: %zu\n", "wavelet->size", wavelet->size);

    fprintf(stderr, "\nGenerated Ricker wavelet trace data\n");
    fprintf(stderr,   "====================================\n");
    double t = 2*wavelet->period;
    ricker__create_trace(wavelet, t, dt);
    fprintf(stderr, "%-20s: %lf\n", "time", t);
    fprintf(stderr, "%-20s: %zu\n", "wavelet->size", wavelet->size);

    for(size_t i = 0; i < wavelet->size; i++)
        fprintf(stderr, "%-16s[%02zu]: %lf\n", "wavelet->trace", i, wavelet->trace[i]);

    // Save wavelet to disk
    fprintf(stderr, "\nWriting wavelet to file %s.\n", filename);
    size_t nelem = ricker__write_to_file(filename, wavelet);
    fprintf(stderr, "It was written %zu trace elements of total %zu wavelet size\n",
            nelem, wavelet->size);

    fprintf(stderr, "\nReading wavelet from file %s.\n", filename);
    ricker_wavelet *wavelet2 = ricker__read_from_file(filename, frequency, wavelet->period, 0);

    fprintf(stderr, "\nRead Ricker wavelet information and data\n");
    fprintf(stderr,   "========================================\n");
    fprintf(stderr, "%-20s: %lf\n", "wavelet2->frequency", wavelet2->frequency);
    fprintf(stderr, "%-20s: %lf\n", "wavelet2->period", wavelet2->period);
    fprintf(stderr, "%-20s: %lf\n", "wavelet2->shift", wavelet2->shift);
    fprintf(stderr, "%-20s: %zu\n", "wavelet2->size", wavelet2->size);

    // Compare if read wavelet is the same as the created one
    for(size_t i = 0; i < nelem; i++) {
        if(fabs(wavelet->trace[i] - wavelet2->trace[i]) > eps) {
            fprintf(stderr, "(%zu) diverge %lf != %lf\n", i, wavelet->trace[i], wavelet2->trace[i]);
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "\nTestOK! All trace elements are equal\n");
    unlink(filename);

    if(!isatty(fileno(stdout)))
        fwrite(wavelet->trace, sizeof *wavelet->trace, wavelet->size, stdout);
    else 
        fprintf(stderr, "\nIf stdout is a file/pipe wavelet will be written to it\n");

    ricker__destroy(wavelet);
    ricker__destroy(wavelet2);

    return 0;
}
