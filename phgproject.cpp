#include <fstream>
#include <QtGui>
#include <QtGlobal>

#include "phgproject.h"
#include "photopoints.h"
#include "controlpoints.h"
#include "intersection.h"

using namespace std;

PHGProject::PHGProject(QObject* parent)
: QObject(parent)
{
    connect(this, SIGNAL(fileLoaded(QString)), this, SLOT(setCurPhotoPoints(QString)));
    connect(this, SIGNAL(fileLoaded(QString)), this, SLOT(setCurControlPoints(QString)));
    connect(this, SIGNAL(fileLoaded(QString)), this, SLOT(setProjectStatus(QString)));
}

PHGProject::~PHGProject()
{

}

void PHGProject::openfile(QString filepath)
{
    filepath = QDir::toNativeSeparators(filepath);
    QByteArray ba1 = filepath.toLocal8Bit();
    const char *pw = ba1.data();
    ifstream in(pw);
    if (filepath.endsWith(".ctl", Qt::CaseInsensitive))
    {
        ControlPoints *pctl = new ControlPoints;
        in >> *pctl;
        int ctlsize = m_ctl.size();
        m_ctl.insert(make_pair(filepath, pctl));
        if (ctlsize != m_ctl.size())
            emit fileLoaded(filepath);
    }
    else if (filepath.endsWith(".pht", Qt::CaseInsensitive))
    {
        PhotoPoints *ppht = new PhotoPoints;
        in >> *ppht;
        int phtsize = m_pht.size();
        m_pht.insert(make_pair(filepath, ppht));
        if (phtsize != m_pht.size())
        {
            emit fileLoaded(filepath);
            int f = QMessageBox::question((QWidget*)this->parent(), tr("¿ò±ê¾à"),
                                          tr("Open a fiducial file?"), 
                                          QMessageBox::Yes | QMessageBox::No);
            double fdl[4] = {1.0, 1.0, 1.0, 1.0};
            if (f == QMessageBox::Yes)
            { 
                QString fdlfile = QFileDialog::getOpenFileName((QWidget*)this->parent(), tr("Open File"), ".", 
                                                       tr("Distance of fiducial marks (*.fdl)"));
                if (!filepath.isEmpty())
                {
                    fdlfile = QDir::toNativeSeparators(fdlfile);
                    QByteArray ba1 = fdlfile.toLocal8Bit();
                    const char *pw = ba1.data();
                    ifstream in(pw);
                    string tmpstr;
                    in >> tmpstr >> fdl[0] >> fdl[1] >> tmpstr >> fdl[2] >> fdl[3];
                }
            }
            else if (f == QMessageBox::No)
            {
            }
            setFiducial(filepath, fdl);
        }
    }
    else
    {}
}

PhotoPoints* PHGProject::photoPoints(QString key)
{
    return m_pht.find(key)->second;
}

ControlPoints* PHGProject::controlPoints(QString key)
{
    return m_ctl.find(key)->second;
}

Intersection* PHGProject::intersection(QString key)
{
    return m_intersection.find(key)->second;
}

QString PHGProject::curPhotoPoints()
{
    return m_curPhotoPoints;
}

QString PHGProject::curControlPoints()
{
    return m_curControlPoints;
}

void PHGProject::setCurControlPoints(QString filepath)
{
    if (filepath.endsWith(".ctl"))
    {
        m_curControlPoints = filepath;
        emit ctlAvailable(filepath);
    }
}

void PHGProject::setFiducial(QString key, double* fid)
{
    PhotoPoints* pht = photoPoints(key);
    //memcpy(pht->m_fiducial, fid, 4*sizeof(double));
    pht->setTfx(fid[0]);
    pht->setTfy(fid[1]);
    pht->setRfx(fid[2]);
    pht->setRfy(fid[3]);
}

void PHGProject::setCurPhotoPoints(QString filepath)
{
    if (filepath.endsWith(".pht"))
    {
        m_curPhotoPoints = filepath;
        emit phtAvailable(filepath);
    }
}

void PHGProject::setProjectStatus(QString filepath)
{
    if (!m_curControlPoints.isEmpty() && !m_curPhotoPoints.isEmpty())
    {
        emit backwardAvailable(true);
    }
}

