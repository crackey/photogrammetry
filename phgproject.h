#ifndef PHGPROJECT_H
#define PHGPROJECT_H

#include <map>
#include <QObject>

class QString;
class PhotoPoints;
class ControlPoints;

using namespace std;

class PHGProject : public QObject
{
    Q_OBJECT
public:
    PHGProject(QObject* parent = 0);
    ~PHGProject();
    
    PhotoPoints* photoPoints(QString key);
    ControlPoints* controlPoints(QString key);
    QString curPhotoPoints();
    QString curControlPoints();

public slots:
    void openfile(QString filepath);
    void setCurPhotoPoints(QString filepath);
    void setCurControlPoints(QString filepath);
    void setProjectStatus(QString filepath);
    void setFiducial(QString key, double* fid);
    
signals:
    void fileLoaded(QString filepath);
    void phtAvailable(QString filepath);
    void backwardAvailable(bool);
    void ctlAvailable(QString filepath);

private:
    map<QString, PhotoPoints*> m_pht;
    map<QString, ControlPoints*> m_ctl;
    QString m_curPhotoPoints;
    QString m_curControlPoints;
};

#endif
