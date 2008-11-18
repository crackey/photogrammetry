#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <QtGui>

#include "globaldefn.h"
#include "controlpoints.h"
#include "photopoints.h"
#include "photogrammetry.h"
#include "projectmodel.h"
#include "photopointsmodel.h"
#include "controlpointsmodel.h"
#include "intersectionwidget.h"
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
    connect(m_prj, SIGNAL(backwardFinished(bool)), this, SLOT(updateBackwardView(bool)));
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

void Photogrammetry::updateBackwardView(bool t)
{
    if (t)
    {
        QTabWidget* tab = ui.tabWidget;
        IntersectionWidget* intsWidget = new IntersectionWidget(m_prj->intersection(m_prj->curIntersection()));
        tab->addTab(intsWidget, "intersection");
    }
}

// end of public slots

#if 0

void Photogrammetry::backwardIntersection()
{
    // get the left photo
    double *lphtdata;
    double *lctldata;
    double lfocus;
    int lnumPoints;

    lnumPoints = backwardData(&lphtdata, &lctldata, &lfocus, 0); 
    //double m_orient[6] = { 0 }; // Xs, Ys, Zs, fai, omega, kappa
    for (int i = 0; i < lnumPoints; ++i)
    {
        m_orient[0] += lctldata[i*3];
        m_orient[1] += lctldata[i*3+1];
        m_orient[2] += lctldata[i*3+2];
    }
    m_orient[0] /= lnumPoints;
    m_orient[1] /= lnumPoints;
    m_orient[2] = m_orient[2]/lnumPoints + 1e3*lfocus;
    m_orient[3] = m_orient[4] = m_orient[5] = 0.0;
    backward(lphtdata, lctldata, m_orient, lfocus, lnumPoints);

    // get the right photo
    double *rphtdata;
    double *rctldata;
    double rfocus;
    int rnumPoints;

    rnumPoints = backwardData(&rphtdata, &rctldata, &rfocus, 1); 
    //double m_orient[6] = { 0 }; // Xs, Ys, Zs, fai, omega, kappa
    for (int i = 0; i < lnumPoints; ++i)
    {
        m_orient[6] += rctldata[i*3];
        m_orient[7] += rctldata[i*3+1];
        m_orient[8] += rctldata[i*3+2];
    }
    m_orient[6] /= rnumPoints;
    m_orient[7] /= rnumPoints;
    m_orient[8] = m_orient[2]/rnumPoints + 1e3*rfocus;
    m_orient[9] = m_orient[10] = m_orient[11] = 0.0;
    backward(rphtdata, rctldata, m_orient+6, rfocus, rnumPoints);
    emit forwardAvailable(true);
    emit backwardFinished();
    // these code should be seperated to another function.
  //  QTableWidgetItem *newItem = new QTableWidgetItem("X");
     ui.lorientTableWidget->setItem(0, 0, new QTableWidgetItem("Xs")); 
     ui.lorientTableWidget->setItem(1, 0, new QTableWidgetItem("Ys")); 
     ui.lorientTableWidget->setItem(2, 0, new QTableWidgetItem("Zs")); 
     ui.lorientTableWidget->setItem(3, 0, new QTableWidgetItem("Phi")); 
     ui.lorientTableWidget->setItem(4, 0, new QTableWidgetItem("Omega")); 
     ui.lorientTableWidget->setItem(5, 0, new QTableWidgetItem("Kappa")); 
     ui.lorientTableWidget->setItem(0, 1, new QTableWidgetItem("")); 
     ui.lorientTableWidget->setItem(1, 1, new QTableWidgetItem("")); 
     ui.lorientTableWidget->setItem(2, 1, new QTableWidgetItem("")); 
     ui.lorientTableWidget->setItem(3, 1, new QTableWidgetItem("")); 
     ui.lorientTableWidget->setItem(4, 1, new QTableWidgetItem("")); 
     ui.lorientTableWidget->setItem(5, 1, new QTableWidgetItem("")); 
     QTableWidgetItem *item;
     for (int i = 0; i < 6; ++i)
     {
        item = ui.lorientTableWidget->item(i,1);
        item->setData(Qt::DisplayRole, m_orient[i]);
     }

}

