#include <QtGui>
#include <vector>
#include <map>

#include "intersection.h"
#include "intersectionwidget.h"
#include "globaldefn.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926
#endif

IntersectionWidget::IntersectionWidget(Intersection* ints, QWidget* parent)
: QWidget(parent)
{
    setIntersection(ints);
    ui.setupUi(this);
}

IntersectionWidget::~IntersectionWidget()
{
}

void IntersectionWidget::setIntersection(Intersection* ints)
{
    m_intersection = ints;
}

void IntersectionWidget::updateBackward()
{
    if (m_intersection == 0)
    {
        qDebug() << "No intersection";
        return;
    }
    QTableWidget* orientTable = ui.orientTable;
    QTableWidgetItem* item = 0;
    vector<double> orient;
    vector<double> orient_residual;
    m_intersection->orient(&orient, &orient_residual);
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

}

void IntersectionWidget::updateForward()
{
    if (m_intersection == 0)
    {
        qDebug() << "No intersection";
        return;
    }
    QTableWidget* forwardTable = ui.forwardTable;
    QTableWidgetItem* item = 0;
    map<int, Point> result;
    map<int, Point>::const_iterator itr;
    int np = 0;
    np = m_intersection->forwardResult(&result);
    forwardTable->setRowCount(np);

    int i;
    for (i = 0, itr = result.begin(); itr != result.end(); ++itr, ++i)
    {
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->first));
        forwardTable->setItem(i, 0, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.y/1000.0, 0, 'f', 3));
        forwardTable->setItem(i, 1, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.x/1000.0, 0, 'f', 3));
        forwardTable->setItem(i, 2, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(itr->second.z/1000.0, 0, 'f', 3));
        forwardTable->setItem(i, 3, item);
    }
}

