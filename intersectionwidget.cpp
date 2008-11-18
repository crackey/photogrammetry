#include <QtGui>

#include "intersection.h"
#include "intersectionwidget.h"

#define M_PI 3.1415926

IntersectionWidget::IntersectionWidget(Intersection* ints, QWidget* parent)
: QWidget(parent)
{
    m_intersection = ints;
    ui.setupUi(this);
    QTableWidget* orientTable = ui.orientTable;
    QTableWidgetItem* item = 0;
    double const* orient = ints->orient();
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            item = new QTableWidgetItem();
            //item = orientTable->item(i,j);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            if (j > 2)  
            {
                item->setData(Qt::DisplayRole, QString("%1").arg(orient[i*6+j]*180/M_PI, 0, 'f', 3));
            }
            else
            {
                item->setData(Qt::DisplayRole, QString("%1").arg(orient[i*6+j], 0, 'f', 3));
            }
            orientTable->setItem(j, i, item);
        }
    }
}

IntersectionWidget::~IntersectionWidget()
{}
