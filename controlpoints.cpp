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

int ControlPoints::data(map<int, Point>* ctl) const
{
    if (ctl == 0)
        return 0;
    map<int, Point>::const_iterator itc;
    int i;
    for (i = 0, itc = m_points.begin(); 
         i < m_pointNum && itc!=m_points.end();
         ++i)
    {
        Point p;
        p.key = itc->second.key;
        p.x = itc->second.y * 1000;
        p.y = itc->second.x * 1000;
        p.z = itc->second.z * 1000;
        ctl->insert(make_pair(itc->first, p));
        ++itc;
    }

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
        is >> p.key >> p.x >> p.y >> p.z;
        cp.m_points.insert(make_pair(p.key, p));
    }
    return is;
}
