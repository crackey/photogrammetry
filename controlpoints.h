#ifndef CONTROLPOINTS_H
#define CONTROLPOINTS_H

#include <map>
#include <iostream>

using namespace std;

#include "globaldefn.h"

class ControlPoints
{
    friend class ControlPointsModel;
    friend ostream& operator<<(ostream& os, const ControlPoints& cp);
    friend istream& operator>>(istream& is, ControlPoints& cp);
public:
    ControlPoints();
    ~ControlPoints();

    size_t count() const;
    int data(double** xyzval = 0, int** index = 0) const;
    int data(map<int, Point>* ctl, vector<int>* index = 0) const;
 
//private:
    int m_pointNum;
    std::map<int, Point> m_points;
};

#endif
