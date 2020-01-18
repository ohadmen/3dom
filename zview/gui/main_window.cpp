#include "main_window.h"
#include "canvas.h"
#include <QtWidgets/QTreeView>
#include <QtWidgets/QApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QStyle>
#include <QtWidgets/QLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>


QRect getCenterRect()
{
	// get the dimension available on this screen
	QSize availableSize = QApplication::primaryScreen()->availableSize();
	static const float initSizeScaleFactor = 0.5;

	int width = availableSize.width();
	int height = availableSize.height();
	//qDebug() << "Available dimensions " << width << "x" << height;
	width *= initSizeScaleFactor; // 90% of the screen size
	height *= initSizeScaleFactor; // 90% of the screen size
	QSize newSize(width, height);

	return QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,newSize,QApplication::primaryScreen()->availableGeometry());
	
}


QAction* MainWindow::privAddAction(const QString& str, void(MainWindow::* ff)(), QKeySequence q)
{
    QAction* p = new QAction("&" + str, this);
    if (q != QKeySequence::StandardKey::UnknownKey)
        p->setShortcut(q);
    QObject::connect(p, &QAction::triggered, this, ff);
    return p;
}
void MainWindow::privAddMenuBar()
{
	auto menuH = menuBar()->addMenu(tr("&File"));
    // menuH->addAction(privAddAction("save ply", &MainWindow::privCallback_savePly, QKeySequence::Save));
    // menuH->addAction(privAddAction("open", &MainWindow::privCallback_load, QKeySequence::Open));


}




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  Canvas* canvas = new Canvas(this);
  QTreeView *objList = new QTreeView(this);
  //TrackStateMachine* stateMachine = new TrackStateMachine;
  objList->setMinimumWidth(140);
  canvas->setMinimumWidth(500);
  canvas->setMinimumHeight(400);

  m_status.setStyleSheet("QLabel { background-color : #FFFFFF; color : black; }");

  QHBoxLayout *layoutP = new QHBoxLayout;
  layoutP->addWidget(canvas, 0);

  QVBoxLayout *sublayoutP = new QVBoxLayout;
  sublayoutP->addWidget(objList, 0);
  sublayoutP->addWidget(&m_status, 1);
  sublayoutP->setStretch(0, 70);
  sublayoutP->setStretch(1, 30);

  layoutP->addLayout(sublayoutP, 1);
  layoutP->setStretch(0, 80);
  layoutP->setStretch(1, 20);

  QWidget *widget = new QWidget();
  widget->setLayout(layoutP);
  setCentralWidget(widget);
  setGeometry(getCenterRect());

  privAddMenuBar();

  //TreeModel* g = new TreeModel(objList,{ "Layer name","v" });

  // objList->setModel(g);

  // QObject::connect(stateMachine, &TrackStateMachine::setStatus, this, &CurieMainWin::setStatus);
  // QObject::connect(g, &TreeModel::viewLabelChanged, &drawablesBuffer, &DrawablesBuffer::viewLabelChanged);
  // QObject::connect(&drawablesBuffer, &DrawablesBuffer::addItem, g, &TreeModel::addItem);
  // QObject::connect(&drawablesBuffer, &DrawablesBuffer::canvasUpdate, canvas, &Canvas::forceUpdate);
  // QObject::connect(this, &CurieMainWin::resetView, canvas, &Canvas::resetView);
  // QObject::connect(stateMachine, &TrackStateMachine::canvasUpdate, canvas, &Canvas::forceUpdate);

  // QObject::connect(stateMachine, &TrackStateMachine::setTexture, canvas, &Canvas::setTexture);

  // QObject::connect(this, &CurieMainWin::signal_keyEvent  ,stateMachine, &TrackStateMachine::input );
  // QObject::connect(this, &CurieMainWin::signal_mouseEvent,stateMachine, &TrackStateMachine::input );
  // QObject::connect(this, &CurieMainWin::signal_wheelEvent,stateMachine, &TrackStateMachine::input );

  // setFocus();
}
