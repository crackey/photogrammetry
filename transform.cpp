#include <cmath>

#include "transform.h"

using namespace std;

void transform(double *r, double* o)
{
    r[0] = a1(o);
    r[1] = a2(o);
    r[2] = a3(o);
    r[3] = b1(o);
    r[4] = b2(o);
    r[5] = b3(o);
    r[6] = c1(o);
    r[7] = c2(o);
    r[8] = c3(o);
}

double a1(double* orient)
{
    return cos(orient[3])*cos(orient[5]) - sin(orient[3])*sin(orient[4])*sin(orient[5]);
}

double a2(double* orient)
{
    return -cos(orient[3])*sin(orient[5]) - sin(orient[3])*sin(orient[4])*cos(orient[5]);
}

double a3(double* orient)
{
    return -sin(orient[3])*cos(orient[4]);
}

double b1(double* orient)
{
    return cos(orient[4])*sin(orient[5]);
}

double b2(double* orient)
{
    return cos(orient[4])*cos(orient[5]);
}

double b3(double* orient)
{
    return -sin(orient[4]);
}

double c1(double* orient)
{
    return sin(orient[3])*cos(orient[5]) + cos(orient[3])*sin(orient[4])*sin(orient[5]);
}

double c2(double* orient)
{
    return -sin(orient[3])*sin(orient[5]) + cos(orient[3])*sin(orient[4])*cos(orient[5]);
}

double c3(double* orient)
{
    return cos(orient[3])*cos(orient[4]);
}

