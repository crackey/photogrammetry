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
    memset(m_orient, 0, sizeof(double)*12);
    memset(m_rol, 0, sizeof(double)*5);
    m_ctl = ctl;
    m_pht = pht;
}

Orientation::~Orientation()
{}

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
    double *data = 0;
    double f;
    int np;
    np = photoData(&data, &f);
    qDebug() << "relative orientation data";
    qDebug() << "focus:" << f;
    for (int i = 0; i < np; ++i)
        qDebug() << data[i*4] << data[i*4+1] << data[i*4+2] << data[i*4+3];

    double* a = new double[np*5];
    double* l = new double[np];
    double* o = m_orient;
    int maxit = 30;
    int itn = 0;
    memset(m_rol, 0, sizeof(double)*5);
    double bx = data[0] - data[2]; // bx = (x1-x2)1 ?? what's
    qDebug() << "bx:" << bx;
    double by; 
    double bz;
    double R1[9];
    double R2[9];
    R1[0] = a1(o);
    R1[1] = a2(o);
    R1[2] = a3(o);
    R1[3] = b1(o);
    R1[4] = b2(o);
    R1[5] = b3(o);
    R1[6] = c1(o);
    R1[7] = c2(o);
    R1[8] = c3(o);
//    np = 6;// for testing, should be deleted
    double N1, N2, Q, left[3], tleft[3], right[3], tright[3];
    do
    {
        ++itn;
        for (int i = 0; i < np; ++i)
        {
            R2[0] = a1(o+6);
            R2[1] = a2(o+6);
            R2[2] = a3(o+6);
            R2[3] = b1(o+6);
            R2[4] = b2(o+6);
            R2[5] = b3(o+6);
            R2[6] = c1(o+6);
            R2[7] = c2(o+6);
            R2[8] = c3(o+6);
            by = bx*m_rol[0];
            bz = bx*m_rol[1];
            right[0] = data[4*i+2];
            right[1] = data[4*i+3];
            right[2] = -f;
            left[0] = data[4*i];
            left[1] = data[4*i+1];
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
//            l[i] = 0;
//            for (int j = 0; j < 5; ++j)
//                l[i] += a[5*i+j]*ro[j];
            l[i] = Q;
        }
        double s[5];
        lls(np, 5, a, 1, l, s);
        for (int i = 0; i < 5; ++i)
        {
            m_rol[i] += l[i];
        }
        for (int i = 2; i < 5; ++i)
        {
            o[7+i] += l[i];
        }
    } while (!exact(l) && (itn < maxit));

    qDebug() << "Relative Orientation, number of iterations:" << itn;
    for (int i = 6; i < 12; ++i)
        qDebug() << m_orient[i];
    qDebug() << "relative orienments:";
    for (int i = 0; i < 5; ++i)
        qDebug() << m_rol[i];
    delete []a;
    delete []l;
    delete []data;
}

bool Orientation::absolute()
{
}

int Orientation::photoData(double** data, double* f)
{
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);

    map<int, PhotoPoint> *pht = &tpht->m_points;
    map<int, PhotoPoint>::iterator itp;
    int np = tpht->count(); // number of points.
    double* phtdata = new double[4*np];
    int i = 0;
    for (itp = pht->begin(); itp != pht->end(); ++itp, ++i)
    {
        phtdata[i*4+0] = itp->second.x1 - 100;
        phtdata[i*4+1] = 100 - itp->second.y1;
        phtdata[i*4+2] = phtdata[i*4] - itp->second.x2;
        phtdata[i*4+3] = phtdata[i*4+1] + 10 - itp->second.y2;
    } 
    *f = tpht->f();
    *data = phtdata;
    return np;
}

