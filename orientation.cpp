#include <QObject>

#include "phgproject.h"
#include "photopoints.h"
#include "controlpoints.h"
#include "transform.h"
#include "orientation.h"

Orientation::Orientaion(QString ctl, QString pht, QObject* parent)
    : QObject(parent)
{
    memset(m_orient, 0, sizeof(double)*12);
}

Orientation::~Orientaion()
{}

bool Orientation::relative()
{

}

bool Orientation::absolute()
{
}

int Orientation::photoData(double** data)
{
    int np = 0; 
    PHGProject* prj = (PHGProject*)parent();
    PhotoPoints* pht = prj->photoPoints(m_pht);

    map<int, vector<Point> > *pht = &tpht->m_points;
    map<int, vector<Point> >::iterator itp;
    np = pht->size();
    double* phtdata = new double[4*np];
    int i = 0;
    for (itp = pht->begin(); itp != pht->end(); ++itp, ++i)
    {
        phtdata[i*4+0] = itp->second.
    } 
}

