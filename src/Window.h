#pragma 

#include <QDesktopWidget>
#include <qfiledialog.h>
#include <QMainWindow>
#include <QSurfaceFormat>
#include <QtCore\qmimedata.h>
#include <QMenuBar>  
#include "Canvas.h"
class Window : public QMainWindow
{
	Q_OBJECT
      
private:

	Canvas m_canvas;
    const QObject m_parent;

    QAction m_openAcation;


    void privOpenAction_callback()
    {
        QString filename = QFileDialog::getOpenFileName(
            this, "Load .stl file", QString(), "*.stl");
        if (!filename.isNull())
        {
            loadMeshFromFile(filename);
        }
    }

public:
	
	explicit Window(QObject *parent = 0):m_parent(parent), 
        m_openAcation("Open", this)
    {


        setCentralWidget(&m_canvas);

        m_openAcation.setShortcut(QKeySequence::Open);
        QObject::connect(&m_openAcation, &QAction::triggered,
            this, &Window::privOpenAction_callback);

        auto file_menu = menuBar()->addMenu(tr("&File"));
        file_menu->addAction(&m_openAcation);
        
        resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
        show();
        
        
    }
    void loadMeshFromFile(const QString& meshfn)
    {
        int token = Loader::i().load(meshfn);
        m_canvas.setToken(token);
        m_canvas.cam2geometry();
    }
protected:

   
};