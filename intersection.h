#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <QObject>

class PhotoPoints;
class ControlPoints;

class Intersection : public QObject
{
    Q_OBJECT
public:
    Intersection(QString ctl, QString pht, QObject* parent = 0);
    ~Intersection();

//    int backward(double* pht, double* ctl, double* result, double f, int n);
//    int forward(double* p, double* o, double* out, int n);
    void setControl(double* pd);
    void setLeftPhoto(double* pd);
    void setRightPhoto(double* pd);
    bool forward();
    bool backward();
    int forwardResult(int** index, double** result);
    double const* orient() const;

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
    int backward_impl(double* pht, double* ctl, double* orient, double f, int n);
    int forward_impl(double* p , double* o, double* out, int n);
    int backwardData(double** ppht, double** pctl, double* focus, int p);
    bool controlData();

    double m_orient[12]; // orient elements, 0-5 for left photo, 6-11 for right
//    double m_ro[6]; // orient elements of right photo
    QString m_pht;
    double* m_forwardResult;
    int *m_index;
    int m_numPhtPt;
    QString m_ctl;
};

#endif
