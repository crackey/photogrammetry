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
        qDebug() << phtindex[i] << pht[i*4] << pht[i*4+1] << pht[i*4+2] << pht[i*4+3]
                 << points[i*3] << points[i*3+1] << points[i*3+2];
    }

    int maxit = 100;
    int itn = 0;
    int nc = nunknown*3 + 12; // number of columns of matrix a
    ofstream of;
    of.open("./debug.txt");
    double* a = new double[4*npht*nc]; // matrix A and B
    double* l = new double[4*npht];              // matrix l
    //double* s = new double[3*nunknown+12];
    double s[24] = {0.0};
    do
    {
        ++itn;
        prepare(a, l, pht, points, orient, idx, f, npht, nmatched);
#if 1
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
                of << phtindex[i] << ' ';
                of << points[i*3] << ' ' << points[i*3+1] << ' ';
                of << points[i*3+2] << ' ' << l[j] << ' ' << l[j+1] << ' ' << l[j+2] << endl;
                points[i*3] += l[j];
                points[i*3+1] += l[j+1];
                points[i*3+2] += l[j+2];
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

// setup matrix a and matrix l.
void Onestep::prepare(double* a, double* l, double* pht, double* ctl, double* o, int* idx, double f, int npht, int nmatched)
{
#define XL pht[4*i+0]
#define YL pht[4*i+1] 
#define XR pht[4*i+2]
#define YR pht[4*i+3]
    int nc = 12+(npht-nmatched)*3;
    memset(a, 0, sizeof(double)*4*npht*nc);
    int iup = 0;
    double* ol = o;
    double* or = o+6; 
    ofstream of;
    of.open("debug2.txt");
    of << endl;
    for (int i = 0; i < npht; ++i)
    {
        of  << idx[i] << ' ' << pht[4*i] << ' ' << pht[4*i+1] << ' '
            << pht[4*i+2] << ' ' << pht[4*i+3] << ' ' << ctl[3*i] << ' '
            << ctl[3*i+1] << ' ' << ctl[3*i+2] << endl;
    }
    of.close();
    for (int i = 0; i < npht; ++i)
    { 
        // matrix A
        // left
        a[i*nc*4+0] = (a1(ol)*f + a3(ol)*XL) / z_(ol, ctl+3*i);
        a[i*nc*4+1] = (b1(ol)*f + b3(ol)*XL) / z_(ol, ctl+3*i);
        a[i*nc*4+2] = (c1(ol)*f + c3(ol)*XL) / z_(ol, ctl+3*i);
        a[i*nc*4+3] = YL*sin(ol[4]) - (XL*(XL*cos(ol[5]) - YL*sin(ol[5]))/f
                                       + f*cos(ol[5])
                                      ) * cos(ol[4]);
        a[i*nc*4+4] = -f*sin(ol[5]) - XL*(XL*sin(ol[5])+YL*cos(ol[5]))/f;
        a[i*nc*4+5] = YL;

        a[i*nc*4+nc+0] = (a2(ol)*f + a3(ol)*YL) / z_(ol, ctl+3*i);
        a[i*nc*4+nc+1] = (b2(ol)*f + b3(ol)*YL) / z_(ol, ctl+3*i);
        a[i*nc*4+nc+2] = (c2(ol)*f + c3(ol)*YL) / z_(ol, ctl+3*i);
        a[i*nc*4+nc+3] = -XL*sin(ol[4]) - (YL*(XL*cos(ol[5])-YL*sin(ol[5]))/f
                                           -f*sin(ol[5])
                                          )*cos(ol[4]);
        a[i*nc*4+nc+4] = -f*cos(ol[5]) - YL*(XL*sin(ol[5])+YL*cos(ol[5]))/f;
        a[i*nc*4+nc+5] = -XL;

        // right 
        a[i*nc*4+nc*2+6] = (a1(or)*f + a3(or)*XR) / z_(or, ctl+3*i);
        a[i*nc*4+nc*2+7] = (b1(or)*f + b3(or)*XR) / z_(or, ctl+3*i);
        a[i*nc*4+nc*2+8] = (c1(or)*f + c3(or)*XR) / z_(or, ctl+3*i);
        a[i*nc*4+nc*2+9] = YR*sin(or[4]) - (XR*(XR*cos(or[5]) - YR*sin(or[5]))/f
                                       + f*cos(or[5])
                                      ) * cos(or[4]);
        a[i*nc*4+nc*2+10] = -f*sin(or[5]) - XR*(XR*sin(or[5])+YR*cos(or[5]))/f;
        a[i*nc*4+nc*2+11] = YR;

        a[i*nc*4+nc*3+6] = (a2(or)*f + a3(or)*YR) / z_(or, ctl+3*i);
        a[i*nc*4+nc*3+7] = (b2(or)*f + b3(or)*YR) / z_(or, ctl+3*i);
        a[i*nc*4+nc*3+8] = (c2(or)*f + c3(or)*YR) / z_(or, ctl+3*i);
        a[i*nc*4+nc*3+9] = -XR*sin(or[4]) - (YR*(XR*cos(or[5])-YR*sin(or[5]))/f
                                           -f*sin(or[5])
                                          )*cos(or[4]);
        a[i*nc*4+nc*3+10] = -f*cos(or[5]) - YR*(XR*sin(or[5])+YR*cos(or[5]))/f;
        a[i*nc*4+nc*3+11] = -XR;

        // matrix B. also stored in a
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

        l[i*4] = XL + f * (a1(ol)*(ctl[3*i]-ol[0]) + 
                           b1(ol)*(ctl[3*i+1]-ol[1]) + 
                           c1(ol)*(ctl[3*i+2]-ol[2])
                          ) / z_(o, ctl+3*i);
        l[i*4+1] = YL + f * (a2(ol)*(ctl[3*i]-ol[0]) + 
                             b2(ol)*(ctl[3*i+1]-ol[1]) + 
                             c2(ol)*(ctl[3*i+2]-ol[2])
                            ) / z_(ol, ctl+3*i);
        l[i*4+2] = XR + f * (a1(or)*(ctl[3*i]-or[0]) + 
                           b1(or)*(ctl[3*i+1]-or[1]) + 
                           c1(or)*(ctl[3*i+2]-or[2])
                          ) / z_(o, ctl+3*i);
        l[i*4+3] = YR + f * (a2(or)*(ctl[3*i]-or[0]) + 
                             b2(or)*(ctl[3*i+1]-or[1]) + 
                             c2(or)*(ctl[3*i+2]-or[2])
                            ) / z_(or, ctl+3*i);
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
