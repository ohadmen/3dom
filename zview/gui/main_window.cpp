#include "main_window.h"
#include "zview/backend/tree_model/tree_model.h"
#include <QtWidgets/QApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QStyle>
#include <QtWidgets/QLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include "zview/io/read_file_list.h"
#include "zview/io/write_shape_to_file.h"



void MainWindow::privSavePly()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save .ply file", QString(), "*.ply");
    std::vector<Types::Shape> shapes;
    for(const auto& a:drawablesBuffer)
    {
        Types::Shape s = a.second->getShape();
        shapes.push_back(s);
    }
        
    io::writeShapeToFile(filename.toStdString(),shapes);
    
}
void MainWindow::privloadFile()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "load file", QString(), tr("PLY (*.ply);;STL (*.stl);;OBJ (*.obj);;All files (*.*)"));
    readFileList(filenames);

}

void MainWindow::slot_setStatus(const QString &str)
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
    {
    auto top = menuBar()->addMenu(tr("&File"));
    top->addAction(privAddAction("Open file", &MainWindow::privloadFile, QKeySequence::Open));
    top->addAction(privAddAction("Save", &MainWindow::privSavePly, QKeySequence::Save));
    }
    {
        auto top = menuBar()->addMenu(tr("&Help"));
        top->addAction(privAddAction("Manual", nullptr));
        top->addAction(privAddAction("About", nullptr));
    }
    
    
    
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setMouseTracking(true);
    setFocus();

    m_canvas = new Canvas(this);

    TreeModel *treeModel = new TreeModel(this);
    m_canvas->setMinimumWidth(500);
    m_canvas->setMinimumHeight(400);

    m_status.setStyleSheet("QLabel { background-color : #FFFFFF; color : black; }");

    QHBoxLayout *layoutP = new QHBoxLayout;
    layoutP->addWidget(m_canvas, 0);

    QVBoxLayout *sublayoutP = new QVBoxLayout;
    sublayoutP->addWidget(treeModel->getTreeView(), 0);
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
    QObject::connect(treeModel, &TreeModel::viewLabelChanged, &drawablesBuffer, &DrawablesBuffer::setShapeVisability);
    //when adding new shape to buffer, add list
    QObject::connect(&drawablesBuffer, &DrawablesBuffer::shapeAdded, treeModel, &TreeModel::addItem);
    //when adding new shape to buffer, add list
    QObject::connect(&drawablesBuffer, &DrawablesBuffer::shapeRemoved, treeModel, &TreeModel::removeItem);

    QObject::connect(&drawablesBuffer, &DrawablesBuffer::updateCanvas, m_canvas, &Canvas::slot_forceUpdate);

    // QObject::connect(this, &CurieMainWin::resetView, canvas, &Canvas::resetView);
    QObject::connect(m_canvas, &Canvas::signal_setStatus, this, &MainWindow::slot_setStatus);
}
void MainWindow::readFileList(const QStringList &files)
{
    auto objList = io::readFileList(files);
    for (const auto &o : objList)
        m_canvas->addShape(o);
}

void MainWindow::keyPressEvent(QKeyEvent *e) { m_canvas->input(e); }
void MainWindow::keyReleaseEvent(QKeyEvent *e) { m_canvas->input(e); }
