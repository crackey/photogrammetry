#include <cmath>

#include "transform.h"

using namespace std;

void transform(double* r, double phi, double omega, double kappa)
{
    r[0] = a1(phi, omega, kappa);
    r[1] = a2(phi, omega, kappa);
    r[2] = a3(phi, omega, kappa);
    r[3] = b1(phi, omega, kappa);
    r[4] = b2(phi, omega, kappa);
    r[5] = b3(phi, omega, kappa);
    r[6] = c1(phi, omega, kappa);
    r[7] = c2(phi, omega, kappa);
    r[8] = c3(phi, omega, kappa);
}

double a1(double phi, double omega, double kappa)
{
    return cos(phi)*cos(kappa) - sin(phi)*sin(omega)*sin(kappa);
}

double a2(double phi, double omega, double kappa)
{
    return -cos(phi)*sin(kappa) - sin(phi)*sin(omega)*cos(kappa);
}

double a3(double phi, double omega, double kappa)
{
    return -sin(phi)*cos(omega);
}

double b1(double phi, double omega, double kappa)
{
    return cos(omega)*sin(kappa);
}

double b2(double phi, double omega, double kappa)
{
    return cos(omega)*cos(kappa);
}

double b3(double phi, double omega, double kappa)
{
    return -sin(omega);
}

double c1(double phi, double omega, double kappa)
{
    return sin(phi)*cos(kappa) + cos(phi)*sin(omega)*sin(kappa);
}

double c2(double phi, double omega, double kappa)
{
    return -sin(phi)*sin(kappa) + cos(phi)*sin(omega)*cos(kappa);
}

double c3(double phi, double omega, double kappa)
{
    return cos(phi)*cos(omega);
}





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

