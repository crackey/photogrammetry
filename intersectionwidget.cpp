#include <QtGui>

#include "intersection.h"
#include "intersectionwidget.h"

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
    double const* orient = m_intersection->orient();
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(orient[i*6+j], 0, 'f', 3));
            orientTable->setItem(j, i, item);
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
    double *data = 0;
    int *index = 0;
    int np = 0;
    np = m_intersection->forwardResult(&index, &data);
    forwardTable->setRowCount(np);

    for (int i = 0; i < np; ++i)
    {
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(index[i]));
            forwardTable->setItem(i, 0, item);
    }
        for (int j = 0; j < np; ++j)
        {
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(data[3*j+1]/1000.0, 0, 'f', 3));
            forwardTable->setItem(j, 1, item);

            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(data[3*j]/1000.0, 0, 'f', 3));
            forwardTable->setItem(j, 2, item);

            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(data[3*j+2]/1000.0, 0, 'f', 3));
            forwardTable->setItem(j, 3, item);
        }
}

