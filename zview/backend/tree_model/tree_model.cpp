#include <QtCore/QStringList>
#include "tree_model.h"
#include "zview/gui/drawables/drawables_buffer.h"
#include <QtWidgets/QCheckBox>
#include <QtCore/QDebug>

constexpr int CHECKBOX_COL  = 1;
constexpr int TITLE_COL  = 2;
constexpr int HANDLENUM_COLUMN  = 3;

TreeModel::TreeModel(QWidget *parent)
    : QAbstractItemModel(parent), m_rootItem(new TreeItem("", -1, nullptr)), m_treeViewP(new QTreeView(parent))
{
     m_headerString = QStringList({"","#", "Layer name", "v"});
    m_treeViewP->setModel(this);
    m_treeViewP->setColumnWidth(0, 0);
    m_treeViewP->setColumnWidth(1, 10);
    m_treeViewP->setColumnWidth(2, 180);
    m_treeViewP->setColumnWidth(3, 10);
    m_treeViewP->setMinimumWidth(140);

     
}
QTreeView* TreeModel::getTreeView(){return m_treeViewP;}
TreeModel::~TreeModel()
{
    delete m_rootItem;
}
    void TreeModel::removeSelected()
{
    const auto index = m_treeViewP->currentIndex();
    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    int num= item->getHandleNum();
    if(num==-1)
        return;
    removeItem(num);
    emit dataChanged(createIndex(0, 1), createIndex(rowCount(), columnCount()));
    m_treeViewP->parentWidget()->setFocus();

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
    case HANDLENUM_COLUMN: //handle num
    {
        int num = item->getHandleNum();
        if (role == Qt::DisplayRole)
            retval = num == -1 ? QString() : QString::number(num);
        break;
    }
    case TITLE_COL: //name
    {
        if (role == Qt::DisplayRole)
            retval = item->getName();
        break;
    }
    case CHECKBOX_COL: //checkbox
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
    if (role == Qt::CheckStateRole && index.column() == CHECKBOX_COL)
    {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

        bool checkval = value == Qt::Checked;
        for (auto a : sprivGetChildren(item))
        {
            a->setChecked(checkval);
            emit viewLabelChanged(a->getHandleNum(), a->isChecked());
        }

        emit dataChanged(createIndex(index.row(), 1), createIndex(rowCount(), columnCount()));

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

    if (index.column() == CHECKBOX_COL)
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

    auto r = m_parentItem->childCount();
    return r;
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
    std::vector<TreeItem *> flatList = sprivGetChildren(m_rootItem);
    std::for_each(flatList.begin(), flatList.end(), [&handleNum](TreeItem *a) {
        if (a->getHandleNum() == int(handleNum))
        {
            if (a->childCount() == 0)
            {
                //leaf node - remove
                a->parent()->removeChild(a);
            }
            else
            {
                //has childeren - just set handleNum to -i
                a->setHandleNum(-1);
            }
        }
    });
    drawablesBuffer.removeShape(handleNum);
}

std::vector<TreeItem *> TreeModel::sprivGetChildren(TreeItem *root)
{
    std::vector<TreeItem *> list;
    list.push_back(root);
    size_t i = 0;
    while (i != list.size())
    {
        TreeItem* cur = list[i++];
        for(auto a:*cur)
            list.push_back(a);
        
        
    }
    return list;
}