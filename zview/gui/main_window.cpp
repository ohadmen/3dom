#include "main_window.h"
#include "zview/io/read_stl.h"
#include "zview/backend/tree_model/tree_model.h"
#include <QtWidgets/QTreeView>
#include <QtWidgets/QApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QStyle>
#include <QtWidgets/QLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>



void MainWindow::slot_setStatus(const QString& str)
{
    //auto t = std::time(nullptr);
    //auto tm = *std::localtime(&t);
    //std::stringstream ss;
    //ss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S]") << " " << str.toStdString();
    m_status.append(str);
    m_status.scroll(0, 1);
}


QRect getCenterRect()
{
    // get the dimension available on this screen
    QSize availableSize = QApplication::primaryScreen()->availableSize();
    static const float initSizeScaleFactor = 0.5;

    int width = availableSize.width();
    int height = availableSize.height();
    //qDebug() << "Available dimensions " << width << "x" << height;
    width *= initSizeScaleFactor;  // 90% of the screen size
    height *= initSizeScaleFactor; // 90% of the screen size
    QSize newSize(width, height);

    return QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, newSize, QApplication::primaryScreen()->availableGeometry());
}

QAction *MainWindow::privAddAction(const QString &str, void (MainWindow::*ff)(), QKeySequence q)
{
    QAction *p = new QAction("&" + str, this);
    if (q != QKeySequence::StandardKey::UnknownKey)
        p->setShortcut(q);
    QObject::connect(p, &QAction::triggered, this, ff);
    return p;
}
void MainWindow::privAddMenuBar()
{
    //auto menuH = menuBar()->addMenu(tr("&File"));
    // menuH->addAction(privAddAction("save ply", &MainWindow::privCallback_savePly, QKeySequence::Save));
    // menuH->addAction(privAddAction("open", &MainWindow::privCallback_load, QKeySequence::Open));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    
    setMouseTracking(true);
    setFocus();

    m_canvas = new Canvas(this);
    QTreeView *objList = new QTreeView(this);
    TreeModel* g = new TreeModel(objList,{"#", "Layer name","v" });
    objList->setModel(g);
    objList->setColumnWidth(0,10);
    objList->setColumnWidth(1,200);
    objList->setColumnWidth(2,10);
    //TrackStateMachine* stateMachine = new TrackStateMachine;
    objList->setMinimumWidth(140);
    m_canvas->setMinimumWidth(500);
    m_canvas->setMinimumHeight(400);

    m_status.setStyleSheet("QLabel { background-color : #FFFFFF; color : black; }");

    QHBoxLayout *layoutP = new QHBoxLayout;
    layoutP->addWidget(m_canvas, 0);

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

    

    

    //when chaning object visiblity on the tree view - update it in the buffer
    QObject::connect(g, &TreeModel::viewLabelChanged, &drawablesBuffer, &DrawablesBuffer::setShapeVisability);
    //when adding new shape to buffer, add list
    QObject::connect(&drawablesBuffer, &DrawablesBuffer::shapeAdded, g, &TreeModel::addItem);
    //when adding new shape to buffer, add list
    QObject::connect(&drawablesBuffer, &DrawablesBuffer::shapeRemoved, g, &TreeModel::removeItem);

    QObject::connect(&drawablesBuffer, &DrawablesBuffer::updateCanvas, m_canvas, &Canvas::slot_forceUpdate);

    // QObject::connect(&drawablesBuffer, &DrawablesBuffer::addItem, g, &TreeModel::addItem);
    // QObject::connect(&drawablesBuffer, &DrawablesBuffer::canvasUpdate, canvas, &Canvas::forceUpdate);
    // QObject::connect(this, &CurieMainWin::resetView, canvas, &Canvas::resetView);
    QObject::connect(m_canvas, &Canvas::signal_setStatus, this, &MainWindow::slot_setStatus);
    // QObject::connect(stateMachine, &TrackStateMachine::canvasUpdate, canvas, &Canvas::forceUpdate);
    // QObject::connect(stateMachine, &TrackStateMachine::setTexture, canvas, &Canvas::setTexture);


    Types::Mesh obj = io::readstl("/home/ohad/dev/projects/zview/example/horse.stl");
    m_canvas->addShape(obj,"debug");
    
    

}
void MainWindow::keyPressEvent(QKeyEvent* e){m_canvas->input(e);}
void MainWindow::keyReleaseEvent(QKeyEvent* e){m_canvas->input(e);}
