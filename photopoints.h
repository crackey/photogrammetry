#ifndef PHOTOPOINTS_H
#define PHOTOPOINTS_H

#include <iostream>
#include <map>
#include <vector>
#include <QObject>

#include "globaldefn.h"

using namespace std;

class PhotoPoints : public QObject
{
    Q_OBJECT
    friend ostream& operator<<(ostream& os, const PhotoPoints& pht);
    friend istream& operator>>(istream& is, PhotoPoints& pht);
    friend class PhotoPointsModel;
public:
    PhotoPoints(QObject* parent = 0);
    ~PhotoPoints();

    size_t count() const;
                                                                  
public slots:
    void setRfx(double val);
    void setRfy(double val);
    void setTfx(double val);
    void setTfy(double val);

signals:
    void rfxChanged(double val);
    void rfyChanged(double val);
    void tfxChanged(double val);
    void tfyChanged(double val);
//private:
public:
    int m_pointNum;
    double m_focus;
    double m_fiducial[4];
    map<int, vector<Point> > m_points;
};

#endif
