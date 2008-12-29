#include <QObject>
#include <cmath>

#include "phgproject.h"
#include "photopoints.h"
#include "controlpoints.h"
#include "transform.h"
#include "lls.h"
#include "orientation.h"

Orientation::Orientation(QString ctl, QString pht, QObject* parent)
    : QObject(parent)
{

    m_ctl = ctl;
    m_pht = pht;
    m_limit = 3e-5;
    int i;
    for (i = 0; i < 5; ++i)
    {
        m_rols.push_back(0);
        m_rol.push_back(0);
    }
    for (i = 0; i < 7; ++i)
    {
        m_aol.push_back(0);
        m_aols.push_back(0);
    }
    for (i = 0; i < 12; ++i)
    {
        m_orient.push_back(0);
    }
}

Orientation::~Orientation()
{}

int Orientation::result(map<int, Point>* point) const
{
    *point = m_result;
    return m_phtnum;
}

int Orientation::relativeOrientElements(vector<double>* o, vector<double>* s) const
{
    *s = m_rols;
    *o = m_rol;
    return 5;
}

int Orientation::absoluteOrientElements(vector<double>* o, vector<double>* s) const
{
    *s = m_aols;
    *o= m_aol;
    return 7;
}

bool Orientation::exact(double* data)
{
    for (int i = 0; i < 5; ++i)
    {
        if (fabs(data[i]) > m_limit)
            return false;
    }
    return true;
}

bool Orientation::relative()
{
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);
    double f;    // focus
    map<int, PhotoPoint> pht;
    int np = tpht->data(&f, &pht);
    double* a = new double[np*5];
    double* l = new double[np];
	double* x = new double[np];
    double* rols = new double[5];
    map<int, PhotoPoint>::const_iterator itp;
    int maxit = 30;
    int itn = 0;
    itp = pht.begin();
    double bx = itp->second.x1 - itp->second.x2; // bx = (x1-x2)1 ?? what's
    qDebug() << "bx:" << bx;
    double by; 
    double bz;
    double R1[9];
    double R2[9];
    transform(R1, m_orient[3], m_orient[4], m_orient[5]);

    double N1, N2, Q, left[3], tleft[3], right[3], tright[3];
    do
    {
        ++itn;
        int i;
        for (itp = pht.begin(), i = 0; itp != pht.end(); ++itp, ++i)
        {
            transform(R2, m_orient[9], m_orient[10], m_orient[11]);
            by = bx*m_rol[0];
            bz = bx*m_rol[1];
            right[0] = itp->second.x2;
            right[1] = itp->second.y2;
            right[2] = -f;
            left[0] = itp->second.x1;
            left[1] = itp->second.y1;
            left[2] = -f;
            matrixMultiply(R1, left, tleft, 3, 3, 1);
            matrixMultiply(R2, right, tright, 3, 3, 1);

#define X1 tleft[0]
#define Y1 tleft[1]
#define Z1 tleft[2]
#define X2 tright[0]
#define Y2 tright[1]
#define Z2 tright[2]

            N1 = (bx*Z2 - bz*X2) / (X1*Z2 - X2*Z1);
            N2 = (bx*Z1 - bz*X1) / (X1*Z2 - X2*Z1);
            Q = N1*Y1 - N2*Y2 - by;
            a[5*i+0] = bx;
            a[5*i+1] = -Y2 / Z2 * bx;
            a[5*i+2] = -X2 * Y2 * N2 / Z2;
            a[5*i+3] = -(Z2 + Y2*Y2/Z2) * N2;
            a[5*i+4] = X2 * N2;
            l[i] = Q;
        }
		memcpy(x, l, sizeof(double)*np);
        lls(np, 5, a, 1, x, rols);

        for (int i = 0; i < 5; ++i)
        {
            m_rol[i] += x[i];
        }
        for (int i = 2; i < 5; ++i)
        {
            m_orient[7+i] += x[i];
        }
    } while (!exact(l) && (itn < maxit));
    residual(&rols, a, x, l, np, 5);
    for (int i = 0; i < 5; ++i)
        m_rols[i] = rols[i];

    // calculate model points.
    transform(R1, m_orient[3], m_orient[4], m_orient[5]);
    transform(R2, m_orient[9], m_orient[10], m_orient[11]);
    by = bx*m_rol[0];
    bz = bx*m_rol[1];
    int i;
    for (itp = pht.begin(), i = 0; itp != pht.end(); ++itp, ++i)
    {
        double m = 10000;
        right[0] = itp->second.x2;
        right[1] = itp->second.y2;
        right[2] = -f;
        left[0] = itp->second.x1;
        left[1] = itp->second.y1;
        left[2] = -f;
        matrixMultiply(R1, left, tleft, 3, 3, 1);
        matrixMultiply(R2, right, tright, 3, 3, 1);
        N1 = (bx*Z2 - bz*X2) / (X1*Z2 - X2*Z1);
        N2 = (bx*Z1 - bz*X1) / (X1*Z2 - X2*Z1);
        Point p;
        p.x = m * N1 * X1;
        p.y = m * 0.5*(N1*Y1+N2*Y2+by);
        p.z = m * f + m*N1*Z1;
        m_modelpoints.insert(make_pair(itp->first, p));
    }

    delete []a;
    delete []l;
    delete []x;
    delete []rols;
    return true;
}

