#include "utils.h"

/* Function to find minimum of x and y without branching */

int imin(int x, int y)
{
  return y ^ ((x ^ y) & -(x < y));
}
 
/* Function to find maximum of x and y */
int imax(int x, int y)
{
  return x ^ ((x ^ y) & -(x < y)); 
}
