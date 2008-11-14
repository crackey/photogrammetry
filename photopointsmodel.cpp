#include "photopoints.h"
#include "photopointsmodel.h"

#include <QtDebug>
#include <iostream>
#include <fstream>
using namespace std;

PhotoPointsModel::PhotoPointsModel(PhotoPoints* pht, QObject* parent)
: QAbstractTableModel(parent)
{
    setRoot(pht);
}

PhotoPointsModel::~PhotoPointsModel()
{}

int PhotoPointsModel::rowCount(const QModelIndex& parent) const
{
    if (m_root)
    {
        return m_root->m_pointNum;
    }
    return 0;
}

int PhotoPointsModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant PhotoPointsModel::data(const QModelIndex& index, int role) const
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
        map<int, vector<Point> >::const_iterator it;
        for (it = m_root->m_points.begin();
             it != m_root->m_points.end() && i < index.row();
             ++it, ++i)
           ;
        switch (index.column())
        {
        case 0:
            return QString("%1").arg(it->first);
        case 1:
            return QString("%1").arg(it->second.at(0).x, 0, 'f', 3);
        case 2:
            return QString("%1").arg(it->second.at(0).y, 0, 'f', 3);
        case 3:
            return QString("%1").arg(it->second.at(1).x, 0, 'f', 3);
        case 4:
            return QString("%1").arg(it->second.at(1).y, 0, 'f', 3);
        }
    }
    return QVariant();
}

QVariant PhotoPointsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return tr("Point Number");
        case 1:
            return tr("x");
        case 2:
            return tr("y");
        case 3:
            return tr("p");
        case 4:
            return tr("q");
        }

    }
    else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        return section + 1;
    }
    return QVariant();
}

void PhotoPointsModel::setRoot(PhotoPoints *pht)
{
    m_root = pht;
    reset();
}
