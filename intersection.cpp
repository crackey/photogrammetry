#include "cmath"

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
    m_forwardResult = 0;
    m_numPhtPt = 0;
    m_index = 0;
    for (int i = 0; i < 12; ++i)
        m_orient[i] = 0.0;
}

Intersection::~Intersection()
{
    if (m_forwardResult != 0)
        delete m_forwardResult;
    if (m_index != 0)
        delete m_index;
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

double Intersection::z_(double* orient, double* ctl, int index)
{
    return a3(orient)*(ctl[3*index] - orient[0])
        + b3(orient)*(ctl[3*index+1] - orient[1])
        + c3(orient)*(ctl[3*index+2] - orient[2]);
}

void Intersection::ma(double* a, double* ctl, double* pht, double f, double* orient, int index)
{
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
}

bool Intersection::orientNotExact(double* orient)
{
    return !((fabs(orient[0]) < 1e-5) && (fabs(orient[1]) < 1e-5) && (fabs(orient[2]) < 1e-5) &&  
             (fabs(orient[3]) < 1e-7) && (fabs(orient[4]) < 1e-7) && (fabs(orient[5]) < 1e-7)); 
}

bool Intersection::forward()
{
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = (prj->photoPoints(prj->curPhotoPoints()));
#if 0
    map<int, PhotoPoint>* pht = &tpht->m_points;
    int np;
    np = pht->size();
    double* phtdata = new double[6*np];
    map<int, PhotoPoint>::iterator it;
    it = pht->begin();

    double scalex;
    double scaley;
    scalex = tpht->m_fiducial[2] / tpht->m_fiducial[0];
    scaley = tpht->m_fiducial[3] / tpht->m_fiducial[1];
    m_index = new int[np];
    for (int i = 0; it != pht->end(); ++it, ++i)
    {
        m_index[i] = it->first;
        phtdata[i*6] = it->second.x1 - 100;
        phtdata[i*6+1] = 100 - it->second.y1;
//        phtdata[i*6+2] = it->second[0].z;
        phtdata[i*6+2] = -tpht->f();
        phtdata[i*6+3] = phtdata[i*6] - it->second.x2;
        phtdata[i*6+4] = phtdata[i*6+1] + 10 - it->second.y2;
        phtdata[i*6+5] = -tpht->f();

        phtdata[i*6] /= scalex;
        phtdata[i*6+1] /= scaley;
        phtdata[i*6+3] /= scalex;
        phtdata[i*6+4] /= scaley;
    }
#endif
    double focus;
    double* phtdata = 0;
    int np;
    np = tpht->data(PhotoPoints::Left | PhotoPoints::Right, &focus, &phtdata, &m_index);

    qDebug() << "forward intersection photo data";
    for (int i = 0; i < tpht->count(); ++i)
    {
        qDebug() << phtdata[i*4] << phtdata[i*4+1] << phtdata[i*4+2] << phtdata[i*4+3];
    }

    m_forwardResult = new double[3*np];
    int status = forward_impl(focus, phtdata, m_orient, m_forwardResult, np);

    qDebug() << "forward result:";
    for (int i = 0; i < np; ++i)
    {
        qDebug() << m_forwardResult[3*i]/1e3 << m_forwardResult[3*i+1]/1e3 << m_forwardResult[3*i+2]/1e3;
    }
    delete []phtdata;
    if (status == 0)
    {
        m_numPhtPt = np;
        return true;
    }
    else
    {
        if (m_forwardResult != 0)
            delete []m_forwardResult;
        m_numPhtPt = 0;
    }
    return false;
}

bool Intersection::backward()
{
    // get the left photo
    double *lphtdata = 0;
    double *lctldata = 0;
    double lfocus;
    int lnumPoints;

    lnumPoints = backwardData(&lphtdata, &lctldata, &lfocus, 0); 
    qDebug() << "Backward intersection.\n Number of matched Points:" << lnumPoints;
    qDebug() << "left photo data";
    for (int i = 0; i < lnumPoints; ++i)
    {
        qDebug() << lphtdata[i*2+0] << lphtdata[i*2+1] << lctldata[i*3+0] << lctldata[i*3+1] << lctldata[i*3+2];
    }

    for (int i = 0; i < lnumPoints; ++i)
    {
        m_orient[0] += lctldata[i*3];
        m_orient[1] += lctldata[i*3+1];
        m_orient[2] += lctldata[i*3+2];
    }
    m_orient[0] /= lnumPoints;
    m_orient[1] /= lnumPoints;
    m_orient[2] = m_orient[2]/lnumPoints + 1e3*lfocus;
    m_orient[3] = m_orient[4] = m_orient[5] = 0.0;
    int left = backward_impl(lphtdata, lctldata, m_orient, lfocus, lnumPoints, 0);
    delete []lphtdata;
    delete []lctldata;
    
    // get the right photo
    double *rphtdata = 0;
    double *rctldata = 0;
    double rfocus;
    int rnumPoints;

    rnumPoints = backwardData(&rphtdata, &rctldata, &rfocus, 1); 
    qDebug() << "right photo data";
    for (int i = 0; i < rnumPoints; ++i)
    {
        qDebug() << rphtdata[i*2+0] << rphtdata[i*2+1] << rctldata[i*3+0] << rctldata[i*3+1] << rctldata[i*3+2];
    }
    for (int i = 0; i < lnumPoints; ++i)
    {
        m_orient[6] += rctldata[i*3];
        m_orient[7] += rctldata[i*3+1];
        m_orient[8] += rctldata[i*3+2];
    }
    m_orient[6] /= rnumPoints;
    m_orient[7] /= rnumPoints;
    m_orient[8] = m_orient[2]/rnumPoints + 1e3*rfocus;
    m_orient[9] = m_orient[10] = m_orient[11] = 0.0;
    int right = backward_impl(rphtdata, rctldata, m_orient+6, rfocus, rnumPoints, 1);
    delete []rphtdata;
    delete []rctldata;

    qDebug() << "Orient elements:"; 
    for (int i = 0; i < 12; ++i)
    {
        qDebug() << m_orient[i];
    }

    if ((left==0) && (right==0))
        return true;
    return false;
}

int Intersection::backward_impl(double* pht, double* ctl, double* orient, double f, int n, int side)
{
    double* a = new double[12*n];
    double* l = new double[2*n];

    double* x = new double[2*n];
    double s[6];
    int maxit = 30;
    int it = 0;    
    do
    {
        ++it;
        for (int i = 0; i < n; ++i)
        {
            ma(a+12*i, ctl, pht, f, orient, i);
            l[i*2] = pht[2*i] + f * (a1(orient)*(ctl[3*i]-orient[0]) + 
                                     b1(orient)*(ctl[3*i+1]-orient[1]) + 
                                     c1(orient)*(ctl[3*i+2]-orient[2])
                                    ) / z_(orient, ctl, i);
            l[i*2+1] = pht[2*i+1] + f * (a2(orient)*(ctl[3*i]-orient[0]) + 
                                         b2(orient)*(ctl[3*i+1]-orient[1]) + 
                                         c2(orient)*(ctl[3*i+2]-orient[2])
                                        ) / z_(orient, ctl, i);
        }
        memcpy(x, l, sizeof(double)*2*n);
        lls(2*n, 6, a, 1, x, s);
        for (int i = 0; i < 6; ++i)
        {
            orient[i] += x[i];
        }
    } while (orientNotExact(x) && it < maxit);
    double* r = (side == 0) ? m_orients : m_orients+6;
    residual(&r, a, x, l, 2*n, 6);

    qDebug() << "\n" << "Backward Iterations: " << it;
    delete []a;
    delete []l;
    delete []x;
    return 0;
}

int Intersection::forward_impl(double f,   /* focus */
                               double* p , /* photo data */
                               double* o, /* orient elements */
                               double* out, /* output */
                               int n)/* number of points*/
{
    double R1[9];
    double R2[9];

    // compute the two transform matrixes.
    transform(R1, o);
    transform(R2, o+6);

    double B[3];
    B[0] = o[6] - o[0];
    B[1] = o[7] - o[1];
    B[2] = o[8] - o[2];

    double p1[3], p2[3];
    double op1[3], op2[3];
    for (int i = 0; i < n; ++i)
    {
        op1[0] = p[4*i];
        op1[1] = p[4*i+1];
        op1[2] = -f;
        op2[0] = p[4*i+2];
        op2[1] = p[4*i+3];
        op2[2] = -f;
        matrixMultiply(R1, op1, p1, 3, 3, 1);
        matrixMultiply(R2, op2, p2, 3, 3, 1);
        double N1, N2;
        N1 = (B[0]*p2[2] - B[2]*p2[0]) / (p1[0]*p2[2] - p2[0]*p1[2]);
        N2 = (B[0]*p1[2] - B[2]*p1[0]) / (p1[0]*p2[2] - p2[0]*p1[2]);
        out[3*i] = o[6] + N2*p2[0];
        out[3*i+1] = o[7] + N2*p2[1];
        out[3*i+2] = o[8] + N2*p2[2];
    }
    return 0;
}

// side = 0 for left photo, 1 for right
int Intersection::backwardData(double** ppht, double** pctl, double* focus, int side)  
{
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* tpht = prj->photoPoints(m_pht);
    ControlPoints* tctl = prj->controlPoints(m_ctl);
    // tpht = m_pht[m_curPhotoPoints];
    // tctl = m_ctl[m_curControlPoints];
    int* phtindex;
    int* ctlindex;
    double* phtdata;
    double* ctldata;
    int nctlp;
    int nphtp;
    nctlp = tctl->data(&ctldata, &ctlindex);
    nphtp = tpht->data((side == 0 ? PhotoPoints::Left : PhotoPoints::Right),
                       focus, &phtdata, &phtindex);
    int i, j;
    int np = 0; // number of matched points
    // the keys was guaranteed in increasing order
    for (i = 0, j = 0; i<nphtp && j<nctlp; )
    {
        if (phtindex[i] == ctlindex[j])
        {
            ++np;
            ++i;
            ++j;
        }
        else if (phtindex[i] > ctlindex[j])
            ++j;
        else
            ++i;
    }
    double* mpd = new double[2*np]; // matched photo data
    double* mpc = new double[3*np]; // matched control data
    np = 0;
    for (i = 0, j = 0; i<nphtp && j<nctlp; )
    {
        if (phtindex[i] == ctlindex[j])
        {
            mpd[np*2] = phtdata[i*2];
            mpd[np*2+1] = phtdata[i*2+1];
            // x and y should be exchanged
            // control data was in meters while photo data in milimeters
            mpc[np*3] = ctldata[j*3+1] * 1000;    
            mpc[np*3+1] = ctldata[j*3] * 1000; 
            mpc[np*3+2] = ctldata[j*3+2] * 1000;
            ++np;
            ++i;
            ++j;
        }
        else if (phtindex[i] > ctlindex[j])
            ++j;
        else
            ++i;
    }
    *ppht = mpd;
    *pctl = mpc;
    delete []phtindex;
    delete []ctlindex;
    delete []phtdata;
    delete []ctldata;

    return np;
}

int Intersection::forwardResult(int** index, double** result)
{
    *index = m_index;
    *result = m_forwardResult;
    return m_numPhtPt;
}

int Intersection::orient(double** o, double** os)
{
    *o = m_orient;
    *os = m_orients;
    return 12;
}
