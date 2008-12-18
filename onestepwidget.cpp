#include <QtGui>

#include "onestep.h"
#include "onestepwidget.h"

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
    double* orient;
    double* orient_residual;
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
    double* data = 0;
    double* data_residual = 0;
    int* isctl = 0;
    int* index = 0;
    int np = 0;
    np = m_onestep->result(&index, &isctl, &data, &data_residual);
    resultTable->setRowCount(np);

    for (int i = 0; i < np; ++i)
    {
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::DisplayRole, QString("%1").arg(index[i]));
        resultTable->setItem(i, 0, item);
    }
    for (int j = 0, i = 0; j < np; ++j)
    {
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
        if (!isctl[j])
        {
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(data_residual[3*i+1]/1000.0));
            resultTable->setItem(j, 4, item);

            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(data_residual[3*i]/1000.0));
            resultTable->setItem(j, 5, item);

            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::DisplayRole, QString("%1").arg(data_residual[3*i+2]/1000.0));
            resultTable->setItem(j, 6, item);
            ++i;
        }
    }
}
