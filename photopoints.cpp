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

int PhotoPoints::data(double* focus, map<int, PhotoPoint>* pht) const
{
    int np = 0;        // number of points retrived.
    if (focus != 0)
        *focus = m_focus;

    if (pht == 0)
        return 0;
    int i;
    np = m_points.size();
    map<int, PhotoPoint>::const_iterator itp;
    for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
    {
        PhotoPoint p;
        p.key = itp->second.key;
        p.x1 = itp->second.x1 - 100;
        p.y1 = 100 - itp->second.y1;
        p.x2 = p.x1 - itp->second.x2;
        p.y2 = p.y1 + 10 - itp->second.y2;
        pht->insert(make_pair(itp->first, p));
    } 

    return np;
}

int PhotoPoints::data(int flag, double* focus, double** xyval, int** index) const
{
    int np = 0;        // number of points retrived.
    if (focus != 0)
        *focus = m_focus;
    map<int, PhotoPoint>::const_iterator itp;
    int i;
    if (xyval != 0)  // retrive x,y values
    {
        double* phtdata;
        double scalex;
        double scaley;
        scalex = m_fiducial[2]/m_fiducial[0];
        scaley = m_fiducial[3]/m_fiducial[1];
        np = m_points.size();
        switch (flag)
        {
        case Left:
            phtdata = new double[2*np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*2] = (itp->second.x1 - 100)*scalex;
                phtdata[i*2+1] = (100 - itp->second.y1)*scaley;
            } 
            break;
        case Right:
            phtdata = new double[2*np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*2] = (itp->second.x1-100 - itp->second.x2)*scalex;
                phtdata[i*2+1] = (100-itp->second.y1 + 10 - itp->second.y2)*scaley;
            } 
            break;
        case Left | Right:
            phtdata = new double[4*np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*4+0] = (itp->second.x1 - 100)*scalex;
                phtdata[i*4+1] = (100 - itp->second.y1)*scaley;
                phtdata[i*4+2] = (itp->second.x1-100 - itp->second.x2)*scalex;
                phtdata[i*4+3] = (100-itp->second.y1 + 10 - itp->second.y2)*scaley;
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

#if 0
int PhotoPoints::data(int flag, double* fodus, PhotoPoint** pdata) const
{
    int np = 0;        // number of points retrived.
    if (focus != 0)
        *focus = m_focus;
    map<int, PhotoPoint>::const_iterator itp;
    int i;
    if (pdata != 0)  // retrive x,y values
    {
        PhotoPoint* phtdata;
        double scalex;
        double scaley;
        scalex = m_fiducial[2]/m_fiducial[0];
        scaley = m_fiducial[3]/m_fiducial[1];
        np = m_points.size();
        switch (flag)
        {
        case Left:
            phtdata = new PhotoPoint[np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*2] = (itp->second.x1 - 100)*scalex;
                phtdata[i*2+1] = (100 - itp->second.y1)*scaley;
            } 
            break;
        case Right:
            phtdata = new PhotoPoint[np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*2] = (itp->second.x1-100 - itp->second.x2)*scalex;
                phtdata[i*2+1] = (100-itp->second.y1 + 10 - itp->second.y2)*scaley;
            } 
            break;
        case Left | Right:
            phtdata = new PhotoPoint[2*np];
            for (itp = m_points.begin(), i = 0; itp != m_points.end(); ++itp, ++i)
            {
                phtdata[i*4+0] = (itp->second.x1 - 100)*scalex;
                phtdata[i*4+1] = (100 - itp->second.y1)*scaley;
                phtdata[i*4+2] = (itp->second.x1-100 - itp->second.x2)*scalex;
                phtdata[i*4+3] = (100-itp->second.y1 + 10 - itp->second.y2)*scaley;
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
#endif

ostream& operator<<(ostream& os, const PhotoPoints& pht)
{
    return os;
}

istream& operator>>(istream& is, PhotoPoints& pht)
{
    is >> pht.m_pointNum >> pht.m_focus;

    for (int i = 0; is && (i < pht.m_pointNum); ++i)
    {
        PhotoPoint p;
        is >> p.key >> p.x1 >> p.y1 >> p.x2 >> p.y2;
        pht.m_points.insert(make_pair(p.key, p));
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

