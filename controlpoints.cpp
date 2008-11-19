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

size_t ControlPoints::count() const
{
    return m_pointNum;
}

int ControlPoints::data(double** xyzval, int** index) const
{
    int np = 0;            // number of points retrived
    map<int, Point>::const_iterator it;
    int i = 0;
    if (xyzval != 0)
    {
        np = m_points.size();
        double* data = new double[3*np];
        for (i = 0, it = m_points.begin(); it != m_points.end(); ++it, ++i)
        {
            data[i*3+0] = it->second.x;
            data[i*3+1] = it->second.y;
            data[i*3+2] = it->second.z;
        }
        *xyzval = data;
    }
    if (index != 0)
    {
        np = m_points.size();
        int* key = new int[np];
        for (i = 0, it = m_points.begin(); it != m_points.end(); ++it, ++i)
        {
            key[i] = it->first;
        }
        *index = key;
    }
    return np;
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
