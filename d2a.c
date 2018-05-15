/*
 * Convert double binary data in float binary data
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 4096

int main(int argc, char *argv[])
{
    
    if(argc != 1) {
        fprintf(stderr, "Convert double to ascii file\n");
        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, " $ %s < doubledataset.bin > asciidataset.bin\n", argv[0]);
        fprintf(stderr, "\nAlso can be used in a pipe as:\n");
        fprintf(stderr, " $ cat doubledataset.bin | %s > asciidataset.bin \n", argv[0]);
        exit(1);
    }

    if(isatty(fileno(stdin))) {
        fprintf(stderr, "STDIN must be a 'pipe' or 'file'\n");
        exit(1);
    }

    if(isatty(fileno(stdout))) {
        fprintf(stderr, "STDOUT must be a 'pipe' or 'file'\n");
        exit(1);
    }

    /* file buffers */
    double bufin[BUFFER];
    size_t bytesread;

    while((bytesread = fread(bufin, sizeof(double), BUFFER, stdin)))
    {
        for(size_t i=0; i<bytesread; i++)
            fprintf(stdout, "%.15e\n", bufin[i]);
    }

    fflush(stdin);
    fflush(stdout);

    return 0;
}
