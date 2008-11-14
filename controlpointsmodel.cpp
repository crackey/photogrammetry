#include "controlpoints.h"
#include "controlpointsmodel.h"

#include <QtDebug>
#include <iostream>
using namespace std;

ControlPointsModel::ControlPointsModel(ControlPoints* ctl, QObject* parent)
: QAbstractTableModel(parent)
{
    setRoot(ctl);
}

ControlPointsModel::~ControlPointsModel()
{
    m_root = 0;  // the model should not delete the data.
}

int ControlPointsModel::rowCount(const QModelIndex& parent) const
{
    if (m_root)
    {
        return m_root->m_pointNum;
    }
    return 0;
}

int ControlPointsModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}

QVariant ControlPointsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_root == 0)
        return QVariant();
    //if (role == Qt::TextAlignmentRole)
    //{
    //    return int(Qt::AlignRight | Qt::AlignVCenter);
    //}

    if (role == Qt::DisplayRole)
    {
        int i = 0;
        map<int, Point>::const_iterator it;
        for (it = m_root->m_points.begin();
             it != m_root->m_points.end() && i < index.row();
             ++it, ++i)
           ;

        switch (index.column())
        {
        case 0:
            return QString("%1").arg(it->first);
        case 1:
            return QString("%1").arg(it->second.x, 0, 'f', 3);
        case 2:
            return QString("%1").arg(it->second.y, 0, 'f', 3);
        case 3:
            return QString("%1").arg(it->second.z, 0, 'f', 3);
        }
    }
    return QVariant();
}

QVariant ControlPointsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return tr("Point Number");
        case 1:
            return tr("X");
        case 2:
            return tr("Y");
        case 3:
            return tr("Z");
        }

    }
    else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        return section + 1;
    }
    return QVariant();
}

void ControlPointsModel::setRoot(ControlPoints* ctl)
{
    m_root = ctl;
    reset();
}
