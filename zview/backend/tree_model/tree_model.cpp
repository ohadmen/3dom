#include <QtCore/QStringList>
#include "tree_model.h"
#include <QtWidgets/QCheckBox>

TreeModel::TreeModel(QTreeView *treeViewP, const QStringList &headerString, QObject *parent)
    : QAbstractItemModel(parent), m_rootItem(new TreeItem("", -1, nullptr)), m_headerString(headerString), m_treeViewP(treeViewP)
{
    
}

TreeModel::~TreeModel()
{
    delete m_rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem *>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    QVariant retval;
    switch (index.column())
    {
    case 0: //handle num
    {
        int num = item->getHandleNum();
        if (role == Qt::DisplayRole)
            retval = num == -1 ? QString() : QString::number(num);
        break;
    }
    case 1: //name
    {
        if (role == Qt::DisplayRole)
            retval = item->getName();
        break;
    }
    case 2: //checkbox
    {
        if (role == Qt::CheckStateRole)
            retval = static_cast<int>(item->isChecked() ? Qt::Checked : Qt::Unchecked);
        break;
    }
    default:
        break;
    }
    return retval;
}
bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == 2)
    {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

        item->setChecked(value == Qt::Checked);
        emit dataChanged(createIndex(index.row(), 1), createIndex(rowCount(), columnCount()));
        emit viewLabelChanged(item->getHandleNum(), item->isChecked());
        //set focus back to parent
        m_treeViewP->parentWidget()->setFocus();
    }

    return true;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == 1)
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_headerString[section];

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
    const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *m_parentItem;

    if (!parent.isValid())
        m_parentItem = m_rootItem;
    else
        m_parentItem = static_cast<TreeItem *>(parent.internalPointer());

    TreeItem *childItem = m_parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
    if (!childItem)
        return QModelIndex();
    TreeItem *m_parentItem = childItem->parent();

    if (m_parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(m_parentItem->row(), 0, m_parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *m_parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        m_parentItem = m_rootItem;
    else
        m_parentItem = static_cast<TreeItem *>(parent.internalPointer());

    return m_parentItem->childCount();
}

void privAddItemRec(const QStringList &list, int handleNum, TreeItem *parent)
{
    if (list.isEmpty())
        return;
    QStringList listm1 = list;
    QString cur = listm1.first();
    listm1.pop_front();

    TreeItem *child = nullptr;
    for (auto &ch : *parent)
    {
        if (ch->getName() == cur)
        {
            child = ch;
            break;
        }
    }
    if (!child)
    {
        //handle num is for actual object
        int treeHandle = listm1.empty() ? handleNum : -1;
        child = new TreeItem(cur, treeHandle, parent);
        parent->appendChild(child);
    }
    privAddItemRec(listm1, handleNum, child);
}

void TreeModel::addItem(const QString &str, size_t handleNum)
{
    
    QStringList list = str.simplified().split(QRegExp("\\/"), QString::SkipEmptyParts);

    privAddItemRec(list, handleNum, m_rootItem);
    m_treeViewP->reset();
    m_treeViewP->expand(index(rowCount() - 1, columnCount() - 1));
}
void TreeModel::removeItem(size_t handleNum)
{
    //go over all tree and locate object with handlenum
    std::vector<TreeItem *> stack;
    stack.push_back(m_rootItem);
    while (!stack.empty())
    {
        TreeItem *cur = stack.back();
        stack.pop_back();

        if (cur->getHandleNum() == int(handleNum))
        {
            if (cur->childCount() == 0)
            {
                //leaf node - remove
                cur->parent()->removeChild(cur);
            }
            else
            {
                //has childeren - just set handleNum to -i
                cur->setHandleNum(-1);
            }
            break;
        }

        for (auto &a : *cur)
            stack.push_back(a);
    }
}