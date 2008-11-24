#ifndef PHGPROJECT_H
#define PHGPROJECT_H

#include <map>
#include <QObject>

class QString;
class PhotoPoints;
class ControlPoints;
class Intersection;
class Orientation;
class Onestep;

using namespace std;

class PHGProject : public QObject
{
    Q_OBJECT
public:
    PHGProject(QObject* parent = 0);
    ~PHGProject();
    
    PhotoPoints* photoPoints(QString key);
    ControlPoints* controlPoints(QString key);
    Intersection* intersection(QString key);
    QString curPhotoPoints();
    QString curControlPoints();
    QString curIntersection();

public slots:
    void openfile(QString filepath);
    void setCurPhotoPoints(QString filepath);
    void setCurControlPoints(QString filepath);
    void setProjectStatus(QString filepath);
    void setFiducial(QString key, double* fid);
    void addIntersection(QString key);
    void forwardIntersection();
    void backwardIntersection();
    void relativeOrientation();
    void absoluteOrientation();
    void onestep();
    
signals:
    void fileLoaded(QString filepath);
    void phtAvailable(QString filepath);
    void backwardAvailable(bool);
    void forwardAvailable(bool);
    void onestepAvailable(bool);
    void ctlAvailable(QString filepath);
    void backwardFinished(bool);
    void forwardFinished(bool);
    void relativeAvailable(bool);
    void relativeFinished(bool);
    void absoluteAvailable(bool);
    void absoluteFinished(bool);
    void onestepFinished(bool);

private:
    map<QString, PhotoPoints*> m_pht;
    map<QString, ControlPoints*> m_ctl;
    map<QString, Intersection*> m_intersection;
    map<QString, Orientation*> m_orientation;
    map<QString, Onestep*> m_onestep;
    QString m_curOrientation;
    QString m_curIntersection;
    QString m_curOnestep;
    QString m_curPhotoPoints;
    QString m_curControlPoints;
};

#endif

