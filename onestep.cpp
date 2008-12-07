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
}

Onestep::~Onestep()
{

}

#if 0
void Onestep::prepare(double** ctl, double** pht, double* f, int** phtindex, int** ctlindex, int* numctl, int* numpht)
{
    ControlPoints* pctl = static_cast<PHGProject*>parent()->controlPoints(m_ctl);
    PhotoPoints* ppht = static_cast<PHGProject*>parent()->photoPoints(m_pht);

    double* phtdata = 0;
    double* ctldata = 0;
    int* pi = 0;
    int* ci = 0;
    double focus;
    int npht;         // number of controlpoints.
    int nctl;         // number of photopoints.
    nctl = pctl->data(&ctldata, &ctlindex);
    npht = ppht->data(PhotoPoints::Left | PhotoPoints::Right,
                      &focus, &phtdata, &phtindex);
    int* matchedindex = new int[min(npht, nctl)];
    int nmatched = 0;
    for (i = 0, j = 0; i<npht && j<nctl;)
    {
        if (phtindex[i] == ctlindex[j])
        {
            matchedindex[i] = phtindex[i];
            ++nmatched;
            ++i;
            ++j;
        }
        if (phtindex[i] < ctlindex[j])
        {
            ++i;
        }
        else
            ++j;
    }
 
}
#endif

bool Onestep::calculate()
{
    ControlPoints* pctl = static_cast<PHGProject*>(parent())->controlPoints(m_ctl);
    PhotoPoints* ppht = static_cast<PHGProject*>(parent())->photoPoints(m_pht);
    vector<double> pht;
    vector<int> index;
    double focus;
    int np;               // total number of points
    np = ppht->data(3, &focus, &pht, &index);
    map<int, Point> ctl;
    int nc;                // number of control points
    nc = pctl->data(&ctl, &index);
    int nun = np - nc;     // number of unknow points

    double* a = new double[4*np*(12+3*nun)]; // matrix A and B
    double* l = new double[4*np];              // matrix l
    double* s = new double[4*np];
    double* orient = new double[12];
    double* points = new double[3*np];
    memset(orient, 0, sizeof(double)*12);
    memset(points, 0, sizeof(double)*3*np);

    // initialize orient elements
    for (int i = 0; i < nc; ++i)
    {
        orient[0] += points[i*3];
        orient[1] += points[i*3+1];
        orient[2] += points[i*3+2];
    }
    orient[0] /= nc;
    orient[1] /= nc;
    orient[2] = orient[2]/nc + 1e3*focus;
    orient[3] = orient[4] = orient[5] = 0.0;
    memcpy(orient+6, orient, sizeof(double)*6);

    // initialize points values
    map<int, Point>::const_iterator itc;
    int i;
    int* isctl = new int[np];
    memset(isctl, 0, sizeof(int)*np);
    for (i = 0, itc = ctl.begin(); i < np && itc != ctl.end(); ++i)
    {
        points[i*3] = itc->second.x;
        points[i*3+1] = itc->second.y;
        points[i*3+2] = itc->second.z;
        if (index[i] == itc->first)
        {
            isctl[i] = 1;
            ++itc;
        }
    }
    
    int maxit = 30;
    int itn = 0;
#if 0
    do
    {
        // set matrix a
        memset(a, 0, sizeof(double)*4*np*(12+3*nc));
        for (i = 0; i < np; ++i)
        {
            int nc = n*3 + 12; // number of columns of matrix a
            a[0] = (a1(o)*f + a3(o)*pht[i*4]) / z_(o, ctl, i);
            a[1] = (b1(o)*f + b3(o)*pht[i*4]) / z_(o, ctl, i);
            a[2] = (c1(o)*f + c3(o)*pht[i*4]) / z_(o, ctl, i);
            a[3] = pht[i*4+1]*sin(o[4]) - 
                (pht[i*4]*(pht[i*4]*cos(o[5])-pht[i*4+1]*sin(o[5]))/f
                 + f*cos(o[5])) * cos(o[4]);
            a[4] = -f*sin(o[5]) - 
                pht[i*4] * (pht[i*4]*sin(o[5]) + pht[i*4+1]*cos(o[5])) / f;
            a[5] = pht[i*4+1];
            a[nc] = (a2(o)*f + a3(o)*pht[i*4+1]) / z_(o, ctl, i);
            a[nc+1] = (b2(o)*f + b3(o)*pht[i*4+1]) / z_(o, ctl, i);
            a[nc+2] = (c2(o)*f + c3(o)*pht[i*4+1]) / z_(o, ctl, i);
            a[nc+3] = -pht[i*4]*sin(o[4]) - (pht[i*4+1]*(pht[i*4]*cos(o[5])
                                                         -pht[i*4+1]*sin(o[5]))/f 
                                             -f*sin(o[5]))*cos(o[4]);
            a[nc+4] = -f*cos(o[5]) - pht[i*4+1] 
                      * (pht[i*4]*sin(o[5]) + pht[i*4+1]*cos(o[5])) / f;
            a[nc+5] = -pht[i*4];
        }
    } while(!exact(l,12+3*nun) && itn<maxit);
#endif
    return true;
}