int Photogrammetry::backwardData(double** ppht, double** pctl, double* focus, int p)  // p = 0 for left photo, 1 for right
{
    int np = 0; // number of matched points
    PhotoPoints* tpht = m_prj->photoPoints(m_prj->curPhotoPoints());
    ControlPoints* tctl = m_prj->controlPoints(m_prj->curControlPoints());
   // tpht = m_pht[m_curPhotoPoints];
   // tctl = m_ctl[m_curControlPoints];
    map<int, Point> *ctl = &tctl->m_points;
    map<int, vector<Point> > *pht = &tpht->m_points;
    map<int, Point>::iterator itc;
    map<int, vector<Point> >::iterator itp;
    int n = min(ctl->size(), pht->size());
    int* keys = new int[n];
    for (itc = ctl->begin(), itp = pht->begin();
         (itc != ctl->end()) && (itp != pht->end()); 
         )
    {
        if (itc->first == itp->first)
        {
            keys[np] = itc->first;
            ++np;
            ++itc; 
            ++itp;
        }
        else if (itc->first < itp->first)
            ++itc;
        else
            ++itp;
    }
    double* phtdata = new double[2*np];
    double* ctldata = new double[3*np];
    double scalex;
    double scaley;
    scalex = tpht->m_fiducial[2] / tpht->m_fiducial[0];
    scaley = tpht->m_fiducial[3] / tpht->m_fiducial[1];
    for (int i = 0; i < np; ++i)
    {
        ctldata[i*3] = (*ctl)[keys[i]].y * 1e3; // x and y should be reverted
        ctldata[i*3+1] = (*ctl)[keys[i]].x * 1e3;
        ctldata[i*3+2] = (*ctl)[keys[i]].z * 1e3;

        switch (p)
        {
        case 0:   // the left photo
            phtdata[i*2] = (*pht)[keys[i]][0].x - 100;
            phtdata[i*2+1] = 100 - (*pht)[keys[i]][0].y;
            break;
        case 1:   // the right photo
            phtdata[i*2] = (*pht)[keys[i]][0].x - 100 - (*pht)[keys[i]][1].x;
            phtdata[i*2+1] = 100 - (*pht)[keys[i]][0].y + 10 - (*pht)[keys[i]][1].y;
            break;
        default:
            break;
        }
        phtdata[i*2] *= scalex;
        phtdata[i*2+1] *= scaley;
    }
    *focus = -(*pht)[keys[0]][0].z;
    *ppht = phtdata;
    *pctl = ctldata;

    return np;
}

void Photogrammetry::forwardIntersection()
{
    PhotoPoints* tpht = m_prj->photoPoints(m_prj->curPhotoPoints());
    map<int, vector<Point> >* pht = &tpht->m_points;
    int np;
    np = pht->size();
    double* phtdata = new double[6*np];
    map<int, vector<Point> >::iterator it;
    it = pht->begin();

    double scalex;
    double scaley;
    scalex = tpht->m_fiducial[2] / tpht->m_fiducial[0];
    scaley = tpht->m_fiducial[3] / tpht->m_fiducial[1];
    for (int i = 0; it != pht->end(); ++it, ++i)
    {
        phtdata[i*6] = it->second[0].x - 100;
        phtdata[i*6+1] = 100 - it->second[0].y;
        phtdata[i*6+2] = it->second[0].z;
        phtdata[i*6+3] = phtdata[i*6] - it->second[1].x;
        phtdata[i*6+4] = phtdata[i*6+1] + 10 - it->second[1].y;
        phtdata[i*6+5] = it->second[1].z;

        phtdata[i*6] *= scalex;
        phtdata[i*6+1] *= scaley;
        phtdata[i*6+3] *= scalex;
        phtdata[i*6+4] *= scaley;
#ifdef _DEBUG
        qDebug() << phtdata[i*6] << phtdata[i*6+1] 
            << phtdata[i*6+2] << phtdata[i*6+3]
            << phtdata[i*6+4] << phtdata[i*6+5];
#endif
    }
    double* out = new double[3*np];

    forward(phtdata, m_orient, out, np);
#ifdef _DEBUG
    for (int i = 0; i < 12; ++i)
    {
        qDebug() << m_orient[i];
    }
    for (int i = 0; i < np; ++i)
    {
        qDebug() << out[3*i]/1e3 << out[3*i+1]/1e3 << out[3*i+2]/1e3;
    }
#endif
    delete []out;
    delete []phtdata;
}
#endif 

#if 0
void Photogrammetry::forwardIntersection()
{
    m_prj->addIntersection(m_prj->curControlPoints(), m_prj->curPhotoPoints());
}

void Photogrammetry::backwardIntersection()
{}
#endif
