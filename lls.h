#ifndef LLS_H
#define LLS_H

#include "f2c.h"

extern int lls(integer m, integer n, doublereal* a, integer nrhs, doublereal* b, doublereal* s);
extern void matrixMultiply(double* a, double* b, double* c, int m, int k, int n);

#endif
