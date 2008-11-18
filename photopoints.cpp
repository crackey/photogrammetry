#include "photopoints.h"

PhotoPoints::PhotoPoints(QObject* parent)
: QObject(parent)
{
    m_pointNum = 0;
    m_focus = 0;
}

PhotoPoints::~PhotoPoints()
{}

size_t PhotoPoints::count() const
{
    return m_pointNum;
}

ostream& operator<<(ostream& os, const PhotoPoints& pht)
{
    return os;
}

istream& operator>>(istream& is, PhotoPoints& pht)
{
    is >> pht.m_pointNum >> pht.m_focus;
//    vector<Point> p;
//    p.push_back(*(new Point));
//    p.push_back(*(new Point));

    for (int i = 0; is && (i < pht.m_pointNum); ++i)
    {
        PhotoPoint p;
        int key;
        is >> key >> p.x1 >> p.y1 >> p.x2 >> p.y2;
        pht.m_points.insert(make_pair(key, p));
    }
    return is;
}

void PhotoPoints::setTfx(double val)
{
    if (val != m_fiducial[0])
    {
        m_fiducial[0] = val;
        emit tfxChanged(val);
    }
}

void PhotoPoints::setTfy(double val)
{
    if (val != m_fiducial[1])
    {
        m_fiducial[1] = val;
        emit tfyChanged(val);
    }
}

void PhotoPoints::setRfx(double val)
{
    if (val != m_fiducial[2])
    {
        m_fiducial[2] = val;
        emit rfxChanged(val);
    }
}

void PhotoPoints::setRfy(double val)
{
    if (val != m_fiducial[3])
    {
        m_fiducial[3] = val;
        emit rfyChanged(val);
    }
}

double PhotoPoints::f() const
{
    return m_focus;
}

