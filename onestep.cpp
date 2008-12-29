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
    for (int i = 0; i < 12; ++i)
        m_orient.push_back(0);
}

Onestep::~Onestep()
{
}
int Onestep::orient(vector<double>* o, vector<double>* os)
{
    *o = m_orient;
    *os = m_orient_residual;
    return 12;
}

int Onestep::result(map<int, Point>* result, map<int, Point>* residual)
{
    *result = m_result;
    *residual = m_result_residual;
    return m_numPoint;
}

bool Onestep::calculate()
{
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);
    ControlPoints* tctl = prj->controlPoints(m_ctl);
    double f;    // focus
    map<int, PhotoPoint> pht;
    map<int, Point> ctl;
    map<int, int> match;    // matched photo point and control point keys
    int npht = tpht->data(&f, &pht);
    int nctl = tctl->data(&ctl);

    map<int, PhotoPoint>::const_iterator itp;
    map<int, Point>::const_iterator itc;
    int nmatch = 0;
    for (itp = pht.begin(), itc = ctl.begin();
         itp!= pht.end() && itc != ctl.end();
         )
    {
        if (itp->first == itc->first)
        {
            match.insert(make_pair(itp->first, itc->first));
            ++nmatch;
            ++itc;
            ++itp;
        }
        else if (itp->first > itc->first)
        {
            ++itc;
        }
        else
        {
            match.insert(make_pair(itp->first, -1));         // unmatched
            ++itp;
        }
    }

    double phtscale = 1e4; // Photo scale;
    map<int, int>::const_iterator itm;
    for (itm = match.begin(); itm != match.end(); ++itm)
    {
        if (itm->second != -1)
        {
            m_orient[0] += ctl[itm->second].x;
            m_orient[1] += ctl[itm->second].y;
            m_orient[2] += ctl[itm->second].z;
        }
    }
    m_orient[0] /= nmatch;
    m_orient[1] /= nmatch;
    m_orient[2] = m_orient[2]/nmatch + phtscale*f;
    m_orient[6] = m_orient[0];
    m_orient[7] = m_orient[1];
    m_orient[8] = m_orient[2];

    for (itm = match.begin(); itm != match.end(); ++itm)
    {
        if (itm->second != -1)
        {
            m_result.insert(make_pair(itm->first, ctl[itm->second]));
        }
        else
        {
            Point p;
            p.x = m_orient[0];
            p.y = m_orient[1];
            p.z = m_orient[2] - phtscale*f;
            m_result.insert(make_pair(itm->first, p));
        }
    }
    
    int maxit = 30; // the lapack dgelsd routine gives a best answer, so one iteration is enough.
    int itn = 0;
    int nc = (npht-nmatch)*3 + 12; // number of cloumns of matrix a
    double* a = new double[4*npht*nc]; // matrix A and B
    double* l = new double[4*npht];              // matrix l
    double* x = new double[4*npht];
    double* s = new double[nc];
    ofstream of;
    of.open("debug.txt");
    do
    {
        ++itn;
        prepare(a, l, m_result, pht, f, match, nmatch);
        for (int i = 0; i < npht*4; ++i)
        {
            for (int j = 0; j < nc; ++j)
                of << a[i*nc+j] << ' ';
            of << endl;
        }
        of << endl;
        memcpy(x, l, sizeof(double)*4*npht);
        lls(4*npht, nc, a, 1, x, s);
        for (int i = 0; i < 12; ++i)
            m_orient[i] += x[i];
        int j;
        for (itm = match.begin(), j = 12; 
             itm != match.end(); 
             ++itm)
        {
            if (itm->second == -1)
            { 
                m_result[itm->first].x += x[j];
                m_result[itm->first].y += x[j+1];
                m_result[itm->first].z += x[j+2];
                j += 3;
            }
        }
    } while(!exact(x,nc) && itn<maxit);
    m_numPoint = npht;
    double* r = new double[nc];
    residual(&r, a, x, l, 4*npht, nc);
    for (int i = 0; i < 12; ++i)
        m_orient_residual.push_back(r[i]);
    int i;
    for (itm = match.begin(), i = 12; itm != match.end(); ++itm)
    {
        Point p;
        if (itm->second == -1)
        {
            p.x = r[i++];
            p.y = r[i++];
            p.z = r[i++];
        }
        else
        {
            p.x = 0;
            p.y = 0;
            p.z = 0;
        }
        m_result_residual.insert(make_pair(itm->first, p));
    }

    delete []a;
    delete []l;
    delete []x;
    delete []s;
    return true;
}


