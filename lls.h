#ifndef LLS_H
#define LLS_H

//#include "la.h"
//#include "lapack.h"
//#include "blas.h"
#include "/usr/gfortran64/include/acml.h"
#include "/usr/gfortran64/include/acml_mv.h"

extern int lls(int m, int n, double* a, int nrhs, double* b, double* s);
extern void matrixMultiply(double* a, double* b, double* c, int m, int k, int n);
extern void matrixInverse(double* a, int m);
extern void matrixATA(double* a, double* c, int m, int n);
extern void residual(double** r, double* a, double* x, double* l, int n, int m);
extern void solve(double** x, double* a, double* l, int m, int n);

#endif
