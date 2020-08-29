#include "main_window.h"
#include "zview/backend/shared_memory/shared_memory_manager.h"
#include <QtGui/QClipboard>
#include <QtGui/QWindow>
#include <QtWidgets/QMessageBox>
#include <QtCore/QMimeData>
#include <QtWidgets/QApplication>
#include <QtCore/QSettings>
#include <QtGui/QScreen>
#include <QtWidgets/QStyle>
#include <QtWidgets/QLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include "zview/io/read_file_list.h"
#include "zview/io/write_shape_to_file.h"
#include "zview/backend/tree_model/tree_model.h"
#include "canvas.h"

void MainWindow::privSavePlyCallback()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save .ply file", QSettings().value(m_default_dir_key).toString(), "*.ply");
    if (filename.isEmpty())
        return;
    savePly(filename);
}
void MainWindow::privShowHideAxes()
{
    auto val = Params::viewAxes();
    Params::viewAxes(1 - val);
    m_canvas->update();
}
void MainWindow::privShowHideGrid()
{
    auto val = Params::viewGrid();
    Params::viewGrid(1 - val);
    m_canvas->update();
}

void MainWindow::privloadFile()
{

    QFileDialog dialog(this);
    dialog.setNameFilter(tr("3d data storage(*.ply *.stl *.obj)"));
    dialog.setDirectory(QSettings().value(m_default_dir_key).toString());

    if (dialog.exec())
    {
        QStringList fileNames = dialog.selectedFiles();
        readFileList(fileNames);
    }
}


void MainWindow::setCamLookAt(const QVector3D& eye,const QVector3D& center,const QVector3D& up)
{
    m_canvas->setCamLookAt(eye,center,up);
}

void MainWindow::savePly(const QString &filename) const
{
    std::vector<Types::Shape> shapes;
    for (const auto &a : drawablesBuffer)
    {
        Types::Shape s = a.second->getShape();
        shapes.push_back(s);
    }

    io::writeShapeToFile(filename.toStdString(), shapes);
}
void MainWindow::setStatus(const QString &str)
{
    m_status.append(str);
    m_status.scroll(0, 1);
}

// QRect getCenterRect()
// {
//     // get the dimension available on this screen
//     QSize availableSize = QApplication::primaryScreen()->availableSize();
//     static const float initSizeScaleFactor = 0.5;

//     int width = availableSize.width();
//     int height = availableSize.height();
//     //qDebug() << "Available dimensions " << width << "x" << height;
//     width *= initSizeScaleFactor;  // 90% of the screen size
//     height *= initSizeScaleFactor; // 90% of the screen size
//     QSize newSize(width, height);

//     return QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, newSize, QApplication::primaryScreen()->availableGeometry());
// }
template <typename Func>
QAction *privAddAction(MainWindow *parent, const QString &str, Func ff, const QString &keySequenceStr = "")
{
    QKeySequence q = keySequenceStr == "" ? QKeySequence::UnknownKey : QKeySequence(keySequenceStr);
    QAction *p = new QAction("&" + str, parent);
    if (q != QKeySequence::StandardKey::UnknownKey)
        p->setShortcut(q);
    QObject::connect(p, &QAction::triggered, parent, ff);
    return p;
}
void MainWindow::privAddMenuBar()
{
    {
        auto top = menuBar()->addMenu(tr("&File"));
        top->addAction(privAddAction(this, "Open file", &MainWindow::privloadFile, "Ctrl+o"));
        top->addAction(privAddAction(this, "Save as ply", &MainWindow::privSavePlyCallback, "Ctrl+s"));
        top->addAction(privAddAction(this, "screenshot to clipboard", &MainWindow::takeScreenshot, "Ctrl+p"));
    }
    {
        auto top = menuBar()->addMenu(tr("&View"));
        top->addAction(privAddAction(this, "show/hide axes", &MainWindow::privShowHideAxes, "Ctrl+a"));
        top->addAction(privAddAction(this, "show/hide grid", &MainWindow::privShowHideGrid, "Ctrl+g"));
        auto txtTop = top->addMenu("Texture");
        {
            Canvas *c = m_canvas;

            for (int i = 0; i != 6; ++i)
                txtTop->addAction(privAddAction(
                    this, "texture #" + QString::number(i), [c, i]() {Params::drawablesTexture(i);c->forceUpdate(); }, "Ctrl+" + QString::number(i)));
        }
    }
    {

        QWidget *parent = this;
        auto top = menuBar()->addMenu(tr("&Help"));
        top->addAction(privAddAction(this, "About", [parent]() {
            QString aboutText(
                "Zview - a general 3d view (version 1.32)\n"
                "Zview was created as a tool to reflect the true state of 3d point cloud/mesh data stored in a file or on the heap."
                "Implementation was written by modern c++ and OpenGL ES, with an effort to minimize cpu load and memory signiture.");
            QMessageBox::about(parent, "Zview", aboutText);
        }));
    }
}

