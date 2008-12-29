#include "vector"
#include "cmath"
#include "map"

#include "intersection.h"
#include "phgproject.h"
#include "photopoints.h"
#include "controlpoints.h"
#include "globaldefn.h"
#include "transform.h"
#include "lls.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Intersection::Intersection(QString ctl, QString pht, QObject* parent)
: QObject(parent)
{
    m_ctl = ctl;
    m_pht = pht;
    m_numPhtPt = 0;
    for (int i = 0; i < 12; ++i)
    {
        m_orient.push_back(0);
        m_orients.push_back(0);
    }
}

Intersection::~Intersection()
{
}

void Intersection::setControl(double* pd)
{
}

void Intersection::setLeftPhoto(double* pd)
{
}

void Intersection::setRightPhoto(double* pd)
{
}

void Intersection::backPrepare(double* a, double* l, map<int, Point>& ctl, 
                           map<int, PhotoPoint>& pht, double f, 
                           vector<int>& match, int side)
{
    double R[9];
    double Xs, Ys, Zs, phi, omega, kappa;
    vector<int>::const_iterator itm;
    int i;    
    if (side == 0)
    {
        Xs = m_orient[0];
        Ys = m_orient[1];
        Zs = m_orient[2];
        phi = m_orient[3];
        omega = m_orient[4];
        kappa = m_orient[5];
        transform(R, phi, omega, kappa);

        for (i = 0, itm = match.begin(); itm != match.end(); ++i, ++itm)
        {
            Point cp = ctl[*itm];
            PhotoPoint pp = pht[*itm];
            double xx = R[0]*(cp.x - Xs) + R[3]*(cp.y - Ys) + R[6]*(cp.z - Zs);
            double yy = R[1]*(cp.x - Xs) + R[4]*(cp.y - Ys) + R[7]*(cp.z - Zs);
            double zz = R[2]*(cp.x - Xs) + R[5]*(cp.y - Ys) + R[8]*(cp.z - Zs);
            l[i*2] = pp.x1 + f*xx/zz;
            l[i*2+1] = pp.y1 + f*yy/zz;

            a[i*12+0] = (R[0]*f + R[2]*pp.x1) / zz;
            a[i*12+1] = (R[3]*f + R[5]*pp.x1) / zz ;
            a[i*12+2] = (R[6]*f + R[8]*pp.x1) / zz;
            a[i*12+3] = pp.y1*sin(omega) - (pp.x1/f*(pp.x1*cos(kappa) - pp.y1*sin(kappa)) + f*cos(kappa)) * cos(omega);
            a[i*12+4] = -f*sin(kappa) - pp.x1/f*(pp.x1*sin(kappa) + pp.y1*cos(kappa));
            a[i*12+5] = pp.y1;
            a[i*12+6] = (R[1]*f + R[2]*pp.y1) / zz;
            a[i*12+7] = (R[4]*f + R[5]*pp.y1) / zz;
            a[i*12+8] = (R[7]*f + R[8]*pp.y1) / zz;
            a[i*12+9] = -pp.x1*sin(omega) - (pp.y1/f*(pp.x1*cos(kappa)-pp.y1*sin(kappa)) - f*sin(kappa))*cos(omega);
            a[i*12+10] = -f*cos(kappa) - pp.y1/f*(pp.x1*sin(kappa) + pp.y1*cos(kappa));
            a[i*12+11] = - pp.x1;
        }
    }
    else
    {
        Xs = m_orient[6];
        Ys = m_orient[7];
        Zs = m_orient[8];
        phi = m_orient[9];
        omega = m_orient[10];
        kappa = m_orient[11];
        transform(R, phi, omega, kappa);

        for (i = 0, itm = match.begin(); itm != match.end(); ++i, ++itm)
        {
            Point cp = ctl[*itm];
            PhotoPoint pp = pht[*itm];
            double xx = R[0]*(cp.x - Xs) + R[3]*(cp.y - Ys) + R[6]*(cp.z - Zs);
            double yy = R[1]*(cp.x - Xs) + R[4]*(cp.y - Ys) + R[7]*(cp.z - Zs);
            double zz = R[2]*(cp.x - Xs) + R[5]*(cp.y - Ys) + R[8]*(cp.z - Zs);
            l[i*2] = pp.x2 + f*xx/zz;
            l[i*2+1] = pp.y2 + f*yy/zz;

            a[i*12+0] = (R[0]*f + R[2]*pp.x2) / zz;
            a[i*12+1] = (R[3]*f + R[5]*pp.x2) / zz ;
            a[i*12+2] = (R[6]*f + R[8]*pp.x2) / zz;
            a[i*12+3] = pp.y2*sin(omega) - (pp.x2/f*(pp.x2*cos(kappa) - pp.y2*sin(kappa)) + f*cos(kappa)) * cos(omega);
            a[i*12+4] = -f*sin(kappa) - pp.x2/f*(pp.x2*sin(kappa) + pp.y2*cos(kappa));
            a[i*12+5] = pp.y2;
            a[i*12+6] = (R[1]*f + R[2]*pp.y2) / zz;
            a[i*12+7] = (R[4]*f + R[5]*pp.y2) / zz;
            a[i*12+8] = (R[7]*f + R[8]*pp.y2) / zz;
            a[i*12+9] = -pp.x2*sin(omega) - (pp.y2/f*(pp.x2*cos(kappa)-pp.y2*sin(kappa)) - f*sin(kappa))*cos(omega);
            a[i*12+10] = -f*cos(kappa) - pp.y2/f*(pp.x2*sin(kappa) + pp.y2*cos(kappa));
            a[i*12+11] = - pp.x2;
        }
    }
}

