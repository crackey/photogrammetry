#include <vector>
#include <map>
#include <cmath>

#include <iostream>
#include <fstream>

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
}

Onestep::~Onestep()
{

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
    int nctl;                // number of control points
    nctl = pctl->data(&ctl, &ctlindex);

    //memset(idx, 0, sizeof(int)*npht);
    int i, j;
    int nmatched = 0; // number of matched points
    // the keys was guaranteed in increasing order
    int* idx = new int[npht];      // matched control point indexes. -1 for unknow points.
    for (i = 0; i < npht; idx[i++] = -1)
        ;
    for (i = 0, j = 0; i<npht && j<nctl; )
    {
        if (phtindex[i] == ctlindex[j])
        {
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

    double* a = new double[4*npht*(12+3*nunknown)]; // matrix A and B
    double* l = new double[4*npht];              // matrix l
    double* s = new double[3*nunknown+12];
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
    for (int i = 0; i < npht; ++i)
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
    orient[6] = orient[0];
    orient[7] = orient[1];
    orient[8] = orient[2];
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
        qDebug() << points[i*3] << points[i*3+1] << points[i*3+2];
    }

    int maxit = 300;
    int itn = 0;
    int nc = nunknown*3 + 12; // number of columns of matrix a
    ofstream of;
    of.open("./debug.txt");
    do
    {
        ++itn;
        memset(a, 0, sizeof(double)*4*npht*nc);
        memset(l, 0, sizeof(double)*4*npht);
        int iup = 0;
        // set matrix a and l
        for (i = 0; i < npht; ++i)
        {
            ma(a+nc*i*4, pht+4*i, points+3*i, orient, nc, idx[i], iup, f, npht, nmatched, i);
            if (idx[i] == -1)
                ++iup;

            l[i*4] = pht[4*i] + f * (a1(orient)*(points[3*i]-orient[0]) + 
                                     b1(orient)*(points[3*i+1]-orient[1]) + 
                                     c1(orient)*(points[3*i+2]-orient[2])
                                    ) / z_(orient, points+3*i);
            l[i*4+1] = pht[4*i+1] + f * (a2(orient)*(points[3*i]-orient[0]) + 
                                         b2(orient)*(points[3*i+1]-orient[1]) + 
                                         c2(orient)*(points[3*i+2]-orient[2])
                                        ) / z_(orient, points+3*i);
            l[i*4+2] = pht[4*i+2] + f * (a1(orient+6)*(points[3*i]-orient[6]) + 
                                         b1(orient+6)*(points[3*i+1]-orient[7]) + 
                                         c1(orient+6)*(points[3*i+2]-orient[8])
                                        ) / z_(orient+6, points+3*i);
            l[i*4+3] = pht[4*i+3] + f * (a2(orient+6)*(points[3*i]-orient[6]) + 
                                         b2(orient+6)*(points[3*i+1]-orient[7]) + 
                                         c2(orient+6)*(points[3*i+2]-orient[8])
                                        ) / z_(orient+6, points+3*i);
        }
#if 0
        of << "Matrix a:" << endl;
        for (int ii = 0; ii < 4*npht; ++ii)
        {
            for (int jj = 0; jj < nc; ++jj)
                of << a[ii*nc+jj] << ' ';
            of << endl;
        }
        of << endl << "Matrix l:" ;
        for (int ii = 0; ii < 4*npht; ++ii)
            of << l[ii] << ' ';
        of << endl;
#endif
        lls(4*npht, nc, a, 1, l, s);
        for (i = 0; i < 12; ++i)
            orient[i] += l[i];
        for (i = 0, j = 12; i < npht; ++i)
        {
            if (idx[i] == -1)
            {
                points[i*3] += l[j];
                points[i*3+1] += l[j+1];
                points[i*3+2] += l[j+2];
                of << phtindex[i] << ' ';
                of << points[i*3] << ' ' << points[i*3+1] << ' ';
                of << points[i*3+2] << ' ' << l[j] << ' ' << l[j+1] << ' ' << l[j+2] << endl;
                j += 3;
            }
        }
#if 0
        of << "l:" << endl;
        for (i = 0; i < 12; ++i)
            of << l[i] << ' ';
        of << endl << endl;
#endif 
    } while(!exact(l,nc) && itn<maxit);
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
}

double Onestep::z_(double* orient, double* ctl)
{
    return a3(orient)*(ctl[0] - orient[0])
        + b3(orient)*(ctl[1] - orient[1])
        + c3(orient)*(ctl[2] - orient[2]);
}

// setup matrix a.
void Onestep::ma(double* a, double* pht, double* ctl, double* o, int nc, int ictl, int iup, double f, int npht, int nmatched, int i)
{
#define XL pht[0]
#define YL pht[1] 
#define XR pht[2]
#define YR pht[3]
#if 0
    a[0] = (a1(orient)*f + a3(orient)*pht[index*2]) / z_(orient, ctl, index);
    a[1] = (b1(orient)*f + b3(orient)*pht[index*2]) / z_(orient, ctl, index);
    a[2] = (c1(orient)*f + c3(orient)*pht[index*2]) / z_(orient, ctl, index);
    a[3] = pht[index*2+1]*sin(orient[4]) - 
        (pht[index*2]*(pht[index*2]*cos(orient[5])-pht[index*2+1]*sin(orient[5]))/f
         + f*cos(orient[5])) * cos(orient[4]);
    a[4] = -f*sin(orient[5]) - 
        pht[index*2] * (pht[index*2]*sin(orient[5]) + pht[index*2+1]*cos(orient[5])) / f;
    a[5] = pht[index*2+1];
    a[6] = (a2(orient)*f + a3(orient)*pht[index*2+1]) / z_(orient, ctl, index);
    a[7] = (b2(orient)*f + b3(orient)*pht[index*2+1]) / z_(orient, ctl, index);
    a[8] = (c2(orient)*f + c3(orient)*pht[index*2+1]) / z_(orient, ctl, index);
    a[9] = -pht[index*2]*sin(orient[4]) - (pht[index*2+1]*(pht[index*2]*cos(orient[5])
                                                           -pht[index*2+1]*sin(orient[5]))/f 
                                           -f*sin(orient[5]))*cos(orient[4]);
    a[10] = -f*cos(orient[5]) - pht[index*2+1] * (pht[index*2]*sin(orient[5]) + pht[index*2+1]*cos(orient[5])) / f;
    a[11] = -pht[index*2];
#endif
    // matrix A
    // left
    a[0] = (a1(o)*f + a3(o)*XL) / z_(o, ctl);
    a[1] = (b1(o)*f + b3(o)*XL) / z_(o, ctl);
    a[2] = (c1(o)*f + c3(o)*XL) / z_(o, ctl);
    a[3] = YL*sin(o[4]) - 
        (XL*(XL*cos(o[5])-YL*sin(o[5]))/f
         + f*cos(o[5])) * cos(o[4]);
    a[4] = -f*sin(o[5]) 
        - XL * (XL*sin(o[5]) + YL*cos(o[5])) / f;
    a[5] = YL;

    a[nc] = (a2(o)*f + a3(o)*YL) / z_(o, ctl);
    a[nc+1] = (b2(o)*f + b3(o)*YL) / z_(o, ctl);
    a[nc+2] = (c2(o)*f + c3(o)*YL) / z_(o, ctl);
    a[nc+3] = -XL*sin(o[4]) - (YL*(XL*cos(o[5])
                                   -YL*sin(o[5]))/f 
                               -f*sin(o[5]))*cos(o[4]);
    a[nc+4] = -f*cos(o[5]) - YL*(XL*sin(o[5]) + YL*cos(o[5])) / f;
    a[nc+5] = -XL;

    // right 
    a[2*nc+6] = (a1(o+6)*f + a3(o+6)*XR) / z_(o+6, ctl);
    a[2*nc+7] = (b1(o+6)*f + b3(o+6)*XR) / z_(o+6, ctl);
    a[2*nc+8] = (c1(o+6)*f + c3(o+6)*XR) / z_(o+6, ctl);
    a[2*nc+9] = YR*sin(o[10]) - 
        (XR*(XR*cos(o[11])-YR*sin(o[11]))/f
         + f*cos(o[11])) * cos(o[10]);
    a[2*nc+10] = -f*sin(o[11]) - 
        XR * (XR*sin(o[11]) + YR*cos(o[11])) / f;
    a[2*nc+11] = YR;

    a[3*nc+6] = (a2(o+6)*f + a3(o+6)*XR) / z_(o+6, ctl);
    a[3*nc+7] = (b2(o+6)*f + b3(o+6)*XR) / z_(o+6, ctl);
    a[3*nc+8] = (c2(o+6)*f + c3(o+6)*XR) / z_(o+6, ctl);
    a[3*nc+9] = -XR*sin(o[10]) - (YR*(XR*cos(o[11])
                                      -YR*sin(o[11]))/f 
                                  -f*sin(o[11]))*cos(o[10]);
    a[3*nc+10] = -f*cos(o[11]) - YR*(XR*sin(o[11]) + YR*cos(o[11])) / f;
    a[3*nc+11] = -XR;

    // matrix B. also stored in a
    if (ictl == -1)
    {
        // left
        a[12+iup*3] = -((a1(o)*f + a3(o)*XL) / z_(o, ctl)); // -a11
        a[12+iup*3+1] = -((b1(o)*f + b3(o)*XL) / z_(o, ctl)); // -a12
        a[12+iup*3+2] = -((c1(o)*f + c3(o)*XL) / z_(o, ctl)); // -a13

        a[nc+12+iup*3] = -((a2(o)*f + a3(o)*YL) / z_(o, ctl));
        a[nc+12+iup*3+1] = -((b2(o)*f + b3(o)*YL) / z_(o, ctl));
        a[nc+12+iup*3+2] = -((c2(o)*f + c3(o)*YL) / z_(o, ctl));

        // right
        a[nc*2+12+iup*3] = -((a1(o+6)*f + a3(o+6)*XR) / z_(o+6, ctl)); // -a11
        a[nc*2+12+iup*3+1] = -((b1(o+6)*f + b3(o+6)*XR) / z_(o+6, ctl)); // -a12
        a[nc*2+12+iup*3+2] = -((c1(o+6)*f + c3(o+6)*XR) / z_(o+6, ctl)); // -a13

        a[nc*3+12+iup*3] = -((a2(o+6)*f + a3(o+6)*XR) / z_(o+6, ctl));
        a[nc*3+12+iup*3+1] = -((b2(o+6)*f + b3(o+6)*XR) / z_(o+6, ctl));
        a[nc*3+12+iup*3+2] = -((c2(o+6)*f + c3(o+6)*XR) / z_(o+6, ctl));
    }
}


bool Onestep::exact(double* x, int n)
{
    double lenlim = 1e-2; // length limits
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
