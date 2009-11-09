#include <QtGui>

#include "globaldefn.h"
#include "controlpoints.h"
#include "photopoints.h"
#include "photogrammetry.h"
#include "projectmodel.h"
#include "photopointsmodel.h"
#include "controlpointsmodel.h"
#include "intersectionwidget.h"
#include "orientationwidget.h"
#include "onestepwidget.h"
//#include "intersection.h"

using namespace std;

Photogrammetry::Photogrammetry()
{
    m_orientation = false;
    m_intersection = false;
    ui.setupUi(this);
    m_prj = new PHGProject(0);

    PhotoPointsModel* phtptsModel = new PhotoPointsModel(0, this);
    ui.photoPointsView->setModel(phtptsModel);
    ControlPointsModel* ctlptsModel = new ControlPointsModel(0, this);
    ui.controlPointsView->setModel(ctlptsModel);

    connect(this, SIGNAL(fileopen(QString)), m_prj, SLOT(openfile(QString)));
    connect(m_prj, SIGNAL(fileLoaded(QString)), this, SLOT(addView(QString)));
    connect(m_prj, SIGNAL(backwardAvailable(bool)), 
            ui.intersectionBackwardAction, SLOT(setEnabled(bool)));
    connect(m_prj, SIGNAL(forwardAvailable(bool)), 
            ui.intersectionForwardAction, SLOT(setEnabled(bool)));
    connect(m_prj, SIGNAL(relativeAvailable(bool)), 
            ui.orientationRelativeAction, SLOT(setEnabled(bool)));
    connect(m_prj, SIGNAL(absoluteAvailable(bool)), 
            ui.orientationAbsoluteAction, SLOT(setEnabled(bool)));
    connect(m_prj, SIGNAL(onestepAvailable(bool)), 
            ui.onestepAction, SLOT(setEnabled(bool)));
    connect(m_prj, SIGNAL(phtAvailable(QString)), this, SLOT(syncFiducial(QString)));
    connect(m_prj, SIGNAL(backwardFinished(bool)), this, SLOT(updateBackwardView(bool)));
    connect(m_prj, SIGNAL(forwardFinished(bool)), this, SLOT(updateForwardView(bool)));
    connect(m_prj, SIGNAL(relativeFinished(bool)), this, SLOT(updateRelativeView(bool)));
    connect(m_prj, SIGNAL(absoluteFinished(bool)), this, SLOT(updateAbsoluteView(bool)));
    connect(m_prj, SIGNAL(onestepFinished(bool)), this, SLOT(updateOnestepView(bool)));
//    connect(this, SIGNAL(backwardAvailable(bool)), ui.intersectionBackwardAction, SLOT(setEnabled(bool)));
//    connect(this, SIGNAL(forwardAvailable(bool)), ui.intersectionForwardAction, SLOT(setEnabled(bool)));
}

Photogrammetry::~Photogrammetry()
{}

// public slots
// auto-connected slots

void Photogrammetry::on_fileOpenAction_triggered()
{
    QString filepath;
    filepath = QFileDialog::getOpenFileName(this, tr("打开"), ".", 
        tr("控制点数据 (*.ctl);;"
           "相片数据 (*.pht);;"));
    if (!filepath.isEmpty())
        emit fileopen(filepath);
}

void Photogrammetry::on_fileNewAction_triggered()
{}

void Photogrammetry::on_fileSaveAction_triggered()
{}

void Photogrammetry::on_fileExitAction_triggered()
{
    close();
}

void Photogrammetry::on_orientationRelativeAction_triggered()
{
    m_prj->relativeOrientation();
}

void Photogrammetry::on_orientationAbsoluteAction_triggered()
{
    m_prj->absoluteOrientation();
}

void Photogrammetry::on_intersectionForwardAction_triggered()
{
    m_prj->forwardIntersection();
}

void Photogrammetry::on_intersectionBackwardAction_triggered()
{
    m_prj->backwardIntersection();
}

void Photogrammetry::on_onestepAction_triggered()
{
    m_prj->onestep();
}

void Photogrammetry::on_helpAboutAction_triggered()
{
    QMessageBox msgBox;
    msgBox.setText(tr("数字摄影测量学作业\n"
                      "\n作者：杨波(oakyangnjucn@gmail.com)\n"
                      "\n版权：This software is in the public domain, "
                      "\ndo with it what you wish."
                      "\nContact the auther to get the source if you want."));
    msgBox.exec(); 
}

// end of auto-connected slots

void Photogrammetry::addView(QString filepath)
{
    if (filepath.endsWith(".pht"))
    {
        static_cast<PhotoPointsModel*>(ui.photoPointsView->model())->setRoot(m_prj->photoPoints(filepath));
    }   
    else if (filepath.endsWith(".ctl"))
    {
        static_cast<ControlPointsModel*>(ui.controlPointsView->model())->setRoot(m_prj->controlPoints(filepath));       
    }
    QTabWidget* tab = ui.tabWidget;
    int n = tab->count();
    tab->setCurrentIndex(0);
    for (int i = 1; i < n; ++i)
    {
        tab->removeTab(1);
    }
}

