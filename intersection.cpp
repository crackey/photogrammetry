#include "cmath"

#include "intersection.h"
#include "lls.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Intersection::Intersection(QObject* parent)
    : QObject(parent)
{
}

Intersection::~Intersection()
{}

void Intersection::setControl(double* pd)
{
    m_ctl = pd;
}

void Intersection::setLeftPhoto(double* pd)
{
    m_lp = pd;
}

void Intersection::setRightPhoto(double* pd)
{
    m_rp = pd;
}

double Intersection::a1(double* orient)
{
    return cos(orient[3])*cos(orient[5]) - sin(orient[3])*sin(orient[4])*sin(orient[5]);
}

double Intersection::a2(double* orient)
{
    return -cos(orient[3])*sin(orient[5]) - sin(orient[3])*sin(orient[4])*cos(orient[5]);
}

double Intersection::a3(double* orient)
{
    return -sin(orient[3])*cos(orient[4]);
}

double Intersection::b1(double* orient)
{
    return cos(orient[4])*sin(orient[5]);
}

double Intersection::b2(double* orient)
{
    return cos(orient[4])*cos(orient[5]);
}

double Intersection::b3(double* orient)
{
    return -sin(orient[4]);
}

double Intersection::c1(double* orient)
{
    return sin(orient[3])*cos(orient[5]) + cos(orient[3])*sin(orient[4])*sin(orient[5]);
}

double Intersection::c2(double* orient)
{
    return -sin(orient[3])*sin(orient[5]) + cos(orient[3])*sin(orient[4])*cos(orient[5]);
}

double Intersection::c3(double* orient)
{
    return cos(orient[3])*cos(orient[4]);
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

int Intersection::backward(double* pht, double* ctl, double* orient, double f, int n)
{
    double* a = new double[12*n];
    double* l = new double[2*n];
    //double a[60] = {0};
    //double l[10] = {0};
    double s[6];
    l[0] = 1000;
    int maxit = 30;
    int it = 0;
    while (orientNotExact(l) && it < maxit)
    {
        ++it;
        for (int i = 0; i < n; ++i)
        {
            //double ax, ay; // approximation of x and y
            //ax = pht[2*i];
            //ay = ;
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
        lls(2*n, 6, a, 1, l, s);
        for (int i = 0; i < 6; ++i)
        {
      //      dorient[i] = l[i];
            orient[i] += l[i];
        }
        
    }
    delete []a;
    delete []l;
    return 0;
}

int Intersection::forward(double* p , /* photo data */
                   double* o, /* orient elements */
                   double* out, /* output */
                   int n)/* number of points*/
{
    double R1[9];
    double R2[9];

    // compute the two transform matrixes.
    R1[0] = a1(o);
    R1[1] = a2(o);
    R1[2] = a3(o);
    R1[3] = b1(o);
    R1[4] = b2(o);
    R1[5] = b3(o);
    R1[6] = c1(o);
    R1[7] = c2(o);
    R1[8] = c3(o);

    R2[0] = a1(o+6);
    R2[1] = a2(o+6);
    R2[2] = a3(o+6);
    R2[3] = b1(o+6);
    R2[4] = b2(o+6);
    R2[5] = b3(o+6);
    R2[6] = c1(o+6);
    R2[7] = c2(o+6);
    R2[8] = c3(o+6);

    double B[3];
    B[0] = o[6] - o[0];
    B[1] = o[7] - o[1];
    B[2] = o[8] - o[2];

    double p1[3], p2[3];
    for (int i = 0; i < n; ++i)
    {
        matrixMultiply(R1, p+6*i, p1, 3, 3, 1);
        matrixMultiply(R2, p+6*i+3, p2, 3, 3, 1);
        double N1, N2;
        N1 = (B[0]*p2[2] - B[2]*p2[0]) / (p1[0]*p2[2] - p2[0]*p1[2]);
        N2 = (B[0]*p1[2] - B[2]*p1[0]) / (p1[0]*p2[2] - p2[0]*p1[2]);
        out[3*i] = o[6] + N2*p2[0];
        out[3*i+1] = o[7] + N2*p2[1];
        out[3*i+2] = o[8] + N2*p2[2];
    }

    return 0;
}

bool Intersection::photoData()
{
    int np = 0; // number of matched points
    PhotoPoints* tpht = m_prj->photoPoints(m_prj->curPhotoPoints());
    ControlPoints* tctl = m_prj->controlPoints(m_prj->curControlPoints());
   // tpht = m_pht[m_curPhotoPoints];
   // tctl = m_ctl[m_curControlPoints];
    map<int, Point> *ctl = &tctl->m_points;
    map<int, vector<Point> > *pht = &tpht->m_points;
    map<int, Point>::iterator itc;
    map<int, vector<Point> >::iterator itp;
    int n = min(ctl->size(), pht->size());
    int* keys = new int[n];
    for (itc = ctl->begin(), itp = pht->begin();
         (itc != ctl->end()) && (itp != pht->end()); 
         )
    {
        if (itc->first == itp->first)
        {
            keys[np] = itc->first;
            ++np;
            ++itc; 
            ++itp;
        }
        else if (itc->first < itp->first)
            ++itc;
        else
            ++itp;
    }
    double* phtdata = new double[2*np];
    double* ctldata = new double[3*np];
    double scalex;
    double scaley;
    scalex = tpht->m_fiducial[2] / tpht->m_fiducial[0];
    scaley = tpht->m_fiducial[3] / tpht->m_fiducial[1];
    for (int i = 0; i < np; ++i)
    {
        ctldata[i*3] = (*ctl)[keys[i]].y * 1e3; // x and y should be reverted
        ctldata[i*3+1] = (*ctl)[keys[i]].x * 1e3;
        ctldata[i*3+2] = (*ctl)[keys[i]].z * 1e3;

        switch (p)
        {
        case 0:   // the left photo
            phtdata[i*2] = (*pht)[keys[i]][0].x - 100;
            phtdata[i*2+1] = 100 - (*pht)[keys[i]][0].y;
            break;
        case 1:   // the right photo
            phtdata[i*2] = (*pht)[keys[i]][0].x - 100 - (*pht)[keys[i]][1].x;
            phtdata[i*2+1] = 100 - (*pht)[keys[i]][0].y + 10 - (*pht)[keys[i]][1].y;
            break;
        default:
            break;
        }
        phtdata[i*2] *= scalex;
        phtdata[i*2+1] *= scaley;
    }
    *focus = -(*pht)[keys[0]][0].z;
    *ppht = phtdata;
    *pctl = ctldata;

    return true;

}

bool Intersection::controlData()
{
    return true;
}

