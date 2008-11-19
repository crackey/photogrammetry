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

int PhotoPoints::data(int flag, double* focus, double** xyval, int** index) const
{
    int np = 0;        // number of points retrived.
    *focus = m_focus;
    map<int, PhotoPoint>::const_iterator itp;
    int i;
    if (xyval != 0)  // retrive x,y values
    {
        double* phtdata;
        np = m_points.size();
        switch (flag)
        {
        case Left:
            phtdata = new double[2*np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*2] = itp->second.x1 - 100;
                phtdata[i*2+1] = 100 - itp->second.y1;
            } 
            break;
        case Right:
            phtdata = new double[2*np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*2] = itp->second.x1-100 - itp->second.x2;
                phtdata[i*2+1] = 100-itp->second.y1 + 10 - itp->second.y2;
            } 
            break;
        case Left | Right:
            phtdata = new double[4*np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*4+0] = itp->second.x1 - 100;
                phtdata[i*4+1] = 100 - itp->second.y1;
                phtdata[i*4+2] = phtdata[i*4] - itp->second.x2;
                phtdata[i*4+3] = phtdata[i*4+1] + 10 - itp->second.y2;
            } 
            break;
        default:
            break;
        }
        *xyval = phtdata;
    }
    if (index != 0)
    {
        np = m_points.size();
        int* key = new int[np];
        for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
        {
            key[i] = itp->first;
        }
        *index = key;
    }
    return np;
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

