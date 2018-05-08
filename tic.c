#include <time.h>
#include <math.h>
#include <stdio.h>


double tic(void)
{
    static clock_t time = 0;
    clock_t oldtime;
    oldtime = time;

    if (time == 0)
    {
        fprintf(stderr, "TIME: %.3lf elapsed: %.3lf seconds\n", time/(double)CLOCKS_PER_SEC, (time-oldtime)/(double)CLOCKS_PER_SEC);
        time = clock();
    }
    else
    {
        time = clock();
        fprintf(stderr, "TIME: %.3lf elapsed: %.3lf seconds\n", time/(double)CLOCKS_PER_SEC, (time-oldtime)/(double)CLOCKS_PER_SEC);
    }
    return time/(double)CLOCKS_PER_SEC;
}

