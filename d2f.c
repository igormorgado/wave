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
        fprintf(stderr, "Convert double to float binary file\n");
        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, " $ %s < doubledataset.bin > floatdataset.bin\n", argv[0]);
        fprintf(stderr, "\nAlso can be used in a pipe as:\n");
        fprintf(stderr, " $ cat doubledataset.bin | %s > floatdataset.bin \n", argv[0]);
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
    float bufout[BUFFER];
    size_t bytesread;

    while((bytesread = fread(bufin, sizeof(double), BUFFER, stdin)))
    {
        for(int i=0; i<BUFFER; i++)
            bufout[i] = bufin[i];

        fwrite(bufout, sizeof(float), bytesread, stdout);
    }

    fflush(stdin);
    fflush(stdout);

    return 0;
}
