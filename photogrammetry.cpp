#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QObject>

#include "globaldefn.h"
#include "controlpoints.h"
#include "photopoints.h"
#include "photogrammetry.h"
#include "projectmodel.h"
#include "photopointsmodel.h"
#include "controlpointsmodel.h"
//#include "intersection.h"

using namespace std;

Photogrammetry::Photogrammetry()
{
    m_orientation = false;
    m_intersection = false;
 
    for (int i = 0; i < 12; ++i)
        m_orient[i] = 0.0;
    ui.setupUi(this);

    m_prj = new PHGProject(0);
    /*
    QStandardItemModel *model = new QStandardItemModel;
    QStandardItem *parentItem = model->invisibleRootItem();
    for (int i = 0; i < 4; ++i) 
    {
        QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
        parentItem->appendRow(item);
    }
    */

    // setup the project model/view
    ProjectModel::Node *root = new ProjectModel::Node(ProjectModel::Node::Root, tr("PHGProject"));
    root->children.append(new ProjectModel::Node(ProjectModel::Node::CTL, tr("Control Points"), root));
    root->children.append(new ProjectModel::Node(ProjectModel::Node::PHT, tr("Photo Points"), root));
    ProjectModel *model = new ProjectModel(root, this);
    ui.projectView->setModel(model);
    //connect(ui.projectView, SIGNAL(clicked(QModelIndex)),
    //        this, SLOT(clicked(QModelIndex)));

    PhotoPointsModel* phtptsModel = new PhotoPointsModel(0, this);
    ui.photoPointsView->setModel(phtptsModel);
    ControlPointsModel* ctlptsModel = new ControlPointsModel(0, this);
    ui.controlPointsView->setModel(ctlptsModel);

    connect(this, SIGNAL(fileopen(QString)), m_prj, SLOT(openfile(QString)));
    connect(m_prj, SIGNAL(fileLoaded(QString)), model, SLOT(addItem(QString)));
    connect(m_prj, SIGNAL(fileLoaded(QString)), this, SLOT(addView(QString)));
    connect(m_prj, SIGNAL(backwardAvailable(bool)), 
            ui.intersectionBackwardAction, SLOT(setEnabled(bool)));
    connect(m_prj, SIGNAL(forwardAvailable(bool)), 
            ui.intersectionForwardAction, SLOT(setEnabled(bool)));
    connect(m_prj, SIGNAL(phtAvailable(QString)), this, SLOT(syncFiducial(QString)));
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
    filepath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", 
        tr("Control points (*.ctl);;"
           "Photo points (*.pht);;"
           "Distance of fiducial marks (*.fdl)"));
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
{}

void Photogrammetry::on_orientationAbsoluteAction_triggered()
{}

void Photogrammetry::on_intersectionForwardAction_triggered()
{
    m_prj->forwardIntersection();
}

void Photogrammetry::on_intersectionBackwardAction_triggered()
{
    m_prj->backwardIntersection();
}

void Photogrammetry::on_helpAboutAction_triggered()
{}

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

// end of public slots

