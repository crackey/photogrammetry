#ifndef ONESTEPWIDGET_H
#define ONESTEPWIDGET_H

#include <QWidget>
#include <QObject>

#include "ui_onestepwidgetbase.h"

class Onestep;

class OnestepWidget: public QWidget
{
    Q_OBJECT
public:
    OnestepWidget(Onestep* ints = 0, QWidget* parent = 0);
    ~OnestepWidget();

public slots:
    void setOnestep(Onestep* ints);
    void update();

private:
    Ui::OnestepWidgetBase ui;
    Onestep* m_onestep;
};

#endif
