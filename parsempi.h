#ifndef _PARSEMPI_H_
#define _PARSEMPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "globals.h"
#include "help.h"

typedef struct args {
    double time;
    double dt;
    double sample;
    size_t order;
    size_t nx;
    size_t nz;
    double dx;
    double dz;
    double vel;
    char modelfile[256];
    double frequency;
    size_t sx;
    size_t sz;
    char sourcefile[256];
    int verbose;
    int ticprt;
} args;

args * arg_parse(int argc, char *argv[]);

#endif
