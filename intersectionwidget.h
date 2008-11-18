#ifndef INTERSECTIONWIDGET_H
#define INTERSECTIONWIDGET_H

#include <QWidget>

#include "ui_intersectionwidgetbase.h"

class Intersection;

class IntersectionWidget: public QWidget
{
public:
    IntersectionWidget(Intersection* ints, QWidget* parent = 0);
    ~IntersectionWidget();

private:
    Ui::IntersectionWidgetBase ui;
    Intersection* m_intersection;
};

#endif
