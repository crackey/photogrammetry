#ifndef LLS_H
#define LLS_H

#include "f2c.h"
#include "lapack.h"
#include "blas.h"

extern integer lls(integer m, integer n, doublereal* a, integer nrhs, doublereal* b, doublereal* s);
extern void matrixMultiply(doublereal* a, doublereal* b, doublereal* c, integer m, integer k, integer n);
extern void matrixInverse(doublereal* a, integer m);
extern void matrixATA(doublereal* a, doublereal* c, integer m, integer n);
extern void residual(double** r, double* a, double* x, double* l, int n, int m);
extern void solve(double** x, double* a, double* l, integer m, integer n);

#endif
