#ifndef CONTROLPOINTS_H
#define CONTROLPOINTS_H

#include <map>
#include <iostream>

using namespace std;

#include "globaldefn.h"

class ControlPoints
{
    friend class ControlPointsModel;
public:
    ControlPoints();
    ~ControlPoints();

    size_t count()
    {
        return m_pointNum;
    }
    friend ostream& operator<<(ostream& os, const ControlPoints& cp);
    friend istream& operator>>(istream& is, ControlPoints& cp);
//private:
    int m_pointNum;
    std::map<int, Point> m_points;
};

#endif
