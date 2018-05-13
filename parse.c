#include "parse.h"

args * arg_parse(int argc, char *argv[], FILE *fd)
{
    /* Default values */
    double time = .25;              // Simulation time
    double sample = 0.008;          // Sampling rate to save
    size_t order = 4;               // Simulation order

    size_t nx = 500;                // Model grid size X axis
    size_t nz = 500;                // Model grid size Z axis
    double dx = 1.;                 // Model cell size X
    double dz = 1.;                 // Model cell size Z
    double vel = 1500.;             // Default constant velocity
                                    // Maybe instead add a velocity model
                                    // as input data from stdin

    double frequency = 20;          // Ricker wavelet frequency

    double sd = 0.;                 // Ricker source start delay
    size_t sx = nx/2 + 1;           // Ricker source position x
    size_t sz = nz/2 + 1;           // Ricker source position z

    verbose = 0;
    ticprt = 1;

    int opt = 0;
    int long_index = 0;

    static  struct option long_options[] = {
        {"time",    required_argument,  0,  't'},   // simumation time in seconds
        {"sample",  required_argument,  0,  'd'},   // Wave simulation sampling
        {"freq",    required_argument,  0,  'f'},   // ricker wavelet frequency
        {"order",   required_argument,  0,  'o'},   // Simulation order
        {"nx",      required_argument,  0,  'x'},   // number of cells in X dir
        {"nz",      required_argument,  0,  'z'},   // number of cells in Y dir
        {"dx",      required_argument,  0,  's'},   // Size of grid in X dir
        {"dz",      required_argument,  0,  'a'},   // Size of grid in Y dir
        {"sx",      required_argument,  0,  'w'},   // Source X position
        {"sz",      required_argument,  0,  'q'},   // Source Y position
        {"sd",      required_argument,  0,  'e'},   // Source delay
        {"vel",     required_argument,  0,  'v'},   // Media constant velocity
        {"tic",     no_argument,        0,  'i'},   // Show tic steps
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
                sample = atof(optarg); break;
            case 'f':
                frequency = atof(optarg); break;
            case 'o':
                order = atoi(optarg); break;
            case 'w':
                sx = atoi(optarg); break;
            case 'q':
                sz = atoi(optarg); break;
            case 'e':
                sd = atof(optarg); break;
            case 'x':
                nx = atoi(optarg); break;
            case 'z':
                nz = atoi(optarg); break;
            case 's':
                dx = atof(optarg); break;
            case 'a':
                dz = atof(optarg); break;
            case 'v':
                vel = atof(optarg); break;
            case 'V':
                verbose = 1; break;
            case 'i':
                ticprt = 1; break;
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
        fprintf(stderr,"%7s: %lf\n", "sample", sample);
        fprintf(stderr,"%7s: %zu\n", "order", order);
        fprintf(stderr,"%7s: %zu\n", "nx", nx);
        fprintf(stderr,"%7s: %zu\n", "nz", nz);
        fprintf(stderr,"%7s: %lf\n", "dx", dx);
        fprintf(stderr,"%7s: %lf\n", "dz", dz);
        fprintf(stderr,"%7s: %lf\n", "vel", vel);
        fprintf(stderr,"%7s: %lf\n", "freq", frequency);
        fprintf(stderr,"%7s: %zu\n", "sx", sx);
        fprintf(stderr,"%7s: %zu\n", "sz", sz);
        fprintf(stderr,"%7s: %lf\n", "sd", sd);
        fprintf(stderr,"%7s: %d\n", "verbose", verbose);
        fprintf(stderr,"%7s: %d\n", "tic", ticprt);
    }

    if(isatty(fileno(fd)))
    {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'.");
        print_tryhelp(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Now allocate everything in the arg parse struct
    args *ap = malloc(sizeof *ap);

    ap->time = time;
    ap->sample = sample;
    ap->frequency = frequency;
    ap->order = order;
    ap->sx = sx;
    ap->sz = sz;
    ap->sd = sd;
    ap->nx = nx;
    ap->nz = nz;
    ap->dx = dx;
    ap->dz = dz;
    ap->vel = vel;
    ap->verbose = verbose;
    ap->ticprt = ticprt;

    return ap;
}
