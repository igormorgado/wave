#include "globals.h"

int verbose = 0;
int ticprt = 0;

const double cfd2nd_2th_order[2] = { -2, 1};
const double cfd2nd_4th_order[3] = { -5/2, 4/3, -1/12};
const double cfd2nd_6th_order[4] = { -49/18, 3/2, -3/20, 1/90 };
const double cfd2nd_8th_order[5] = { -205/72, 8/5, -1/5, 8/315, -1/560 };
