#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include "tree_view_signaled.h"
#include "tree_item.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
    std::vector<TreeItem*> sprivGetChildren(TreeItem* root);
signals:
    void signal_viewLabelChanged(size_t handleNum, bool setval);
    void signal_focusOnObject(int key);
    
    


public:
    TreeModel(QWidget* parent);
    ~TreeModel();

    QVariant data(const QModelIndex& index, int role)  const;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
        const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
public slots:
    void addItem(const QString& str,size_t handleNum);
    void removeItem(size_t handleNum);
    QTreeView* getTreeView();

    void removeSelected();
    void focusSelected();
    

private:
    

    TreeItem* m_rootItem;
    QStringList m_headerString;
    TreeViewSignaled* m_treeViewP;
};
