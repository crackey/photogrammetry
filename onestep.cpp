#include <vector>
#include <map>
#include <cmath>

#include "controlpoints.h"
#include "photopoints.h"
#include "transform.h"
#include "phgproject.h"
#include "lls.h"
#include "onestep.h"

using namespace std;

Onestep::Onestep(QString ctl, QString pht, QObject* parent)
: QObject(parent)
{
    m_ctl = ctl;
    m_pht = pht;
    m_result = 0;
    m_index = 0;
    m_residual = 0;
    m_isctl = 0;
}

Onestep::~Onestep()
{
    if (m_result != 0)
        delete []m_result;
    if (m_index != 0)
        delete []m_index;
    if (m_residual != 0)
        delete []m_residual;
    if (m_isctl = 0)
        delete []m_isctl;
}
int Onestep::orient(double** o, double** os)
{
    *o = m_orient;
    *os = m_residual;
    return 12;
}

int Onestep::result(int** index, int** isctl, double** r, double** rs)
{
    *isctl = m_isctl;
    *r = m_result;
    *rs = m_residual+12;
    *index = m_index;
    return m_pnum;
}

bool Onestep::calculate()
{
    ControlPoints* pctl = static_cast<PHGProject*>(parent())->controlPoints(m_ctl);
    PhotoPoints* ppht = static_cast<PHGProject*>(parent())->photoPoints(m_pht);
    double* pht = 0;
    int* phtindex = 0;
    double* ctl = 0;
    int* ctlindex = 0;
    double f;    // focus
    int npht;               // total number of points
    npht = ppht->data(3, &f, &pht, &phtindex);
    m_pnum = npht;
    int nctl;                // number of control points
    nctl = pctl->data(&ctl, &ctlindex);
    
    m_index = phtindex;
    //memset(idx, 0, sizeof(int)*npht);
    int i, j;
    int nmatched = 0; // number of matched points
    // the keys was guaranteed in increasing roder
    int* idx = new int[npht];      // matched control point indexes. -1 for unknow points.
    m_isctl = new int[npht];
    memset(m_isctl, 0, sizeof(int)*npht);
    for (i = 0; i < npht; idx[i++] = -1)
        ;
    for (i = 0, j = 0; i<npht && j<nctl; )
    {
        if (phtindex[i] == ctlindex[j])
        {
            m_isctl[i] = 1;
            idx[i] = j;
            ++nmatched;
            ++i;
            ++j;
        }
        else if (phtindex[i] > ctlindex[j])
        {
            ++j;
        }
        else
        {
            ++i;
        }
    }
    int nunknown = npht - nmatched;   // number of unknow points

    double* orient = m_orient;
    m_result = new double[3*npht];
    double* points = m_result;

#if 0
    double ip[3];  // initial values for unknown points
    // use the first control points as the initial value of unkonwn points.
    i = 0;
    while( idx[i] == -1)
    {
        ++i;
    }
    ip[0] = ctl[idx[i]*3+1] * 1000;
    ip[1] = ctl[idx[i]*3] * 1000;
    ip[2] = ctl[idx[i]*3+2] * 1000;
#endif

    int si[2];
    for (i = 0, j = 0; i<npht && j<2; ++i)
    {
        if (idx[i] != -1)
            si[j++] = i;
    }
    double phtscale;
    double scp[4];
    scp[0] = ctl[idx[si[0]]*3+1] * 1000;
    scp[1] = ctl[idx[si[0]]*3] * 1000;
    scp[2] = ctl[idx[si[1]]*3+1] * 1000;
    scp[3] = ctl[idx[si[1]]*3] * 1000;
    double spp[4];
    spp[0] = pht[si[0]*4];
    spp[1] = pht[si[0]*4+1];
    spp[2] = pht[si[1]*4];
    spp[3] = pht[si[1]*4+1];
    phtscale = sqrt((scp[0]-scp[2])*(scp[0]-scp[2]) + (scp[1]-scp[3])*(scp[1]-scp[3]))
        / sqrt((spp[0]-spp[2])*(spp[0]-spp[2]) + (spp[1]-spp[3])*(spp[1]-spp[3]));

    memset(orient, 0, sizeof(double)*12);
    for (i = 0; i < npht; ++i)
    {
        if (idx[i] != -1)
        {
            orient[0] += ctl[idx[i]*3+1]*1000;
            orient[1] += ctl[idx[i]*3]*1000;
            orient[2] += ctl[idx[i]*3+2]*1000;
        }
    }
    orient[0] /= nmatched;
    orient[1] /= nmatched;
    orient[2] = orient[2]/nmatched + phtscale*f;
    memcpy(orient+6, orient, 6*sizeof(double));
    // initialize points values
    for (i = 0; i < npht; ++i)
    {
        if (idx[i] != -1)
        {
            points[i*3] = ctl[idx[i]*3+1] * 1000;
            points[i*3+1] = ctl[idx[i]*3] * 1000;
            points[i*3+2] = ctl[idx[i]*3+2] * 1000;
        }
        else
        {
            points[i*3] = m_orient[0];
            points[i*3+1] = m_orient[1];
            points[i*3+2] = m_orient[2] - phtscale*f;
        }
    }
    qDebug() << "Onestep initial values";
    for (i = 0; i < npht; ++i)
    {
        qDebug() << phtindex[i] << pht[i*4] << pht[i*4+1] << pht[i*4+2] << pht[i*4+3]
                 << points[i*3] << points[i*3+1] << points[i*3+2];
    }

    int maxit = 1; // the lapack dgelsd routine gives a best answer, so one iteration is enough.
    int itn = 0;
    int nc = nunknown*3 + 12; // number of cloumns of matrix a
    double* a = new double[4*npht*nc]; // matrix A and B
    double* l = new double[4*npht];              // matrix l
    //double* s = new double[3*nunknown+12];
    double* x = new double[4*npht];
    double s[24] = {0.0};
    do
    {
        ++itn;
        prepare(a, l, pht, points, orient, idx, f, npht, nmatched);
        memcpy(x, l, sizeof(double)*4*npht);
        lls(4*npht, nc, a, 1, x, s);
        for (i = 0; i < 12; ++i)
            orient[i] += x[i];
        for (i = 0, j = 12; i < npht; ++i)
        {
            if (idx[i] == -1)
            { 
                points[i*3] += x[j];
                points[i*3+1] += x[j+1];
                points[i*3+2] += x[j+2];
                j += 3;
            }
        }
        qDebug() << "Points";
        for (i = 0; i < npht; ++i)
        {
            qDebug() << points[i*3] << points[i*3+1] << points[i*3+2];
        }
    } while(!exact(l,nc) && itn<maxit);
            
    m_residual = new double[nc];
    residual(&m_residual, a, x, l, 4*npht, nc);

    qDebug() << "onestep itertations: " << itn;
    qDebug() << "Orient Elements:";
    qDebug() << "Left:" << m_orient[0] << m_orient[1] << m_orient[2] << m_orient[3] << m_orient[4] << m_orient[5];
    qDebug() << "Right:" << m_orient[6] << m_orient[7] << m_orient[8] << m_orient[9] << m_orient[10] << m_orient[11]; 
    qDebug() << "Onestep results: ";
    for (i = 0; i < npht; ++i)
    {
        qDebug() << m_result[i*3+1] / 1000 << m_result[i*3] / 1000 << m_result[i*3+2] / 1000;
    }
    return true;
    delete []a;
    delete []l;
    delete []idx;
    delete []ctl;
    delete []pht;
    delete []ctlindex;
}

