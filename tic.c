#include <time.h>
#include <stdio.h>


double tic(void)
{
    static clock_t timen = 0;
    clock_t oldtime;
    oldtime = timen;

    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timen == 0)
    {
        fprintf(stderr, "%02d:%02d:%02d TIME: %.3lf elapsed: %.3lf seconds\n",
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                timen/(double)CLOCKS_PER_SEC, (timen-oldtime)/(double)CLOCKS_PER_SEC);
        timen = clock();
    }
    else
    {
        timen = clock();
        fprintf(stderr, "%02d:%02d:%02d TIME: %.3lf elapsed: %.3lf seconds\n",
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                timen/(double)CLOCKS_PER_SEC, (timen-oldtime)/(double)CLOCKS_PER_SEC);
    }
    return timen/(double)CLOCKS_PER_SEC;
}

