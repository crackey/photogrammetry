#ifndef CONTROLPOINTSMODEL_H
#define CONTROLPOINTSMODEL_H

#include <QAbstractTableModel>

class ControlPoints;

class ControlPointsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ControlPointsModel(ControlPoints* ctl = 0, QObject* parent = 0);
    ~ControlPointsModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setRoot(ControlPoints* ctl = 0);

private:
    ControlPoints* m_root;
};

#endif