#if 0
void Onestep::ma(vector<double> pht, double* p, double* o, int isc, double f, int n, int s)
{
    int nc = n*3 + 12; // number of columns of matrix a
    a[0] = (a1(o)*f + a3(o)*pht[i*4]) / z_(o, ctl, i);
    a[1] = (b1(o)*f + b3(o)*pht[i*4]) / z_(o, ctl, i);
    a[2] = (c1(o)*f + c3(o)*pht[i*4]) / z_(o, ctl, i);
    a[3] = pht[i*4+1]*sin(o[4]) - 
        (pht[i*4]*(pht[i*4]*cos(o[5])-pht[i*4+1]*sin(o[5]))/f
         + f*cos(o[5])) * cos(o[4]);
    a[4] = -f*sin(o[5]) - 
        pht[i*4] * (pht[i*4]*sin(o[5]) + pht[i*4+1]*cos(o[5])) / f;
    a[5] = pht[i*4+1];
    a[nc] = (a2(o)*f + a3(o)*pht[i*4+1]) / z_(o, ctl, i);
    a[nc+1] = (b2(o)*f + b3(o)*pht[i*4+1]) / z_(o, ctl, i);
    a[nc+2] = (c2(o)*f + c3(o)*pht[i*4+1]) / z_(o, ctl, i);
    a[nc+3] = -pht[i*4]*sin(o[4]) - (pht[i*4+1]*(pht[i*4]*cos(o[5])
                                                 -pht[i*4+1]*sin(o[5]))/f 
                                     -f*sin(o[5]))*cos(o[4]);
    a[nc+4] = -f*cos(o[5]) - pht[i*4+1] * (pht[i*4]*sin(o[5]) + pht[i*4+1]*cos(o[5])) / f;
    a[nc+5] = -pht[i*4];
}
#endif

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
    for (i = 12; i < 12+3*n; ++i)
    {
        if (fabs(x[i]) > lenlim)
            return false;
    }
    return true;
}

