#include <QtGui>

#include "orientation.h"
#include "orientationwidget.h"

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
    double *data = 0;
    double *s = 0;
    int n = 0;
    n = m_orientation->relativeOrientElements(&data, &s);
    orientTable->setRowCount(n);
    for (int i = 0; i < n; ++i)
    {
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(data[i]));
        orientTable->setItem(i, 0, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(s[i]));
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
    double *orient = 0;
    double *orient_residual = 0;
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

    double *data = 0;
    int *index = 0;
    int np = 0;
    np = m_orientation->result(&index, &data);
    resultTable->setRowCount(np);

    for (int j = 0; j < np; ++j)
    {   
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(index[j]));
        resultTable->setItem(j, 0, item);
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(data[3*j+1]/1000.0, 0, 'f', 3));
        resultTable->setItem(j, 1, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(data[3*j]/1000.0, 0, 'f', 3));
        resultTable->setItem(j, 2, item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(data[3*j+2]/1000.0, 0, 'f', 3));
        resultTable->setItem(j, 3, item);
    }
}
