#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <QObject>

class PhotoPoints;
class ControlPoints;

class Intersection : public QObject
{
    Q_OBJECT
public:
    Intersection(QObject* parent = 0);
    ~Intersection();

    int backward(double* pht, double* ctl, double* result, double f, int n);
    int forward(double* p, double* o, double* out, int n);
    void setControl(double* pd);
    void setLeftPhoto(double* pd);
    void setRightPhoto(double* pd);

private:
    double a1(double* orient);
    double a2(double* orient);
    double a3(double* orient);
    double b1(double* orient);
    double b2(double* orient);
    double b3(double* orient);
    double c1(double* orient);
    double c2(double* orient);
    double c3(double* orient);
    double z_(double* orient, double* ctl, int index);
    void ma(double* a, double* ctl, double* pht, double f, double* orient, int index);
    bool orientNotExact(double* orient);
    int backward(double* pht, double* ctl, double* orient, double f, int n);
    int forward(double* p , double* o, double* out, int n);

    double m_lo[6]; // orient elements of left photo
    double m_ro[6]; // orient elements of right photo
    double* m_ctld; // control points data
    double* m_lphotod;
    double* m_rphotod;
    PhotoPoints* m_pht;
    ControlPoints* m_ctl;
};

#endif
