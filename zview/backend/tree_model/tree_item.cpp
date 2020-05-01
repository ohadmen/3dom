#include "tree_item.h"

bool TreeItem::isChecked() { return m_checked; }
void TreeItem::setChecked(bool set) {
    m_checked = set;
}
void TreeItem::setHandleNum(int hn){m_handleNum=hn;}
QList<TreeItem*>::iterator TreeItem::begin() { return m_childItems.begin(); }
QList<TreeItem*>::iterator TreeItem::end() { return m_childItems.end(); }

TreeItem::TreeItem(const QString& name,int handleNum, TreeItem* parent) :m_name(name),m_handleNum(handleNum), m_parentItem(parent), m_checked(true)
{}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem* item)
{
    m_childItems.append(item);
}
void TreeItem::removeChild(TreeItem* itm)
{
    m_childItems.removeOne(itm);
}



TreeItem* TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();

}

int TreeItem::columnCount() const
{
    return 4;
}

QString TreeItem::getName() const
{
    return m_name;
}
QString TreeItem::getFullName() const
{
    if (!m_parentItem)
        return "";
    return m_parentItem->getFullName()+"/"+m_name;
}


TreeItem* TreeItem::parent()
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}


int TreeItem::getHandleNum() const{return m_handleNum;}