#if 0
bool Onestep::calculate()
{
    ControlPoints* pctl = static_cast<PHGProject*>parent()->controlPoints(m_ctl);
    PhotoPoints* ppht = static_cast<PHGProject*>parent()->photoPoints(m_pht);

    double* phtdata = 0;
    double* ctldata = 0;
    int* phtindex = 0;
    int* ctlindex = 0;
    double focus;
    int npht;         // number of controlpoints.
    int nctl;         // number of photopoints.
    nctl = pctl->data(&ctldata, &ctlindex);
    npht = ppht->data(PhotoPoints::Left | PhotoPoints::Right,
                      &focus, &phtdata, &phtindex);
    if ((npht == 0) || (nctl == 0))
        return false;
    double* ma = new double[4*npht*(12+3*npht)];
    double* l = new double[4*npht];
    double* s = new double[4*npht];
    double* orient = new double[12];
    double* points = new double[3*npht];
    memset(orient, 0, sizeof(double)*12);
    int i, j;

    int* matchedindex = new int[npht];
    memset(matchedindex, 0, sizeof(int)*npht);
    double* ctl = new double[nctl*3];
    int nmatched = 0;
    for (i = 0, j = 0; i<npht && j<nctl;)
    {
        if (phtindex[i] == ctlindex[j])
        {
            points[i*3] = phtdata[i*3+1]*1000;
            points[i*3+1] = phtdata[i*3]*1000;
            points[i*3+2] = phtdata[i*3+2]*1000;
            matchedindex[i] = phtindex[i];
            ++nmatched;
            ++i;
            ++j;
        }
        if (phtindex[i] < ctlindex[j])
        {
            ++i;
        }
        else
            ++j;
    }
    for (int i = 0; i < nmatched; ++i)
    {
        orient[0] += points[i*3];
        orient[1] += points[i*3+1];
        orient[2] += points[i*3+2];
    }
    orient[0] /= nmatched;
    orient[1] /= nmatched;
    orient[2] = orient[2]/nmatched + 1e3*focus;
    orient[3] = orient[4] = orient[5] = 0.0;
    memcpy(orient+6, orient, sizeof(double)*6);
 
    double defaultxyz[3];
    memset(defaultxyz, 0, sizeof(double)*3);
    for (i = 0; i < nmatchedpoints; ++i)
    {
        defaultxyz[0] += points[i*3];
        defaultxyz[1] += points[i*3+1];
        defaultxyz[2] += points[i*3+2];
    }
    defaultxyz[0] /= nmatched;
    defaultxyz[1] /= nmatched;
    defaultxyz[2] /= nmatched;
    for (i = 0, j = 0; i < npht; ++i)
    {
        if (phtindex[i] == matchedindex[j])
        {
//            points[i*3] = pctl->m_points[matchedindex[i]].second.y * 1000;
//            points[i*3+1] = pctl->m_points[matchedindex[i]].second.x * 1000;
//            points[i*3+2] = pctl->m_points[matchedindex[i]].second.z * 1000;
            memset(points+i*3, 0, 3*sizeof(double));
            ++j;
        }
        else
        {
            points[i*3] = defaultxyz[0];
            points[i*3+1] = defaultxyz[1];
            points[i*3+2] = defaultxyz[2];
        }
        ++i;
    }

    int itn = 0;
    int maxitn = 30;
    do
    {
        memset(a, 0, sizeof(double)*4*npht*(12+3*npht));
        memset(l, 0, sizeof(double)*4*npht);
        for (i = 0; i < npht; ++i)
        {
            ma(a, points, phtdata, focus, orient, npht, i);           
            if (matchedindex[i] != 0)
            {
                memset(l+i*4, 0, sizeof(double)*4);
            }
            else
            {
                l[i*4] = phtdata[4*i] + f * (a1(orient)*(ctldata[3*i]-orient[0]) 
                                             + b1(orient)*(ctldata[3*i+1]-orient[1]) 
                                             + c1(orient)*(ctldata[3*i+2]-orient[2])
                                            ) / z_(orient, ctldata, i);
                l[i*4+1] = phtdata[4*i+1] 
                           + f * (a2(orient)*(ctldata[3*i]-orient[0])
                                  + b2(orient)*(ctldata[3*i+1]-orient[1]) 
                                  + c2(orient)*(ctldata[3*i+2]-orient[2])
                                 ) / z_(orient, ctldata, i);
                l[i*4+2] = phtdata[4*i] + f * (a1(orient+6)*(ctldata[3*i]-orient[6]) 
                                             + b1(orient+6)*(ctldata[3*i+1]-orient[7]) 
                                             + c1(orient+6)*(ctldata[3*i+2]-orient[8])
                                            ) / z_(orient+6, ctldata, i);
                l[i*4+3] = phtdata[4*i+1] 
                           + f * (a2(orient+6)*(ctldata[3*i]-orient[6])
                                  + b2(orient+6)*(ctldata[3*i+1]-orient[7]) 
                                  + c2(orient+6)*(ctldata[3*i+2]-orient[8])
                                 ) / z_(orient+6, ctldata, i);
            }
        }
    } while(!exact(l, 12+3*(npht-nmatched)) && (itn < maxit));
    delete []ma;
    delete []l;
    delete []s;
    delete []phtdata;
    delete []phtindex;
    delete []ctldata;
    delete []ctlindex;
}

double Onestep::z_(double* o, double* ctl, int i)
{
    return a3(o)*(ctl[3*i] - o[0])
        + b3(o)*(ctl[3*i+1] - o[1])
        + c3(o)*(ctl[3*i+2] - o[2]);
}

