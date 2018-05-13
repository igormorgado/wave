#include "help.h"

void print_tryhelp(const char command[]) {
    fprintf(stderr, "Try %s -help for more info\n", command);
}

void print_help(const char command[]) {
    fprintf(stderr, "%s: Simulate a 2d wave equation over a velocity field.\n\n", command);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s [PARAMETERS] > OUTPUT.bin\n\n", command);
    fprintf(stderr, "*** Redirection to a output file or a pipe is mandatory. ***\n");
    fprintf(stderr, "\nGeneral Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-help",            "This help");
    fprintf(stderr, "%-18s %s\n", "-verbose",         "Verbose mode. Default: False");
    fprintf(stderr, "%-18s %s\n", "-tic",             "Show processing step timings. Default: True");
    fprintf(stderr, "\nSimulation Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-time   <double>", "Simulation time in seconds. Default: .25s");
    fprintf(stderr, "%-18s %s\n", "-sample <double>", "Sampling frequency to output. Default 0.008s");
    fprintf(stderr, "%-18s %s\n", "-order  <int>",    "Laplacian order. Default: 4.");
    fprintf(stderr, "\nWavelet Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-freq <double>",   "Ricker wavelet frequency in Hz. Default: 25Hz");
    fprintf(stderr, "\nModel Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-nx <int>",        "Number of cells in X axis. Default: 500");
    fprintf(stderr, "%-18s %s\n", "-nz <int>",        "Number of cells in Z axis. Default: 500");
    fprintf(stderr, "%-18s %s\n", "-dx <double>",     "Size of cells in X direction in meters. Default: 1");
    fprintf(stderr, "%-18s %s\n", "-dz <double>",     "Size of cells in Z direction in meters. Default: 1");
    fprintf(stderr, "\nSource Parameters:\n\n");
    fprintf(stderr, "%-18s %s\n", "-sx <int>",        "Source position in Grid X Component in units. Default: nx/2");
    fprintf(stderr, "%-18s %s\n", "-sz <int>",        "Source position in Grid Z Component in units. Default: nz/2");
    fprintf(stderr, "%-18s %s\n", "-sd <double>",     "Source delay. Default: 0");
}
