#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "globals.h"
#include "help.h"

typedef struct args {
    double time;
    double sample;
    double frequency;
    size_t order;
    size_t sx;
    size_t sz;
    double sd;
    size_t nx;
    size_t nz;
    double dx;
    double dz;
    double vel;
    int verbose;
    int ticprt;
} args;

args * arg_parse(int argc, char *argv[], FILE *fd);

#endif