void Photogrammetry::syncFiducial(QString key)
{
    PhotoPoints* pht = m_prj->photoPoints(key);
    connect(pht, SIGNAL(tfxChanged(double)), ui.tfxSpinBox, SLOT(setValue(double)));
    connect(pht, SIGNAL(tfyChanged(double)), ui.tfySpinBox, SLOT(setValue(double)));
    connect(pht, SIGNAL(rfxChanged(double)), ui.rfxSpinBox, SLOT(setValue(double)));
    connect(pht, SIGNAL(rfyChanged(double)), ui.rfySpinBox, SLOT(setValue(double)));

    connect(ui.tfxSpinBox, SIGNAL(valueChanged(double)), pht, SLOT(setTfx(double)));
    connect(ui.tfySpinBox, SIGNAL(valueChanged(double)), pht, SLOT(setTfy(double)));
    connect(ui.rfxSpinBox, SIGNAL(valueChanged(double)), pht, SLOT(setRfx(double)));
    connect(ui.rfySpinBox, SIGNAL(valueChanged(double)), pht, SLOT(setRfy(double)));
}

void Photogrammetry::updateBackwardView(bool t)
{
    if (t)
    {
        QTabWidget* tab = ui.tabWidget;
        QString tabLabel;
        tabLabel = tr("交会");
        IntersectionWidget* intsWidget = 0;
        for (int i = 0; i < tab->count(); ++i)
        {
            if (tabLabel == tab->tabText(i))
            {
                intsWidget = (IntersectionWidget*)tab->widget(i);
                break;
            }
        }
        if (intsWidget == 0)
        {
            intsWidget = new IntersectionWidget(m_prj->intersection(m_prj->curIntersection()));
            tab->addTab(intsWidget,  tabLabel);
        }
        intsWidget->setIntersection(m_prj->intersection(m_prj->curIntersection()));
        intsWidget->updateBackward();
        tab->setCurrentWidget(intsWidget);
    }
}

void Photogrammetry::updateForwardView(bool t)
{
    if (t)
    {
        QTabWidget* tab = ui.tabWidget;
        QString tabLabel;
        tabLabel = tr("交会");
        IntersectionWidget* intsWidget = 0;
        for (int i = 0; i < tab->count(); ++i)
        {
            if (tabLabel == tab->tabText(i))
            {
                intsWidget = (IntersectionWidget*)tab->widget(i);
                break;
            }
        }
        if (intsWidget != 0)
        {
            intsWidget->updateForward();
            tab->setCurrentWidget(intsWidget);
        }
    }
}

void Photogrammetry::updateRelativeView(bool t)
{
        if (t)
    {
        QTabWidget* tab = ui.tabWidget;
        QString tabLabel;
        tabLabel = tr("定向");
        OrientationWidget* ortWidget = 0;
        for (int i = 0; i < tab->count(); ++i)
        {
            if (tabLabel == tab->tabText(i))
            {
                ortWidget = (OrientationWidget*)tab->widget(i);
                break;
            }
        }
        if (ortWidget == 0)
        {
            ortWidget = new OrientationWidget(m_prj->orientation(m_prj->curOrientation()));
            tab->addTab(ortWidget,  tabLabel);
        }
        ortWidget->setOrientation(m_prj->orientation(m_prj->curOrientation()));
        ortWidget->updateRelative();
        tab->setCurrentWidget(ortWidget);
    }
}

void Photogrammetry::updateAbsoluteView(bool t)
{
   if (t)
    {
        QTabWidget* tab = ui.tabWidget;
        QString tabLabel;
        tabLabel = tr("定向");
        OrientationWidget* ortWidget = 0;
        for (int i = 0; i < tab->count(); ++i)
        {
            if (tabLabel == tab->tabText(i))
            {
                ortWidget = (OrientationWidget*)tab->widget(i);
                break;
            }
        }
        if (ortWidget != 0)
        {
            ortWidget->updateAbsolute();
            tab->setCurrentWidget(ortWidget);
        }
    }
}

void Photogrammetry::updateOnestepView(bool t)
{
    if (t)
    {
        QTabWidget* tab = ui.tabWidget;
        QString tabLabel;
        tabLabel = tr("一步法");
        OnestepWidget* ostWidget = 0;
        for (int i = 0; i < tab->count(); ++i)
        {
            if (tabLabel == tab->tabText(i))
            {
                ostWidget = (OnestepWidget*)tab->widget(i);
                break;
            }
        }
        if (ostWidget == 0)
        {
            ostWidget = new OnestepWidget(m_prj->onestep(m_prj->curOnestep()));
            tab->addTab(ostWidget,  tabLabel);
        }
        ostWidget->setOnestep(m_prj->onestep(m_prj->curOnestep()));
        ostWidget->update();
        tab->setCurrentWidget(ostWidget);
    }
}

// end of public slots

