#include <cmath>

#include "lapack/dgesv.h"
#include "lapack/dgelsd.h"
#include "blas/dgemm.h"
#include "lls.h"

using namespace std;

int lls(integer m, integer n, doublereal* aa, integer nrhs, doublereal* b, doublereal* s)
{
    integer mmax = m, nmax = n;
    integer nlvl;
    nlvl = max(0, int(log(min(m, n)/26.0)+1));
    integer lda = mmax, liwork=3*mmax*nlvl+11*mmax, lwork;
    integer maxmn = max(m, n);
    lwork=(12*maxmn+50*maxmn+8*maxmn*nlvl+maxmn*nrhs+26*26)*2;
    liwork *= 2;
    lwork *= 2;
    doublereal rcond;
    integer info, rank;
    doublereal* a = new doublereal[lda*nmax];
    doublereal* work = new doublereal[lwork];
    integer* iwork = new integer[liwork];
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            a[i+lda*j] = aa[n*i+j];
    rcond = -1; // use the machine precision.

    dgelsd_(&m, &n, &nrhs, a, &lda, b, &maxmn, s, &rcond, &rank, work, &lwork, iwork, &info);
    delete []a;
    delete []work;
    delete []iwork;
    return 0;
}

void matrixMultiply(double* a, double* b, double* c, int m, int k, int n)
{
    char transa='t';
    char transb = 'n';
    integer mm = m, kk = k, nn = n;
    doublereal alpha = 1.0, beta = 0;
 //   DGEMM(TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC)

    dgemm_(&transa, &transb, &mm, &nn, 
        &kk, &alpha, a,
        &kk, b, &kk,
        &beta, c, &mm);    
}