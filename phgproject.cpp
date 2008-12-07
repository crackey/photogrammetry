#include <fstream>
#include <QtGui>
#include <QtGlobal>

#include "phgproject.h"
#include "photopoints.h"
#include "controlpoints.h"
#include "intersection.h"
#include "orientation.h"
#include "onestep.h"

using namespace std;

PHGProject::PHGProject(QObject* parent)
: QObject(parent)
{
    m_pht = 0;
    m_ctl = 0;
    m_intersection = 0;
    m_orientation = 0;
    m_onestep = 0;
    connect(this, SIGNAL(fileLoaded(QString)), this, SLOT(setCurPhotoPoints(QString)));
    connect(this, SIGNAL(fileLoaded(QString)), this, SLOT(setCurControlPoints(QString)));
    connect(this, SIGNAL(fileLoaded(QString)), this, SLOT(setProjectStatus(QString)));
    connect(this, SIGNAL(backwardFinished(bool)), this, SIGNAL(forwardAvailable(bool)));
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
        if (m_ctl != 0)
            delete m_ctl;
        m_ctl = pctl;
        m_curControlPoints = filepath;
        emit fileLoaded(filepath);
    }
    else if (filepath.endsWith(".pht", Qt::CaseInsensitive))
    {
        PhotoPoints *ppht = new PhotoPoints;
        in >> *ppht;
        if (m_pht != 0)
            delete m_pht;
        m_pht = ppht;
        m_curPhotoPoints = filepath;
        emit fileLoaded(filepath);
        int f = QMessageBox::question((QWidget*)this->parent(), tr("框标距"),
            tr("是否从文件读入框标距？"), 
            QMessageBox::Yes | QMessageBox::No);
        double fdl[4] = {1.0, 1.0, 1.0, 1.0};
        if (f == QMessageBox::Yes)
        { 
            QString fdlfile = QFileDialog::getOpenFileName((QWidget*)this->parent(), tr("打开"), ".", 
                tr("框标距文件 (*.fdl)"));
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
    else
    {}
}

PhotoPoints* PHGProject::photoPoints(QString key)
{
    return m_pht;
}

ControlPoints* PHGProject::controlPoints(QString key)
{
    return m_ctl;
}

Intersection* PHGProject::intersection(QString key)
{
    return m_intersection;
}

QString PHGProject::curPhotoPoints()
{
    return m_curPhotoPoints;
}

QString PHGProject::curControlPoints()
{
    return m_curControlPoints;
}

QString PHGProject::curIntersection()
{
    return m_curIntersection;
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
    if (!m_curPhotoPoints.isEmpty())
    {
        emit relativeAvailable(true);
    }
    if (!m_curControlPoints.isEmpty() && !m_curPhotoPoints.isEmpty())
    {
        emit backwardAvailable(true);
        emit onestepAvailable(true);
    }
}

void PHGProject::addIntersection(QString key)
{}

void PHGProject::forwardIntersection()
{
    if (m_intersection != 0 && m_intersection->forward())
    {
        emit forwardFinished(true);
    }
    else
        emit forwardFinished(false);
}

void PHGProject::backwardIntersection()
{
    QString key = m_curControlPoints + m_curPhotoPoints;
    if (m_intersection != 0)
        delete m_intersection;

    m_intersection = new Intersection(m_curControlPoints, m_curPhotoPoints, this);
    if (m_intersection->backward())
    {
        m_curIntersection = key;
        emit backwardFinished(true);
    }
    else
        emit backwardFinished(false);

}

void PHGProject::relativeOrientation()
{
    QString key = m_curControlPoints + m_curPhotoPoints;

    if (m_orientation != 0)
        delete m_orientation;

    m_orientation = new Orientation(m_curControlPoints, m_curPhotoPoints, this);
    if (m_orientation->relative())
    {
        m_curOrientation = key;
        emit relativeFinished(true);
    }
    else
        emit relativeFinished(false);
}

void PHGProject::absoluteOrientation()
{
    if (m_orientation != 0 && m_orientation->absolute())
    {
        emit absoluteFinished(true);
        return;
    }
    emit absoluteFinished(false);

}

void PHGProject::onestep()
{
    QString key = m_curControlPoints + m_curPhotoPoints;
    if (m_onestep != 0)
        delete m_onestep;
    Onestep* m_onestep = new Onestep(m_curControlPoints, m_curPhotoPoints, this);
    if (m_onestep->calculate())
    {
        m_curOnestep = key;
        emit onestepFinished(true);
    }
    else 
        emit onestepFinished(false);
}
