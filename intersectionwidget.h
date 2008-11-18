#ifndef INTERSECTIONWIDGET_H
#define INTERSECTIONWIDGET_H

#include <QWidget>
#include <QObject>

#include "ui_intersectionwidgetbase.h"

class Intersection;

class IntersectionWidget: public QWidget
{
    Q_OBJECT
public:
    IntersectionWidget(Intersection* ints = 0, QWidget* parent = 0);
    ~IntersectionWidget();

public slots:
    void setIntersection(Intersection* ints);
    void updateForward();
    void updateBackward();

private:
    Ui::IntersectionWidgetBase ui;
    Intersection* m_intersection;
};

#endif
