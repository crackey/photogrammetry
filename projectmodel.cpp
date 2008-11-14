#include "projectmodel.h"

ProjectModel::ProjectModel(Node* node, QObject* parent)
: QAbstractItemModel(parent)
{
    m_root = node;
    Node *n1 = m_root->children.at(0);
    Node *n2 = m_root->children.at(1);
}

ProjectModel::~ProjectModel()
{
    delete m_root;
}

int ProjectModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;
    Node* parentNode = parent.isValid() ? static_cast<Node*>(parent.internalPointer()) : m_root;
    if (!parentNode)
        return 0;
    return parentNode->children.count();
}

int ProjectModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant ProjectModel::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    
    Node* node = index.isValid() ? static_cast<Node*>(index.internalPointer()) : m_root;
    if (!node)
        return QVariant();
    
    if (index.column() == 0)
    {
        switch (node->type)
        {
        case Node::Root:
            return node->data;
        case Node::CTL:
            return node->data;
        case Node::PHT:
            return node->data;
        case Node::CTLITEM:
            return node->data.right(node->data.size() - node->data.lastIndexOf("\\") - 1);
            //Node* parentNode = static_cast<Node*>(index.parent().internalPointer());
            //map<QString, PhotoPoints>* pctlmap = static_cast<map<QString, PhotoPoints>*)parentNode->pdata;
        case Node::PHTITEM:
            return node->data.right(node->data.size() - node->data.lastIndexOf("\\") - 1);
        }
    }
    return QVariant();
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return tr("Project");
    }
    return QVariant();
}

QModelIndex ProjectModel::parent(const QModelIndex& child) const
{
    Node* node = child.isValid() ? static_cast<Node*>(child.internalPointer()) : m_root;
    if (!node)
        return QModelIndex();
    Node* parentNode = node->parent;
    if (!parentNode)
        return QModelIndex();
    Node* grandparentNode = parentNode->parent;
    if (!grandparentNode)
        return QModelIndex();

    int row = grandparentNode->children.indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QModelIndex ProjectModel::index(int row, int column, const QModelIndex& index) const
{
    if (!m_root || row < 0 || column < 0)
    {
        return QModelIndex();
    }
    Node* parentNode = index.isValid() ? static_cast<Node*>(index.internalPointer()) : m_root;
    Node* childNode = parentNode->children.at(row);
    return createIndex(row, column, childNode);
}

void ProjectModel::addItem(QString str)
{
    if (str.isEmpty())
        return;
    else
    {
        if (str.endsWith(".ctl"))
        {
            Node* pctlNode;
            for (int i = 0; i < m_root->children.count(); ++i)
            {
                if (m_root->children.at(i)->type == Node::CTL)
                    pctlNode = m_root->children.at(i);
            }
            pctlNode->children.append(new Node(Node::CTLITEM, str, pctlNode));
        }
        else if (str.endsWith(".pht"))
        {
            Node* pphtNode;
            for (int i = 0; i < m_root->children.count(); ++i)
            {
                if (m_root->children.at(i)->type == Node::PHT)
                    pphtNode = m_root->children.at(i);
            }
            pphtNode->children.append(new Node(Node::PHTITEM, str, pphtNode));
        }
    }
}
