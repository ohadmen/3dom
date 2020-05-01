#pragma once
#include "zview/backend/tree_model/tree_model.h"
#include "canvas.h"
#include <QtCore/QObject>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
  void privloadFile();
  void privSavePly();
  void privAddMenuBar();
  QAction *privAddAction(const QString &str, void (MainWindow::*ff)(), QKeySequence q = QKeySequence::UnknownKey);
public slots:
  void slot_setStatus(const QString &str);

public:
  explicit MainWindow(QWidget *parent = 0);

  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  void readFileList(const QStringList &files);

private:
  QTextBrowser m_status;
  Canvas *m_canvas;
  TreeModel* m_treeModel;
  const QString m_default_dir_key="default_dir";
};
