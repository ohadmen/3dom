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
  void privSavePlyCallback();
  void privShowHideAxes();
  void privShowHideGrid();
  

  void privAddMenuBar();
  
public slots:
  int getLastKeyStroke(bool reset);
  void setStatus(const QString &str);
  void savePly(const QString& fn) const;
  
  void setCamLookAt(const QVector3D& eye,const QVector3D& center,const QVector3D& up);


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
  int m_lastKeyStroke;
};