void Onestep::ma(double* a, int* mi, double* pht, double f, double* o,int np, int i)
{
    int nc = 12+np*3;     // number of columns of matrix a
    // left photo
    a[0] = (a1(o)*f + a3(o)*pht[i*4]) / z_(o, ctl, i);
    a[1] = (b1(o)*f + b3(o)*pht[i*4]) / z_(o, ctl, i);
    a[2] = (c1(o)*f + c3(o)*pht[i*4]) / z_(o, ctl, i);
    a[3] = pht[i*4+1]*sin(o[4]) - 
        (pht[i*4]*(pht[i*4]*cos(o[5])-pht[i*4+1]*sin(o[5]))/f
         + f*cos(o[5])) * cos(o[4]);
    a[4] = -f*sin(o[5]) - 
        pht[i*4] * (pht[i*4]*sin(o[5]) + pht[i*4+1]*cos(o[5])) / f;
    a[5] = pht[i*4+1];
    a[nc] = (a2(o)*f + a3(o)*pht[i*4+1]) / z_(o, ctl, i);
    a[nc+1] = (b2(o)*f + b3(o)*pht[i*4+1]) / z_(o, ctl, i);
    a[nc+2] = (c2(o)*f + c3(o)*pht[i*4+1]) / z_(o, ctl, i);
    a[nc+3] = -pht[i*4]*sin(o[4]) - (pht[i*4+1]*(pht[i*4]*cos(o[5])
                                              -pht[i*4+1]*sin(o[5]))/f 
                                  -f*sin(o[5]))*cos(o[4]);
    a[nc+4] = -f*cos(o[5]) - pht[i*4+1] * (pht[i*4]*sin(o[5]) + pht[i*4+1]*cos(o[5])) / f;
    a[nc+5] = -pht[i*4];

    // right photo
    a[2*nc+0] = (a1((o+6))*f + a3((o+6))*pht[i*4+2]) / z_((o+6), ctl, i);
    a[2*nc+1] = (b1((o+6))*f + b3((o+6))*pht[i*4+2]) / z_((o+6), ctl, i);
    a[2*nc+2] = (c1((o+6))*f + c3((o+6))*pht[i*4+2]) / z_((o+6), ctl, i);
    a[2*nc+3] = pht[i*4+2+1]*sin((o+6)[4]) - 
        (pht[i*4+2]*(pht[i*4+2]*c(o+6)s((o+6)[5])-pht[i*4+2+1]*sin((o+6)[5]))/f
         + f*c(o+6)s((o+6)[5])) * c(o+6)s((o+6)[4]);
    a[2*nc+4] = -f*sin((o+6)[5]) 
        - pht[i*4+2] * (pht[i*4+2]*sin((o+6)[5])
                        + pht[i*4+2+1]*c(o+6)s((o+6)[5])) / f;
    a[2*nc+5] = pht[i*4+2+1];
    a[3*nc] = (a2((o+6))*f + a3((o+6))*pht[i*4+2+1]) / z_((o+6), ctl, i);
    a[3*nc+1] = (b2((o+6))*f + b3((o+6))*pht[i*4+2+1]) / z_((o+6), ctl, i);
    a[3*nc+2] = (c2((o+6))*f + c3((o+6))*pht[i*4+2+1]) / z_((o+6), ctl, i);
    a[3*nc+3] = -pht[i*4+2]*sin((o+6)[4]) 
                - (pht[i*4+2+1]*(pht[i*4+2]*c(o+6)s((o+6)[5])
                                 - pht[i*4+2+1]*sin((o+6)[5]))/f 
                   -f*sin((o+6)[5]))*c(o+6)s((o+6)[4]);
    a[3*nc+4] = -f*c(o+6)s((o+6)[5]) 
                - pht[i*4+2+1] * (pht[i*4+2]*sin((o+6)[5]) 
                                  + pht[i*4+2+1]*c(o+6)s((o+6)[5])) / f;
    a[3*nc+5] = -pht[i*4+2];
    // unknow points.  Matrix B
    if (mi[i] == 0)
    {
        a[12+3*(i-1)] = -a[0];
        a[12+3*(i-1)+1] = -a[1];
        a[12+3*(i-1)+2] = -a[2];
        a[nc+12+3*(i-1)] = -a[nc];
        a[nc+12+3*(i-1)+1] = -a[nc+1];
        a[nc+12+3*(i-1)+2] = -a[nc+2];
        a[2*nc+12+3*(i-1)] = -a[2*nc];
        a[2*nc+12+3*(i-1)+1] = -a[2*nc+1];
        a[2*nc+12+3*(i-1)+2] = -a[2*nc+2];
        a[3*nc+12+3*(i-1)] = -a[3*nc];
        a[3*nc+12+3*(i-1)+1] = -a[3*nc+1];
        a[3*nc+12+3*(i-1)+2] = -a[3*nc+2];
    }
}

#endif