void Onestep::prepare(double* a, double* l, map<int, Point>& ctl, 
                           map<int, PhotoPoint>& pht, double f, 
                           map<int, int>& match, int nmatch)
{
    int nc = 12 + (pht.size()-nmatch)*3;
    double Rl[9];
    double Xsl, Ysl, Zsl, phil, omegal, kappal;
    map<int, int>::const_iterator itm;
    int i;    
    Xsl = m_orient[0];
    Ysl = m_orient[1];
    Zsl = m_orient[2];
    phil = m_orient[3];
    omegal = m_orient[4];
    kappal = m_orient[5];
    transform(Rl, phil, omegal, kappal);

    double Rr[9];
    double Xsr, Ysr, Zsr, phir, omegar, kappar;
    Xsr = m_orient[6];
    Ysr = m_orient[7];
    Zsr = m_orient[8];
    phir = m_orient[9];
    omegar = m_orient[10];
    kappar = m_orient[11];
    transform(Rr, phir, omegar, kappar);
    int iun = 0;
    memset(a, 0, sizeof(double)*nc*pht.size()*4);
    for (i = 0, itm = match.begin(); itm != match.end(); ++i, ++itm)
    {
        Point cp = ctl[itm->first];
        PhotoPoint pp = pht[itm->first];
        // left
        double xx = Rl[0]*(cp.x - Xsl) + Rl[3]*(cp.y - Ysl) + Rl[6]*(cp.z - Zsl);
        double yy = Rl[1]*(cp.x - Xsl) + Rl[4]*(cp.y - Ysl) + Rl[7]*(cp.z - Zsl);
        double zz = Rl[2]*(cp.x - Xsl) + Rl[5]*(cp.y - Ysl) + Rl[8]*(cp.z - Zsl);
        l[i*4] = pp.x1 + f*xx/zz;
        l[i*4+1] = pp.y1 + f*yy/zz;

        a[i*4*nc] = (Rl[0]*f + Rl[2]*pp.x1) / zz;
        a[i*4*nc+1] = (Rl[3]*f + Rl[5]*pp.x1) / zz;
        a[i*4*nc+2] = (Rl[6]*f + Rl[8]*pp.x1) / zz;
        a[i*4*nc+3] = pp.y1*sin(omegal) - (pp.x1/f*(pp.x1*cos(kappal) - pp.y1*sin(kappal)) + f*cos(kappal)) * cos(omegal);
        a[i*4*nc+4] = -f*sin(kappal) - pp.x1/f*(pp.x1*sin(kappal) + pp.y1*cos(kappal));
        a[i*4*nc+5] = pp.y1;
        a[(i*4+1)*nc] = (Rl[1]*f + Rl[2]*pp.y1) / zz;
        a[(i*4+1)*nc+1] = (Rl[4]*f + Rl[5]*pp.y1) / zz;
        a[(i*4+1)*nc+2] = (Rl[7]*f + Rl[8]*pp.y1) / zz;
        a[(i*4+1)*nc+3] = -pp.x1*sin(omegal) - (pp.y1/f*(pp.x1*cos(kappal)-pp.y1*sin(kappal)) - f*sin(kappal))*cos(omegal);
        a[(i*4+1)*nc+4] = -f*cos(kappal) - pp.y1/f*(pp.x1*sin(kappal) + pp.y1*cos(kappal));
        a[(i*4+1)*nc+5] = - pp.x1;

        // right
        xx = Rr[0]*(cp.x - Xsr) + Rr[3]*(cp.y - Ysr) + Rr[6]*(cp.z - Zsr);
        yy = Rr[1]*(cp.x - Xsr) + Rr[4]*(cp.y - Ysr) + Rr[7]*(cp.z - Zsr);
        zz = Rr[2]*(cp.x - Xsr) + Rr[5]*(cp.y - Ysr) + Rr[8]*(cp.z - Zsr);
        l[i*4+2] = pp.x2 + f*xx/zz;
        l[i*4+3] = pp.y2 + f*yy/zz;

        a[(i*4+2)*nc+6] = (Rr[0]*f + Rr[2]*pp.x2) / zz;
        a[(i*4+2)*nc+7] = (Rr[3]*f + Rr[5]*pp.x2) / zz ;
        a[(i*4+2)*nc+8] = (Rr[6]*f + Rr[8]*pp.x2) / zz;
        a[(i*4+2)*nc+9] = pp.y2*sin(omegar) - (pp.x2/f*(pp.x2*cos(kappar) - pp.y2*sin(kappar)) + f*cos(kappar)) * cos(omegar);
        a[(i*4+2)*nc+10] = -f*sin(kappar) - pp.x2/f*(pp.x2*sin(kappar) + pp.y2*cos(kappar));
        a[(i*4+2)*nc+11] = pp.y2;
        a[(i*4+3)*nc+6] = (Rr[1]*f + Rr[2]*pp.y2) / zz;
        a[(i*4+3)*nc+7] = (Rr[4]*f + Rr[5]*pp.y2) / zz;
        a[(i*4+3)*nc+8] = (Rr[7]*f + Rr[8]*pp.y2) / zz;
        a[(i*4+3)*nc+9] = -pp.x2*sin(omegar) - (pp.y2/f*(pp.x2*cos(kappar)-pp.y2*sin(kappar)) - f*sin(kappar))*cos(omegar);
        a[(i*4+3)*nc+10] = -f*cos(kappar) - pp.y2/f*(pp.x2*sin(kappar) + pp.y2*cos(kappar));
        a[(i*4+3)*nc+11] = - pp.x2;

        if (itm->second == -1)
        {
            a[i*4*nc+12+iun*3] = -a[i*4*nc];
            a[i*4*nc+12+iun*3+1] = -a[i*4*nc+1];
            a[i*4*nc+12+iun*3+2] = -a[i*4*nc+2];
            a[(i*4+1)*nc+12+iun*3] = -a[(i*4+1)*nc];
            a[(i*4+1)*nc+12+iun*3+1] = -a[(i*4+1)*nc+1];
            a[(i*4+1)*nc+12+iun*3+2] = -a[(i*4+1)*nc+2];

            a[(i*4+2)*nc+12+iun*3] = -a[(i*4+2)*nc+6];
            a[(i*4+2)*nc+12+iun*3+1] = -a[(i*4+2)*nc+7];
            a[(i*4+2)*nc+12+iun*3+2] = -a[(i*4+2)*nc+8];
            a[(i*4+3)*nc+12+iun*3] = -a[(i*4+3)*nc+6];
            a[(i*4+3)*nc+12+iun*3+1] = -a[(i*4+3)*nc+7];
            a[(i*4+3)*nc+12+iun*3+2] = -a[(i*4+3)*nc+8];
            ++iun;
        }
    }
}

bool Onestep::exact(double* x, int n)
{
    double lenlim = 1; // length limits
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
