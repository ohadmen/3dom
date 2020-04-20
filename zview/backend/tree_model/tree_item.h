#pragma once
#include <QtCore/QList>

class TreeItem
{
public:
    TreeItem(const QString& name,int handleNum, TreeItem* parent);
    ~TreeItem();

    void appendChild(TreeItem* child);
    void removeChild(TreeItem* itm);

    TreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QString getName() const;
    QString getFullName() const;

    int getHandleNum() const;

    int row() const;
    TreeItem* parent();
    QList<TreeItem*>::iterator begin();
    QList<TreeItem*>::iterator end();
    bool isChecked();
    void setChecked(bool set);
    void setHandleNum(int hn);

private:
    QList<TreeItem*> childItems;
    
    QString m_name;
    int m_handleNum;
    TreeItem* m_parentItem;
    bool m_checked;
};
