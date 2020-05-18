#include "tree_view_signaled.h"


TreeViewSignaled::TreeViewSignaled(QWidget *parent) : QTreeView(parent){}

void TreeViewSignaled::mouseDoubleClickEvent(QMouseEvent * event)
{
  Q_UNUSED(event);
  emit doubleClicked();
}


