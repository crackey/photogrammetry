#ifndef TRANSFORM_H
#define TRANSFORM_H

double a1(double* orient);
double a2(double* orient);
double a3(double* orient);
double b1(double* orient);
double b2(double* orient);
double b3(double* orient);
double c1(double* orient);
double c2(double* orient);
double c3(double* orient);
void transform(double* r, double* o);

void transform(double* r, double phi, double omega, double kappa);
double a1(double phi, double omega, double kappa);
double a2(double phi, double omega, double kappa);
double a3(double phi, double omega, double kappa);
double b1(double phi, double omega, double kappa);
double b2(double phi, double omega, double kappa);
double b3(double phi, double omega, double kappa);
double c1(double phi, double omega, double kappa);
double c2(double phi, double omega, double kappa);
double c3(double phi, double omega, double kappa);

#endif