bool Orientation::absolute()
{
    PHGProject* prj = (PHGProject*)parent();
    ControlPoints* tctl = prj->controlPoints(m_ctl);
    map<int, Point> ctl;
    int nctl = tctl->data(&ctl);
    map<int, Point>::const_iterator itc;
    map<int, Point>::const_iterator itm;
    vector<int> match;    // matched photo point and control point keys
    for (itm = m_modelpoints.begin(), itc = ctl.begin();
         itm!= m_modelpoints.end() && itc != ctl.end();
         )
    {
        if (itm->first == itc->first)
        {
            match.push_back(itm->first);
            ++itc;
            ++itm;
        }
        else if (itm->first > itc->first)
            ++itc;
        else
            ++itm;
    }

    int nm = match.size();
    m_aol[3] = 1;
    double* a = new double[7*3*nm];
    memset(a, 0, sizeof(double)*21*nm);
    double* l = new double[3*nm];
    double* x = new double[3*nm];
    double s[7];
    double R[9];
    int maxit = 30;
    int itn = 0;
    int i;
    do
    {
        ++itn;
        transform(R, m_aol[4], m_aol[5], m_aol[6]);
        for (i = 0; i < nm; ++i)
        {
            double pp[3]; // Xp, Yp, Zp
            pp[0] = m_modelpoints[match[i]].x;
            pp[1] = m_modelpoints[match[i]].y;
            pp[2] = m_modelpoints[match[i]].z;
            double p[3]; // R0*[Xp,Yp,Zp]T
            matrixMultiply(R, pp, p, 3, 3, 1);
            double tp[3];  // Xtp, Ytp, Ztp
            tp[0] = ctl[match[i]].x;
            tp[1] = ctl[match[i]].y;
            tp[2] = ctl[match[i]].z;
            l[3*i] = tp[0] - p[0]*m_aol[3] - m_aol[0];
            l[3*i+1] = tp[1] - p[1]*m_aol[3] - m_aol[1];
            l[3*i+2] = tp[2] - p[2]*m_aol[3] - m_aol[2];
            a[i*21] = 1;
            a[i*21+3] = pp[0];
            a[i*21+4] = -pp[2];
            a[i*21+6] = -pp[1];
            a[i*21+8] = 1;
            a[i*21+10] = pp[1];
            a[i*21+12] = -pp[2];
            a[i*21+13] = pp[0];
            a[i*21+16] = 1;
            a[i*21+17] = pp[2];
            a[i*21+18] = pp[0];
            a[i*21+19] = pp[1];
        }
        memcpy(x, l, sizeof(double)*3*nm);
        lls(3*nm, 7, a, 1, x, s);
        for (i = 0; i < 7; ++i)
        {
            m_aol[i] += x[i];
        }
    } while(itn < maxit && !exact(l));
    double* r = new double[7];
    residual(&r, a, x, l, 3*nm, 7);
    for (i = 0; i < 7; ++i)
        m_aols[i] = r[i];

    for (itm = m_modelpoints.begin(); itm != m_modelpoints.end(); ++itm)
    {
        transform(R, m_aol[4], m_aol[5], m_aol[6]);
        double pp[3];
        pp[0] = itm->second.x;
        pp[1] = itm->second.y;
        pp[2] = itm->second.z;
        double p[3]; // R0*[Xp,Yp,Zp]T
        matrixMultiply(R, pp, p, 3, 3, 1);
        Point resultp;
        resultp.x = m_aol[3]*p[0]+m_aol[0];
        resultp.y = m_aol[3]*p[1]+m_aol[1];
        resultp.z = m_aol[3]*p[2]+m_aol[2];
        m_result.insert(make_pair(itm->first, resultp));
    }
    m_phtnum = m_modelpoints.size();
    delete []a;
    delete []l;
   // delete []phtindex;
    delete []x;
    delete []r;
    return true;
}