double Onestep::z_(double* orient, double* ctl)
{
    return a3(orient)*(ctl[0] - orient[0])
        + b3(orient)*(ctl[1] - orient[1])
        + c3(orient)*(ctl[2] - orient[2]);
}

// setup matrix a and matrix l.
void Onestep::prepare(double* a, double* l, double* pht, double* ctl, double* o, int* idx, double f, int npht, int nmatched)
{
#define XL pht[4*i+0]
#define YL pht[4*i+1] 
#define XR pht[4*i+2]
#define YR pht[4*i+3]

    int nc = 12+(npht-nmatched)*3;
    memset(a, 0, sizeof(double)*4*npht*nc);
    double* lo = o;
    double* ro = o+6; 
    int iup = 0;
    for (int i = 0; i < npht; ++i)
    { 
        // matrix A
        // left
        a[i*nc*4+0] = (a1(lo)*f + a3(lo)*XL) / z_(lo, ctl+3*i);
        a[i*nc*4+1] = (b1(lo)*f + b3(lo)*XL) / z_(lo, ctl+3*i);
        a[i*nc*4+2] = (c1(lo)*f + c3(lo)*XL) / z_(lo, ctl+3*i);
        a[i*nc*4+3] = YL*sin(lo[4]) - (XL*(XL*cos(lo[5]) - YL*sin(lo[5]))/f
                                       + f*cos(lo[5])
                                      ) * cos(lo[4]);
        a[i*nc*4+4] = -f*sin(lo[5]) - XL*(XL*sin(lo[5])+YL*cos(lo[5]))/f;
        a[i*nc*4+5] = YL;

        a[i*nc*4+nc+0] = (a2(lo)*f + a3(lo)*YL) / z_(lo, ctl+3*i);
        a[i*nc*4+nc+1] = (b2(lo)*f + b3(lo)*YL) / z_(lo, ctl+3*i);
        a[i*nc*4+nc+2] = (c2(lo)*f + c3(lo)*YL) / z_(lo, ctl+3*i);
        a[i*nc*4+nc+3] = -XL*sin(lo[4]) - (YL*(XL*cos(lo[5])-YL*sin(lo[5]))/f
                                           -f*sin(lo[5])
                                          )*cos(lo[4]);
        a[i*nc*4+nc+4] = -f*cos(lo[5]) - YL*(XL*sin(lo[5])+YL*cos(lo[5]))/f;
        a[i*nc*4+nc+5] = -XL;

        // right 
        a[i*nc*4+nc*2+6] = (a1(ro)*f + a3(ro)*XR) / z_(ro, ctl+3*i);
        a[i*nc*4+nc*2+7] = (b1(ro)*f + b3(ro)*XR) / z_(ro, ctl+3*i);
        a[i*nc*4+nc*2+8] = (c1(ro)*f + c3(ro)*XR) / z_(ro, ctl+3*i);
        a[i*nc*4+nc*2+9] = YR*sin(ro[4]) - (XR*(XR*cos(ro[5]) - YR*sin(ro[5]))/f
                                       + f*cos(ro[5])
                                      ) * cos(ro[4]);
        a[i*nc*4+nc*2+10] = -f*sin(ro[5]) - XR*(XR*sin(ro[5])+YR*cos(ro[5]))/f;
        a[i*nc*4+nc*2+11] = YR;

        a[i*nc*4+nc*3+6] = (a2(ro)*f + a3(ro)*YR) / z_(ro, ctl+3*i);
        a[i*nc*4+nc*3+7] = (b2(ro)*f + b3(ro)*YR) / z_(ro, ctl+3*i);
        a[i*nc*4+nc*3+8] = (c2(ro)*f + c3(ro)*YR) / z_(ro, ctl+3*i);
        a[i*nc*4+nc*3+9] = -XR*sin(ro[4]) - (YR*(XR*cos(ro[5])-YR*sin(ro[5]))/f
                                           -f*sin(ro[5])
                                          )*cos(ro[4]);
        a[i*nc*4+nc*3+10] = -f*cos(ro[5]) - YR*(XR*sin(ro[5])+YR*cos(ro[5]))/f;
        a[i*nc*4+nc*3+11] = -XR;

        // matrix B. also stroed in a
        if (idx[i] == -1)       // unknown points
        {
            // left
            a[i*nc*4+iup*3+12] = -a[i*nc*4+0]; // -a11
            a[i*nc*4+iup*3+13] = -a[i*nc*4+1]; // -a12
            a[i*nc*4+iup*3+14] = -a[i*nc*4+2]; // -a13

            a[i*nc*4+nc+iup*3+12] = -a[i*nc*4+nc+0];   // -a21
            a[i*nc*4+nc+iup*3+13] = -a[i*nc*4+nc+1]; // -a22
            a[i*nc*4+nc+iup*3+14] = -a[i*nc*4+nc+2]; // -a23

            // right
            a[i*nc*4+nc*2+iup*3+12] = -a[i*nc*4+nc*2+6]; // -a11
            a[i*nc*4+nc*2+iup*3+13] = -a[i*nc*4+nc*2+7]; // -a12
            a[i*nc*4+nc*2+iup*3+14] = -a[i*nc*4+nc*2+8]; // -a13

            a[i*nc*4+nc*3+iup*3+12] = -a[i*nc*4+nc*3+6];   // -a21
            a[i*nc*4+nc*3+iup*3+13] = -a[i*nc*4+nc*3+7]; // -a22
            a[i*nc*4+nc*3+iup*3+14] = -a[i*nc*4+nc*3+8]; // -a23    
            ++iup;
        }

        l[i*4] = XL + f * (a1(lo)*(ctl[3*i]-lo[0]) + 
                           b1(lo)*(ctl[3*i+1]-lo[1]) + 
                           c1(lo)*(ctl[3*i+2]-lo[2])
                          ) / z_(o, ctl+3*i);
        l[i*4+1] = YL + f * (a2(lo)*(ctl[3*i]-lo[0]) + 
                             b2(lo)*(ctl[3*i+1]-lo[1]) + 
                             c2(lo)*(ctl[3*i+2]-lo[2])
                            ) / z_(lo, ctl+3*i);
        l[i*4+2] = XR + f * (a1(ro)*(ctl[3*i]-ro[0]) + 
                           b1(ro)*(ctl[3*i+1]-ro[1]) + 
                           c1(ro)*(ctl[3*i+2]-ro[2])
                          ) / z_(o, ctl+3*i);
        l[i*4+3] = YR + f * (a2(ro)*(ctl[3*i]-ro[0]) + 
                             b2(ro)*(ctl[3*i+1]-ro[1]) + 
                             c2(ro)*(ctl[3*i+2]-ro[2])
                            ) / z_(ro, ctl+3*i);
   }
}


bool Onestep::exact(double* x, int n)
{
    double lenlim = 10; // length limits
    double anglim = 3e-5; // angle limits
    int i;
    for (i = 0; i < 3; ++i)
    {
        if (fabs(x[i]) > lenlim)
            return false;
    }
    for (i = 3; i < 6; ++i)
    {
        if (fabs(x[i]) > anglim)
            return false;
    }
    for (i = 6; i < 9; ++i)
    {
        if (fabs(x[i]) > lenlim)
            return false;
    }
    for (i = 9; i < 12; ++i)
    {
        if (fabs(x[i]) > anglim)
            return false;
    }
    for (i = 12; i < n; ++i)
    {
        if (fabs(x[i]) > lenlim)
            return false;
    }
    return true;
}
