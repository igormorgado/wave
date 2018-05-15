#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "ricker.h"

void print_tryhelp(const char command[]) {
    fprintf(stderr, "Try %s -help for more info\n", command);
}


void print_help(const char command[]) {
    fprintf(stderr, "%s: Writes a source  to a file..\n\n", command);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s [PARAMETERS] > OUTPUT.bin\n\n", command);
    fprintf(stderr, "*** Redirection to a output file or a pipe is mandatory. ***\n");
    fprintf(stderr, "\nGeneral Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-help",            "This help");
    fprintf(stderr, "%-18s %s\n", "-verbose",         "Verbose mode. Default: False");
    fprintf(stderr, "\nModel Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-freq  <double>",        "Base frequency. Default: 20");
    fprintf(stderr, "%-18s %s\n", "-time  <double>",        "Simulation time in secondds.  Default: .25");
    fprintf(stderr, "%-18s %s\n", "-dt  <double>",      "Sampling interval in secondds.  Default: .008");
}



int main(int argc, char *argv[]) {

    double time = .25;              // Simulation time
    double dt = 0.008;          // Sampling rate to save
    double frequency = 20;          // Ricker wavelet frequency
    double sd = 0.;                 // Ricker source start delay

    int opt = 0;
    int long_index = 0;
    int verbose = 0;

    static  struct option long_options[] = {
        {"time",    required_argument,  0,  't'},   // simumation time in seconds
        {"dt",  required_argument,  0,  'd'},   // Wave simulation dt
        {"freq",    required_argument,  0,  'f'},   // ricker wavelet frequency
        {"sd",      required_argument,  0,  'e'},   // Source delay
        {"verbose", no_argument,        0,  'V'},   // verbose mode / with debug
        {"help",    no_argument,        0,  'h'},   // Call for help
        {0,         0,                  0,  0  }
    };

    while ((opt = getopt_long_only(argc, argv, "t:d:f:o:x:z:s:a:v:w:q:e:ihV", long_options, &long_index)) != -1)
    {
        switch(opt) {
            case 't':
                time = atof(optarg); break;
            case 'd':
                dt = atof(optarg); break;
            case 'f':
                frequency = atof(optarg); break;
            case 'e':
                sd = atof(optarg); break;
            case 'V':
                verbose = 1; break;
            case 'h':
                print_help(argv[0]); exit(EXIT_FAILURE);
            default:
                print_tryhelp(argv[0]); exit(EXIT_FAILURE);
        }
    }

    if(verbose)
    {
        fprintf(stderr,"Command line parameters\n");
        fprintf(stderr,"%7s: %lf\n", "time", time);
        fprintf(stderr,"%7s: %lf\n", "dt", dt);
        fprintf(stderr,"%7s: %lf\n", "freq", frequency);
        fprintf(stderr,"%7s: %lf\n", "sd", sd);
        fprintf(stderr,"%7s: %d\n", "verbose", verbose);
    }

    if(isatty(fileno(stdout)))
    {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'.");
        print_tryhelp(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a  new wavelet
    ricker_wavelet *wavelet = ricker__create(frequency);
    ricker__create_trace(wavelet, time, dt);
    fwrite(wavelet->trace, sizeof *wavelet->trace, wavelet->size, stdout);
    ricker__destroy(wavelet);

    return 0;
}
