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
    m_modelpoints = 0;
    m_index = 0;
    m_limit = 3e-5;
    m_result = 0;
    m_rol = new double[5];
    m_rols = new double[5];
    memset(m_rols, 0, sizeof(double)*5);
    memset(m_rol, 0, sizeof(double)*5); 
    memset(m_orient, 0, sizeof(double)*12);
}

Orientation::~Orientation()
{}

int Orientation::result(int** index, double** data) const
{
    *index = m_index;
    *data = m_result;
    return m_phtnum;
}

int Orientation::relativeOrientElements(double** data, double** s) const
{
    *s = m_rols;
    *data = m_rol;
    return 5;
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
    double *data = 0;
    double f;
    int np;
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);
    np = tpht->data(PhotoPoints::Left | PhotoPoints::Right, &f, &data);
//    np = photoData(&data, &f);
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
    transform(R1, m_orient);
//    np = 6;// for testing, should be deleted
    double N1, N2, Q, left[3], tleft[3], right[3], tright[3];
    do
    {
        ++itn;
        for (int i = 0; i < np; ++i)
        {
            transform(R2, m_orient+6);
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
            l[i] = Q;
        }
        lls(np, 5, a, 1, l, m_rols);
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

    // calculate model points.
    m_modelpoints = new double[np*3];
    double* mp = m_modelpoints;
    transform(R1, m_orient);
    transform(R2, m_orient+6);
    by = bx*m_rol[0];
    bz = bx*m_rol[1];
    for (int i = 0; i < np; ++i)
    {
        double m = 10000;
        right[0] = data[4*i+2];
        right[1] = data[4*i+3];
        right[2] = -f;
        left[0] = data[4*i];
        left[1] = data[4*i+1];
        left[2] = -f;
        matrixMultiply(R1, left, tleft, 3, 3, 1);
        matrixMultiply(R2, right, tright, 3, 3, 1);
        N1 = (bx*Z2 - bz*X2) / (X1*Z2 - X2*Z1);
        N2 = (bx*Z1 - bz*X1) / (X1*Z2 - X2*Z1);
        mp[i*3] = m * N1 * X1;
        mp[i*3+1] = m * 0.5*(N1*Y1+N2*Y2+by);
        mp[i*3+2] = m * f + m*N1*Z1;
    }
    
    qDebug() << "model points";
    for (int i = 0; i < np; ++i)
        qDebug() << mp[i*3] << mp[i*3+1] << mp[i*3+2];

    delete []a;
    delete []l;
    delete []data;
    return true;
}

bool Orientation::absolute()
{
    int np;
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);
    int* phtindex = 0;
    np = tpht->data(0, 0 , 0, &phtindex);
    m_phtnum = np;
    m_index = phtindex;
    ControlPoints* tctl = prj->controlPoints(m_ctl);
    double* ctldata = 0;
    int* ctlindex = 0;
    int nm;
    int nc = tctl->data(&ctldata, &ctlindex);
    nm = min(np, nc);
    int* ctlidx = new int[nm];
    int* modelidx = new int[nm];
    int i, j;
    nm = 0;
    for (i = 0, j = 0; i<np && j<nc; )
    {
        if (phtindex[i] == ctlindex[j])
        {
            ctlidx[nm] = j;
            modelidx[nm] = i;
            ++nm;
            ++i;
            ++j;
        }
        else if (phtindex[i] > ctlindex[j])
            ++j;
        else
            ++i;
    }
    memset(m_aol, 0, sizeof(double)*7);
    m_aol[3] = 1;
    double* a = new double[7*3*nm];
    memset(a, 0, sizeof(double)*21*nm);
    double* l = new double[3*nm];
    double s[7];
    double R[9];
    int maxit = 30;
    int itn = 0;
    do
    {
        ++itn;
        transform(R, m_aol+1);
        for (i = 0; i < nm; ++i)
        {
            double pp[3]; // Xp, Yp, Zp
            pp[0] = m_modelpoints[modelidx[i]*3];
            pp[1] = m_modelpoints[modelidx[i]*3+1];
            pp[2] = m_modelpoints[modelidx[i]*3+2];
            double p[3]; // R0*[Xp,Yp,Zp]T
            matrixMultiply(R, pp, p, 3, 3, 1);
            double tp[3];  // Xtp, Ytp, Ztp
            tp[0] = ctldata[ctlidx[i]*3+1]*1000;
            tp[1] = ctldata[ctlidx[i]*3]*1000;
            tp[2] = ctldata[ctlidx[i]*3+2]*1000;
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
        //for (int ii = 0; ii < 3*nm; ++ii)
        //    qDebug() << a[ii*7] << a[ii*7+1] << a[ii*7+2] << a[ii*7+3] << a[ii*7+4] 
        //<< a[ii*7+5] << a[ii*7+6];
        lls(3*nm, 7, a, 1, l, s);
        for (i = 0; i < 7; ++i)
        {
            m_aol[i] += l[i];
        }
    } while(itn < maxit && !exact(l));
    qDebug() << "Absolute orientation iterations: " << itn;

    m_result = new double[3*np];
    for (i = 0; i < np; ++i)
    {
        transform(R, m_aol+1);
        double pp[3];
        pp[0] = m_modelpoints[i*3];
        pp[1] = m_modelpoints[i*3+1];
        pp[2] = m_modelpoints[i*3+2];
        double p[3]; // R0*[Xp,Yp,Zp]T
        matrixMultiply(R, pp, p, 3, 3, 1);
        m_result[i*3] = m_aol[3]*p[0]+m_aol[0];
        m_result[i*3+1] = m_aol[3]*p[1]+m_aol[1];
        m_result[i*3+2] = m_aol[3]*p[2]+m_aol[2];
        qDebug() << m_result[i*3+1]/1000 << m_result[i*3]/1000 << m_result[i*3+2]/1000;
    }
    delete []a;
    delete []l;
   // delete []phtindex;
    delete []ctlindex;
    delete []ctldata;
    delete []ctlidx;
    delete []modelidx;
    return true;
}

