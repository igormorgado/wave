#include <math.h>
#include "tic.h"

int main(int argc, char *argv[])
{
    for(int j=0; j<10; j++) {
        tic();
        for(int i=0; i< 100000000; i++)
            sqrt(i);
    }
    return 0;
}
