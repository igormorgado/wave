#include "parsempi.h"

args * arg_parse(int argc, char *argv[])
{
    /* Default values */
    double time = .25;              // Simulation time
    double sample = 0.008;          // Sampling rate to save
    size_t order = 4;               // Simulation order
    double dt = 0.0001333333333;

    size_t nx = 600;                // Model grid size X axis
    size_t nz = 600;                // Model grid size Z axis
    double dx = 1.;                 // Model cell size X
    double dz = 1.;                 // Model cell size Z
    double vel = 1500.;             // Default constant velocity
                                    // Maybe instead add a velocity model
                                    // as input data from stdin

    double frequency = 20;          // Ricker wavelet frequency

    size_t sx = nx/2 + 1;           // Ricker source position x
    size_t sz = nz/2 + 1;           // Ricker source position z

    char modelfile[256];
    char sourcefile[256];

    verbose = 0;
    ticprt = 1;

    int opt = 0;
    int long_index = 0;

    static  struct option long_options[] = {
        {"time",    required_argument,  0,  't'},   // simumation time in seconds
        {"dt",      required_argument,  0,  'k'},   // Size of grid in Y dir
        {"sample",  required_argument,  0,  'd'},   // Wave simulation sampling
        {"order",   required_argument,  0,  'o'},   // Simulation order
        {"nx",      required_argument,  0,  'x'},   // number of cells in X dir
        {"nz",      required_argument,  0,  'z'},   // number of cells in Y dir
        {"dx",      required_argument,  0,  's'},   // Size of grid in X dir
        {"dz",      required_argument,  0,  'a'},   // Size of grid in Y dir
        {"vel",     required_argument,  0,  'v'},   // Media constant velocity
        {"model",   required_argument,  0,  'm'},   // Velocity Model file
        {"freq",    required_argument,  0,  'f'},   // ricker wavelet frequency
        {"sx",      required_argument,  0,  'w'},   // Source X position
        {"sz",      required_argument,  0,  'q'},   // Source Y position
        {"source",  required_argument,  0,  'n'},   // Source  file
        {"tic",     no_argument,        0,  'i'},   // Show tic steps
        {"verbose", no_argument,        0,  'V'},   // verbose mode / with debug
        {"help",    no_argument,        0,  'h'},   // Call for help
        {0,         0,                  0,  0  }
    };

    while ((opt = getopt_long_only(argc, argv, "t:k:d:o:x:z:s:a:v:m:f:w:q:n:Vih", long_options, &long_index)) != -1)
    {
        switch(opt) {
            case 't':
                time = atof(optarg); break;
            case 'k':
                dt = atof(optarg); break;
            case 'd':
                sample = atof(optarg); break;
            case 'o':
                order = atoi(optarg); break;
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
            case 'm':
                strcpy(modelfile, optarg); break;
            case 'f':
                frequency = atof(optarg); break;
            case 'w':
                sx = atoi(optarg); break;
            case 'q':
                sz = atoi(optarg); break;
            case 'n':
                strcpy(sourcefile, optarg); break;
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
        fprintf(stderr,"%7s: %lf\n", "dt", dt);
        fprintf(stderr,"%7s: %lf\n", "sample", sample);
        fprintf(stderr,"%7s: %zu\n", "order", order);
        fprintf(stderr,"%7s: %zu\n", "nx", nx);
        fprintf(stderr,"%7s: %zu\n", "nz", nz);
        fprintf(stderr,"%7s: %lf\n", "dx", dx);
        fprintf(stderr,"%7s: %lf\n", "dz", dz);
        fprintf(stderr,"%7s: %lf\n", "vel", vel);
        fprintf(stderr,"%7s: %s\n", "modelfile", modelfile);
        fprintf(stderr,"%7s: %lf\n", "freq", frequency);
        fprintf(stderr,"%7s: %zu\n", "sx", sx);
        fprintf(stderr,"%7s: %zu\n", "sz", sz);
        fprintf(stderr,"%7s: %s\n", "sourcefile", sourcefile);
        fprintf(stderr,"%7s: %d\n", "verbose", verbose);
        fprintf(stderr,"%7s: %d\n", "tic", ticprt);
    }

    // Now allocate everything in the arg parse struct
    args *ap = malloc(sizeof *ap);

    ap->time = time;
    ap->dt = dt;
    ap->sample = sample;
    ap->order = order;
    ap->nx = nx;
    ap->nz = nz;
    ap->dx = dx;
    ap->dz = dz;
    ap->vel = vel;
    strcpy(ap->modelfile, modelfile);
    ap->frequency = frequency;
    ap->sx = sx;
    ap->sz = sz;
    strcpy(ap->sourcefile, sourcefile);
    ap->verbose = verbose;
    ap->ticprt = ticprt;

    return ap;
}
