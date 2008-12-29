#include <map>
#include <vector>
#include <QtGui>

#include "orientation.h"
#include "orientationwidget.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926
#endif

OrientationWidget::OrientationWidget(Orientation* ort, QWidget* parent)
: QWidget(parent)
{
    setOrientation(ort);
    ui.setupUi(this);
}

OrientationWidget::~OrientationWidget()
{
}

void OrientationWidget::setOrientation(Orientation* ort)
{
    m_orientation = ort;
}

void OrientationWidget::updateRelative()
{
    if (m_orientation == 0)
    {
        qDebug() << "No orientation";
        return;
    }
    QTableWidget* orientTable = ui.relativeOrientTable;
    QTableWidgetItem* item = 0;
    vector<double> orients;
    vector<double> residual;
    int n = 0;
    n = m_orientation->relativeOrientElements(&orients, &residual);
    orientTable->setRowCount(n);
    for (int i = 0; i < n; ++i)
    {
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(orients[i]));
        orientTable->setItem(i, 0, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(residual[i]));
        orientTable->setItem(i, 1, item);
    }
}

void OrientationWidget::updateAbsolute()
{
    if (m_orientation == 0)
    {
        qDebug() << "No orientation";
        return;
    }

    // orient elements
    QTableWidget* orientTable = ui.absoluteOrientTable;
    QTableWidgetItem* item = 0; 
    vector<double> orient;
    vector<double> orient_residual;
    int n = 0;
    n = m_orientation->absoluteOrientElements(&orient, &orient_residual);
    orientTable->setRowCount(n);
    for (int i = 0; i < n; ++i)
    {
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(orient[i]));
        orientTable->setItem(i, 0, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(orient_residual[i]));
        orientTable->setItem(i, 1, item);
    }

    // point data
    QTableWidget* resultTable = ui.resultTable;

    map<int, Point> result;
    int np = 0;
    np = m_orientation->result(&result);
    resultTable->setRowCount(np);

    map<int, Point>::const_iterator itr;
    int i;
    for (i = 0, itr = result.begin(); itr != result.end(); ++itr, ++i)
    {   
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->first));
        resultTable->setItem(i, 0, item);
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.y/1000.0, 0, 'f', 3));
        resultTable->setItem(i, 1, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.x/1000.0, 0, 'f', 3));
        resultTable->setItem(i, 2, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.z/1000.0, 0, 'f', 3));
        resultTable->setItem(i, 3, item);
    }
}
