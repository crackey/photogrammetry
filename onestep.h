#ifndef ONESTEP_H
#define ONESTEP_H

#include <map>
#include <vector>
#include <vector>
#include <QObject>

#include "globaldefn.h"

using namespace std;

class QString;

class Onestep : public QObject
{
    Q_OBJECT
public:
    Onestep(QString ctl, QString pht, QObject* parent);
    ~Onestep();

    bool calculate();
    int orient(vector<double>* o, vector<double>* os);
    int result(map<int, Point>* result, map<int, Point>* residual);

private:
    bool exact(double* x, int n);
    void prepare(double* a, double* l, map<int, Point>& ctl, 
                 map<int, PhotoPoint>& pht, double f, 
                 map<int, int>& match, int nmatch);
    map<int, Point> m_result;
    map<int, Point> m_result_residual;
    vector<double> m_orient;
    vector<double> m_orient_residual;
    int m_numPoint;
    QString m_ctl;
    QString m_pht;
};

#endif

