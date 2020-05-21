#pragma once



#include <QtCore/QObject>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QMainWindow>

class Canvas;
class TreeModel;
class SharedMemoryManager;
class MainWindow : public QMainWindow
{
  Q_OBJECT
  
  void privloadFile();
  void privSavePly();
  void privShowHideAxes();
  void privShowHideGrid();
  

  void privAddMenuBar();
  
public slots:
  void slot_setStatus(const QString &str);

public:
  explicit MainWindow(QWidget *parent = 0);
  void takeScreenshot();
  void dropEvent(QDropEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void keyPressEvent(QKeyEvent *e) override;
  void keyReleaseEvent(QKeyEvent *e) override;
  void readFileList(const QStringList &files);

private:
  QTextBrowser m_status;
  Canvas *m_canvas;
  TreeModel* m_treeModel;
  SharedMemoryManager* m_smm;
  const QString m_default_dir_key="default_dir";
};
