#pragma once
#include <QtCore/QObject>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow
{
  Q_OBJECT

  	void privAddMenuBar();
    QAction* privAddAction(const QString& str, void(MainWindow::* ff)(), QKeySequence q = QKeySequence::UnknownKey);


public:
  explicit MainWindow(QWidget *parent = 0);

private:
  QTextBrowser m_status;
  
};

