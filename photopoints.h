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
    enum Side { Left = 0x01, Right = 0x02 };

    PhotoPoints(QObject* parent = 0);
    ~PhotoPoints();

    size_t count() const;
    // return focus
    double f() const;
    // get data, xyval and index would not be fetched if equals to 0 
    int data(int flag, double* focus, double** xyval = 0, int** index = 0) const;
    int data(double* focus, map<int, PhotoPoint>* pht) const;
    //int data(double* focus, PhotoPoint** pdata) const;
                                                                  
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
    map<int, PhotoPoint> m_points;
};

#endif
