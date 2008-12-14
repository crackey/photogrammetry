#ifndef ORIENTATIONWIDGET_H
#define ORIENTATIONWIDGET_H

#include <QWidget>
#include <QObject>

#include "ui_orientationwidgetbase.h"

class Orientation;

class OrientationWidget: public QWidget
{
    Q_OBJECT
public:
    OrientationWidget(Orientation* ort = 0, QWidget* parent = 0);
    ~OrientationWidget();

public slots:
    void setOrientation(Orientation* ort);
    void updateAbsolute();
    void updateRelative();

private:
    Ui::OrientationWidgetBase ui;
    Orientation* m_orientation;
};

#endif
