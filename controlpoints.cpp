#include <fstream>

#include "controlpoints.h"

using namespace std;

ControlPoints::ControlPoints()
{
    m_pointNum = 0;
}

ControlPoints::~ControlPoints()
{
}

ostream& operator<<(ostream& os, const ControlPoints& cp)
{
    return os;
}

istream& operator>>(istream& is, ControlPoints& cp)
{
    Point p;
    is >> cp.m_pointNum;
    for (int i = 0; is && (i < cp.m_pointNum); ++i)
    {
        int key;
        is >> key >> p.x >> p.y >> p.z;
        cp.m_points.insert(make_pair<int, Point>(key, p));
    }
    return is;
}
