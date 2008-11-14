#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QList>

using namespace std;

class PHGProject;

class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    struct Node
    {
        enum Type {Root, PHT, CTL, PHTITEM, CTLITEM, Unknown };
        Node(Type tp = Unknown, QString str = "", Node* pparent = 0)
        {
            type = tp;
            data = str;
            parent = pparent;
        }
        ~Node()
        {
            qDeleteAll(children);
        }

        Type type;
        QString data;
        Node* parent;
        QList<Node*> children;
    };

    ProjectModel(Node* node = 0, QObject* parent = 0);
    ~ProjectModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;  
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex parent(const QModelIndex& child) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

public slots:
    void addItem(QString str);

private:
    Node* m_root;
};

#endif
