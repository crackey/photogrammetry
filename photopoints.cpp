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
    vector<Point> p;
    p.push_back(*(new Point));
    p.push_back(*(new Point));

    for (int i = 0; is && (i < pht.m_pointNum); ++i)
    {
        is >> p[0].key >> p[0].x >> p[0].y >> p[1].x >> p[1].y;
        p[0].z = p[1].z = -pht.m_focus;
        p[1].key = p[0].key;
        pht.m_points.insert(make_pair(p[0].key, p));
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