void MainWindow::takeScreenshot()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;

    QApplication::beep();
    QPixmap originalPixmap = screen->grabWindow(winId());
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(originalPixmap);
    m_canvas->setStatus("image copied to clipboard");
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (const auto &a : urlList)
        {
            pathList.append(a.toLocalFile());
        }

        // call a function to open the files
        readFileList(pathList);
    }
}
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(":zview256.png"));

    setMouseTracking(true);
    setAcceptDrops(true); //drag and drop
    setFocus();
    m_smm = new SharedMemoryManager(this);
    m_canvas = new Canvas(this);
    m_treeModel = new TreeModel(this);

    m_canvas->setMinimumWidth(500);
    m_canvas->setMinimumHeight(400);

    m_status.setStyleSheet("QLabel { background-color : #FFFFFF; color : black; }");

    QHBoxLayout *layoutP = new QHBoxLayout;
    layoutP->addWidget(m_canvas, 0);

    QVBoxLayout *sublayoutP = new QVBoxLayout;
    sublayoutP->addWidget(m_treeModel->getTreeView(), 0);
    sublayoutP->addWidget(&m_status, 1);
    sublayoutP->setStretch(0, 70);
    sublayoutP->setStretch(1, 30);

    layoutP->addLayout(sublayoutP, 1);
    layoutP->setStretch(0, 80);
    layoutP->setStretch(1, 20);

    QWidget *widget = new QWidget();
    widget->setLayout(layoutP);
    setCentralWidget(widget);

    showMaximized();

    privAddMenuBar();

    //when chaning object visiblity on the tree view - update it in the buffer
    QObject::connect(m_treeModel, &TreeModel::signal_viewLabelChanged, &drawablesBuffer, &DrawablesBuffer::setShapeVisability);
    //when adding new shape to buffer, add list
    QObject::connect(&drawablesBuffer, &DrawablesBuffer::signal_shapeAdded, m_treeModel, &TreeModel::addItem);
    //when adding new shape to buffer, add list
    QObject::connect(&drawablesBuffer, &DrawablesBuffer::signal_shapeRemoved, m_treeModel, &TreeModel::removeItem);

    QObject::connect(&drawablesBuffer, &DrawablesBuffer::signal_updateCanvas, m_canvas, &Canvas::forceUpdate);
    //on double click in tree view - zoom to object
    QObject::connect(m_treeModel, &TreeModel::signal_focusOnObject, m_canvas, &Canvas::resetView);

    // QObject::connect(this, &CurieMainWin::resetView, canvas, &Canvas::resetView);
    QObject::connect(m_canvas, &Canvas::signal_setStatus, this, &MainWindow::setStatus);
    //extrnal event: save ply
    QObject::connect(m_smm, &SharedMemoryManager::signal_savePly, this, &MainWindow::setStatus);
    //extrnal event: set cam pos
    QObject::connect(m_smm, &SharedMemoryManager::signal_setCamLookAt, this, &MainWindow::setCamLookAt);
}
void MainWindow::readFileList(const QStringList &files)
{
    auto objList = io::readFileList(files);
    for (const auto &o : objList)
        m_canvas->addShape(o);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{

    if (e->key() == Qt::Key::Key_Delete && e->modifiers() == Qt::KeyboardModifier::NoModifier)
    {
        auto selectedKeys =  m_treeModel->getSelected();
        for(auto& k:selectedKeys)
        {
            
            drawablesBuffer.removeShape(k);
        }
        
    }
    m_canvas->input(e);
}
void MainWindow::keyReleaseEvent(QKeyEvent *e) { m_canvas->input(e); }
