#ifndef ORIENTATION_H
#define ORIENTATTION_H

#include <map>
#include <vector>
#include <QObject>

#include "globaldefn.h"

using namespace std;

class QString;

class Orientation : public QObject
{
    Q_OBJECT
public:
    Orientation(QString ctl, QString pht, QObject* parent = 0);
    ~Orientation();

    int result(map<int, Point>* point) const;
    int relativeOrientElements(vector<double>* o, vector<double>* s) const;
    int absoluteOrientElements(vector<double>* o, vector<double>* s) const;
    bool relative();
    bool absolute();

private:
    bool exact(double* data);
    vector<double> m_orient; // 0-5 was orient elements for left photo which
                         // should normally be 0, 6-11 for right photo
    double m_limit;
    vector<double> m_rol;    // relative orient elements.
    vector<double> m_rols;   // relative orient elements residual.
    vector<double> m_aol;    // absolute orient elements.
    vector<double> m_aols;
    map<int, Point> m_modelpoints;
    map<int, Point> m_result;
    int m_phtnum;
    QString m_pht;
    QString m_ctl;
};

#endif

