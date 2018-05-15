#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "simulation.h"
#include "ricker.h"
#include "velocity_model.h"
#include "tic.h"


void print_tryhelp(const char command[]) {
    fprintf(stderr, "Try %s -help for more info\n", command);
}


void print_help(const char command[]) {
    fprintf(stderr, "%s: Writes a good DT to stdout file..\n\n", command);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s [PARAMETERS]\n\n", command);
    fprintf(stderr, "\nGeneral Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-help",            "This help");
    fprintf(stderr, "\nModel Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-dx     <double>", "Model dx. Default: 1");
    fprintf(stderr, "%-18s %s\n", "-dz     <double>", "Model dz. Default: 1");
    fprintf(stderr, "%-18s %s\n", "-vmax   <double>", "Max model velocity.  Default: 1500");
    fprintf(stderr, "%-18s %s\n", "-vmin   <double>", "Min model velocity.  Default: 1500");
    fprintf(stderr, "%-18s %s\n", "-freq   <double>", "Source frequency. Default: 20Hz");
}



int main(int argc, char *argv[]) {

    double dx = 1.;
    double dz = 1.;
    double frequency = 20;          // Ricker wavelet frequency
    double v_min = 1500;
    double v_max = 1500;
    double dt;

    int opt = 0;
    int long_index = 0;

    static  struct option long_options[] = {
        {"dx",        required_argument,  0,  'x'},   // simumation time in seconds
        {"dz",        required_argument,  0,  'z'},   // Wave simulation sampling
        {"freq",      required_argument,  0,  'f'},   // ricker wavelet frequency
        {"vmin",      required_argument,  0,  'm'},   // Source delay
        {"vmax",      required_argument,  0,  'M'},   // Source delay
        {"help",      no_argument,        0,  'h'},   // Call for help
        {0,           0,                  0,  0  }
    };

    while ((opt = getopt_long_only(argc, argv, "x:z:f:m:M:Vh", long_options, &long_index)) != -1)
    {
        switch(opt) {
            case 'x':
                dx = atof(optarg); break;
            case 'z':
                dz = atof(optarg); break;
            case 'f':
                frequency = atof(optarg); break;
            case 'm':
                v_min = atof(optarg); break;
            case 'M':
                v_max = atof(optarg); break;
            case 'h':
                print_help(argv[0]); exit(EXIT_FAILURE);
            default:
                print_tryhelp(argv[0]); exit(EXIT_FAILURE);
        }
    }

    dt = stable_dt_from_param(dx, dz, v_max);

    fprintf(stdout, "%lf\n", dt);


    return 0;
}
