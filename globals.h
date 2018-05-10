#ifndef _GLOBALS_H_
#define _GLOBALS_H_

extern int verbose;
extern int ticprt;

// 2nd derivative
// TODO: Programaticaly find the coefficients for any derivative in any order
extern const double cfd2nd_2th_order[2];
extern const double cfd2nd_4th_order[3];
extern const double cfd2nd_6th_order[4];
extern const double cfd2nd_8th_order[5];

#endif
