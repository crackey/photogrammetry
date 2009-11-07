#include "lls.h"

#include <cmath>
#include <cstring>
#include <limits>

// #include "lapack/dgesv.h"
// #include "lapack/dgelsd.h"
// #include "clapack.h"
// #include "blas/dgemm.h"

#define max(A, B) ((A)>(B) ? (A) : (B))
#define min(A, B) ((A)>(B) ? (B) : (A)) 

using namespace std;

integer lls(integer m, integer n, doublereal* aa, integer nrhs, doublereal* b, doublereal* s)
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

void matrixMultiply(double* a, double* b, double* c, integer m, integer k, integer n)
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

void matrixATA(doublereal* a, doublereal* c, integer m, integer n)
{
	doublereal* b = new doublereal[m*n];
	memcpy(b, a, sizeof(doublereal)*m*n);
    char transa='n';
    char transb = 't';
    integer mm = n, kk = m, nn = n;
    doublereal alpha = 1.0, beta = 0;
 //   DGEMM(TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC)

    dgemm_(&transa, &transb, &mm, &nn, 
        &kk, &alpha, b,
        &mm, b, &nn,
        &beta, c, &mm); 
    delete []b;
}

void matrixInverse(doublereal* a, integer m)
{
    integer n = m;
	doublereal* aa = new doublereal[m*n];
	memcpy(aa, a, sizeof(doublereal)*m*n);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            a[i+m*j] = aa[n*i+j];

    integer* ipiv = new integer[m];
	integer iwork = m*n;
	doublereal* work = new doublereal[iwork];
	integer info;
	dgetrf_(&m, &n, a, &m, ipiv, &info);
	dgetri_(&m, a, &n, ipiv, work, &iwork, &info);

	delete []aa;
	delete []ipiv;
}

// calculate the residuals
void residual(double** r, double* a, double* x, double* l, int n, int m)
{
    if (n <= m)
        return;
	double* V = new double[n];
	matrixMultiply(a, x, V, n, m, 1);  // V = a*x
	for (int i = 0; i < n; ++i)  // V = a*x - l
	{
		V[i] -= l[i];
	}
	double VV = 0;
	for (int i = 0; i < n; ++i)
		VV += V[i]*V[i];
	double m0 = sqrt(VV/(n-m));
	double* Q = new double[m*m];
    matrixATA(a, Q, n, m);
	matrixInverse(Q, m);   
    /*
    double* Qr = new double[m*m];
    memcpy(Qr, Q, sizeof(double)*m*m);

    double* QQr = new double[m*m];
    matrixMultiply(Q, Qr, QQr, m, m, m);
    */
	for (int i = 0; i < m; ++i)
	{
		(*r)[i] = m0 * sqrt(Q[i*m+i]);
	}
    delete []V;
    delete []Q;
}

void solve(double** x, double* a, double* l, integer m, integer n)
{
    double* ATA = new double[n*n];
    matrixATA(a, ATA, m, n);
    matrixInverse(ATA, n);
    double* AT = new double[m*n];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            AT[m*i+j] = a[i+n*j];

    double* ATL = new double[m*n]; 
    matrixMultiply(AT, l, ATL, n, m, 1);
    matrixMultiply(ATA, ATL, *x, n, n, 1);
    double wx[24];
    memcpy(wx, *x, 24*sizeof(double));
    delete []ATA;
    delete []AT;
    delete []ATL;
}
