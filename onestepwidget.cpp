#include <QtGui>
#include <map>
#include <vector>

#include "globaldefn.h"
#include "onestep.h"
#include "onestepwidget.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926
#endif

OnestepWidget::OnestepWidget(Onestep* ints, QWidget* parent)
: QWidget(parent)
{
    setOnestep(ints);
    ui.setupUi(this);
}

OnestepWidget::~OnestepWidget()
{
}

void OnestepWidget::setOnestep(Onestep* ints)
{
    m_onestep = ints;
}

void OnestepWidget::update()
{
    if (m_onestep == 0)
    {
        qDebug() << "No onestep";
        return;
    }
    QTableWidget* orientTable = ui.orientTable;
    QTableWidgetItem* item = 0;
    vector<double> orient;
    vector<double> orient_residual;
    m_onestep->orient(&orient, &orient_residual);
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(orient[i*6+j]));
            orientTable->setItem(j, i*2, item);
        }
        for (int j = 0; j < 6; ++j)
        {
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(orient_residual[i*6+j]));
            orientTable->setItem(j, i*2+1, item);
        }
    }
    QTableWidget* resultTable = ui.resultTable;
    map<int, Point> point;
    map<int, Point> point_residual;
    int np = 0;
    np = m_onestep->result(&point, &point_residual);
    resultTable->setRowCount(np);

    map<int, Point>::const_iterator itp;
    map<int, Point>::const_iterator itr;
    
    int i = 0;
    for (itp = point.begin(), itr = point_residual.begin(), i = 0; 
        itp != point.end() && itr != point_residual.end(); 
        ++i, ++itp, ++itr)
    {
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itp->first));
        resultTable->setItem(i, 0, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itp->second.y/1000.0, 0, 'f', 3));
        resultTable->setItem(i, 1, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itp->second.x/1000.0, 0, 'f', 3));
        resultTable->setItem(i, 2, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itp->second.z/1000.0, 0, 'f', 3));
        resultTable->setItem(i, 3, item);

        // residual
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.y/1000.0));
        resultTable->setItem(i, 4, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.x/1000.0));
        resultTable->setItem(i, 5, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.z/1000.0));
        resultTable->setItem(i, 6, item);
    }
}
