#ifndef PHOTOPOINTSMODEL_H
#define PHOTOPOINTSMODEL_H

#include <QAbstractTableModel>

class PhotoPoints;

class PhotoPointsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PhotoPointsModel(PhotoPoints* pht = 0, QObject* parent = 0);
    ~PhotoPointsModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setRoot(PhotoPoints* pht = 0);

private:
    PhotoPoints* m_root;
};

#endif
