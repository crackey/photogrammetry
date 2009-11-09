#include "lls.h"

#include <cmath>
#include <cstring>
#include <limits>

// #include "lapack/dgesv.h"
// #include "lapack/dgelsd.h"
// #include "clapack.h"
// #include "blas/dgemm.h"

#ifndef max
#define max(A, B) ((A)>(B) ? (A) : (B))
#endif
#ifndef min
#define min(A, B) ((A)>(B) ? (B) : (A)) 
#endif

using namespace std;

int lls(int m, int n, double* aa, int nrhs, double* b, double* s)
{
    int mmax = m, nmax = n;
    int nlvl;
    nlvl = max(0, int(log(min(m, n)/26.0)+1));
    int lda = mmax, liwork=3*mmax*nlvl+11*mmax, lwork;
    int maxmn = max(m, n);
    lwork=(12*maxmn+50*maxmn+8*maxmn*nlvl+maxmn*nrhs+26*26)*2;
    liwork *= 2;
    lwork *= 2;
    double rcond;
    int info, rank;
    double* a = new double[lda*nmax];
    double* work = new double[lwork];
    int* iwork = new int[liwork];
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
    int mm = m, kk = k, nn = n;
    double alpha = 1.0, beta = 0;
 //   DGEMM(TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC)

    dgemm(transa, transb, mm, nn, 
        kk, alpha, a,
        kk, b, kk,
        beta, c, mm);    
}

//void dgemm(char, char, int, int, int, double, double*, int, double*, int, double, double*, int)’


void matrixATA(double* a, double* c, int m, int n)
{
	double* b = new double[m*n];
	memcpy(b, a, sizeof(double)*m*n);
    char transa='n';
    char transb = 't';
    int mm = n, kk = m, nn = n;
    double alpha = 1.0, beta = 0;
 //   DGEMM(TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC)

    dgemm(transa, transb, mm, nn, 
        kk, alpha, b,
        mm, b, nn,
        beta, c, mm); 
    delete []b;
}

void matrixInverse(double* a, int m)
{
    int n = m;
	double* aa = new double[m*n];
	memcpy(aa, a, sizeof(double)*m*n);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            a[i+m*j] = aa[n*i+j];

    int* ipiv = new int[m];
	int iwork = m*n;
	double* work = new double[iwork];
	int info;
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

void solve(double** x, double* a, double* l, int m, int n)
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
