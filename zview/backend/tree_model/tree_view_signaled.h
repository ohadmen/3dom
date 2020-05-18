#pragma once
#include <QObject>
#include <QtWidgets/QTreeView>


class TreeViewSignaled: public QTreeView
{
  Q_OBJECT
  public:
  TreeViewSignaled(QWidget *parent = 0);

protected:
  void mouseDoubleClickEvent(QMouseEvent * event) override;
  

signals:
  void doubleClicked();

};