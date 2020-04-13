#pragma once
#include "canvas.h"
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

    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);
private:
  QTextBrowser m_status;
  Canvas * m_canvas;
  
};

