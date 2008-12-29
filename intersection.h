#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <QObject>
#include <map>

#include "globaldefn.h"

using namespace std;

class PhotoPoints;
class ControlPoints;

class Intersection : public QObject
{
    Q_OBJECT
public:
    Intersection(QString ctl, QString pht, QObject* parent = 0);
    ~Intersection();

    void setControl(double* pd);
    void setLeftPhoto(double* pd);
    void setRightPhoto(double* pd);
    bool forward();
    bool backward();
    int forwardResult(map<int, Point>* result);
    int orient(vector<double>* o, vector<double>* os);

private:
    //double z_(double* orient, double* ctl, int index);
    void backPrepare(double* a, double* l, map<int, Point>& ctl, 
                 map<int, PhotoPoint>& pht, double f, 
                 vector<int>& match, int side);
    bool orientNotExact(double* orient);

    vector<double> m_orient; // orient elements, 0-5 for left photo, 6-11 for right
	vector<double> m_orients; // residual
    QString m_pht;
    map<int, Point> m_forwardResult;
    int m_numPhtPt;
    QString m_ctl;
};

#endif