bool Intersection::orientNotExact(double* orient)
{
    return !((fabs(orient[0]) < 1e-5) && (fabs(orient[1]) < 1e-5) && (fabs(orient[2]) < 1e-5) &&  
             (fabs(orient[3]) < 1e-7) && (fabs(orient[4]) < 1e-7) && (fabs(orient[5]) < 1e-7)); 
}

bool Intersection::forward()
{
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);
    double f;    // focus
    map<int, PhotoPoint> pht;
    int npht = tpht->data(&f, &pht);

    double Xsl, Ysl, Zsl, phil, omegal, kappal;
    double Xsr, Ysr, Zsr, phir, omegar, kappar;
    Xsl = m_orient[0];
    Ysl = m_orient[1];
    Zsl = m_orient[2];
    phil = m_orient[3];
    omegal = m_orient[4];
    kappal = m_orient[5];
    Xsr = m_orient[6];
    Ysr = m_orient[7];
    Zsr = m_orient[8];
    phir = m_orient[9];
    omegar = m_orient[10];
    kappar = m_orient[11];
    // compute the two transform matrixes.
    double Rl[9];
    double Rr[9];
    transform(Rl, phil, omegal, kappal);
    transform(Rr, phir, omegar, kappar);
    double B[3];
    B[0] = Xsr - Xsl;
    B[1] = Ysr - Ysl;
    B[2] = Zsr - Zsl;
    double pl[3], pr[3];
    double tpl[3], tpr[3];
    map<int, PhotoPoint>::const_iterator itp;
    int i;
    for (i = 0, itp = pht.begin(); itp != pht.end(); ++itp, ++i)
    {
        PhotoPoint p = itp->second;
        pl[0] = p.x1;
        pl[1] = p.y1;
        pl[2] = -f;
        pr[0] = p.x2;
        pr[1] = p.y2;
        pr[2] = -f;
        matrixMultiply(Rl, pl, tpl, 3, 3, 1);
        matrixMultiply(Rr, pr, tpr, 3, 3, 1);
        double N1, N2;
        N1 = (B[0]*tpr[2] - B[2]*tpr[0]) / (tpl[0]*tpr[2] - tpr[0]*tpl[2]);
        N2 = (B[0]*tpl[2] - B[2]*tpl[0]) / (tpl[0]*tpr[2] - tpr[0]*tpl[2]);
        Point rp;        // result 
        rp.x = Xsr + N2*tpr[0];
        rp.y = Ysr + N2*tpr[1];
        rp.z = Zsr + N2*tpr[2];
        m_forwardResult.insert(make_pair(itp->first, rp));
    }
    m_numPhtPt = npht;
    return true;
}

bool Intersection::backward()
{
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);
    ControlPoints* tctl = prj->controlPoints(m_ctl);
    double f;    // focus
    map<int, PhotoPoint> pht;
    map<int, Point> ctl;
    vector<int> match;    // matched photo point and control point keys
    int npht = tpht->data(&f, &pht);
    int nctl = tctl->data(&ctl);

    map<int, PhotoPoint>::const_iterator itp;
    map<int, Point>::const_iterator itc;
    for (itp = pht.begin(), itc = ctl.begin();
         itp!= pht.end() && itc != ctl.end();
         )
    {
        if (itp->first == itc->first)
        {
            match.push_back(itp->first);
            ++itc;
            ++itp;
        }
        else if (itp->first > itc->first)
            ++itc;
        else
            ++itp;
    }

    double phtscale = 1e4; // Photo scale;
    vector<int>::const_iterator itm;
    int n = match.size();     // matched number of points
    for (itm = match.begin(); itm != match.end(); ++itm)
    {
        m_orient[0] += ctl[*itm].x;
        m_orient[1] += ctl[*itm].y;
        m_orient[2] += ctl[*itm].z;
    }
    m_orient[0] /= n;
    m_orient[1] /= n;
    m_orient[2] = m_orient[2]/n + phtscale*f;
    m_orient[6] = m_orient[0];
    m_orient[7] = m_orient[1];
    m_orient[8] = m_orient[2];

    double* a = new double[12*n];
    double* l = new double[2*n];
    double* x = new double[2*n];
    double s[6];
    int maxit = 30;
    int it = 0;    
    do              // left photo
    {
        ++it;
        backPrepare(a, l, ctl, pht, f, match, 0);
        memcpy(x, l, sizeof(double)*2*n);
        lls(2*n, 6, a, 1, x, s);
        for (int i = 0; i < 6; ++i)
        {
            m_orient[i] += x[i];
        }
    } while (orientNotExact(x) && it < maxit);
    double* r = new double[6];
    residual(&r, a, x, l, 2*n, 6);
    for (int i = 0; i < 6; ++i)
    {
        m_orients[i] = r[i];
    }

    it = 0;
    do              // right photo
    {
        ++it;
        backPrepare(a, l, ctl, pht, f, match, 1);
        memcpy(x, l, sizeof(double)*2*n);
        lls(2*n, 6, a, 1, x, s);
        for (int i = 0; i < 6; ++i)
        {
            m_orient[i+6] += x[i];
        }
    } while (orientNotExact(x) && it < maxit);
    residual(&r, a, x, l, 2*n, 6);
    for (int i = 0; i < 6; ++i)
    {
        m_orients[i+6] = r[i];
    }

    delete []a;
    delete []l;
    delete []x;
    delete []r;
    return true;
}

int Intersection::forwardResult(map<int, Point>* result)
{
    *result = m_forwardResult;
    return m_forwardResult.size();
}

int Intersection::orient(vector<double>* o, vector<double>* os)
{
    *o = m_orient;
    *os = m_orients;
    return 12;
}
