/*
 * bddiff   Performs a binary difference in double files
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "utils.h"

#define BUFFER 4096

int main(int argc, char *argv[])
{
    
    if(argc != 2) {
        fprintf(stderr, "Performs a binary float subtraction between two datasets\n");
        fprintf(stderr, "\n\tDSET1 - DSET2 = RESULT\n");
        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, " $ %s DSET2.bin < DSET1.bin > RESULT.bin\n", argv[0]);
        fprintf(stderr, "\nAlso can be used in a pipe as:\n");
        fprintf(stderr, " $ cat DSET1.bin | %s DSET2.bin > RESULT.bin\n", argv[0]);
        fprintf(stderr, "\nDSET1 and DSET2 should be of same size if don't the output");
        fprintf(stderr, "is clipped on the shorter.\n");
        exit(1);
    }

    if(isatty(fileno(stdin))) {
        fprintf(stderr, "STDIN must be a 'pipe' or 'file'\n");
        exit(0);
    }

    if(isatty(fileno(stdout))) {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'\n");
        exit(1);
    }

    /* filenames */
    char filename[PATH_MAX];

    /* file buffers */
    double buf[BUFFER];
    double bufin[BUFFER];
    double bufout[BUFFER];

    /* Size read */
    size_t bytes_read_buf;
    size_t bytes_read_stdin;
    size_t bytes_read;

    strcpy(filename, argv[1]);
    FILE *fp = fopen(filename, "rb");

    while((bytes_read_buf = fread(buf, sizeof(double), BUFFER, fp)) &&
          (bytes_read_stdin =fread(bufin, sizeof(double), BUFFER, stdin)))
    {
        bytes_read = imin(bytes_read_buf, bytes_read_stdin);

        for(int i=0; i<bytes_read; i++)
            bufout[i] = bufin[i] - buf[i];

        fwrite(bufout, sizeof(double), bytes_read, stdout);
    }

    fclose(fp);

    return 0;
}
