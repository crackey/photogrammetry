#ifndef PHOTOGRAMMETRY_H
#define PHOTOGRAMMETRY_H

#include <QMainWindow>
#include <QObject>

#include "ui_photogrammetrybase.h"
#include "phgproject.h"

class Photogrammetry : public QMainWindow
{
    Q_OBJECT
public:
    Photogrammetry();
    ~Photogrammetry();

public slots:
    // these slots would be connected automatically.
    void on_fileOpenAction_triggered();
    void on_fileNewAction_triggered();
    void on_fileSaveAction_triggered();
    void on_fileExitAction_triggered();
    void on_orientationRelativeAction_triggered();
    void on_orientationAbsoluteAction_triggered();
    void on_intersectionForwardAction_triggered();
    void on_intersectionBackwardAction_triggered();
    void on_helpAboutAction_triggered();

    // my slots:
    void addView(QString filepath);
    void updateBackwardView(bool t);
    void updateForwardView(bool t);
    void syncFiducial(QString key);

signals:
    void fileopen(QString filepath);
    void forwardAvailable(bool);
    void backwardAvailable(bool);
    void backwardFinished();

private:
#if 0
    void backwardIntersection();
    void forwardIntersection();
    int backwardData(double** ppht, double** pctl, double* focus, int p);
#endif

    Ui::PhotogrammetryBase ui;
    PHGProject *m_prj;
    bool m_orientation;
    bool m_intersection;
    double m_orient[12];
};

#endif
