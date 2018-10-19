#pragma 

#include <QDesktopWidget>
#include <QMessageBox>
#include <qfiledialog.h>
#include <QMainWindow>
#include <QSurfaceFormat>
#include <QtCore/qmimedata.h>
#include <QMenuBar>  
#include <list>
#include "Canvas.h"


class Window : public QMainWindow
{
	Q_OBJECT
      
private:

	Canvas m_canvas;
    const QObject m_parent;

    
    std::list<QAction> m_menuActions;

    QAction* privAddAction(const QString& str, void(Window::* ff)(), QKeySequence q = QKeySequence::UnknownKey)
    {
        m_menuActions.emplace_back("&" + str, this);
        if (q != QKeySequence::StandardKey::UnknownKey)
            m_menuActions.back().setShortcut(q);
        QAction* p = &m_menuActions.back();
        QObject::connect(p, &QAction::triggered, this, ff);
        return p;

    }

    void privOpenAction_callback()
    {
        QString filename = QFileDialog::getOpenFileName(
            this, "Load .stl file", QString(), "*.stl");
        if (!filename.isNull())
        {
            loadMeshFromFile(filename);
        }
    }
    void privQuit_callback()
    {
        close();

    }
    void privAbout_callback()
    {

            QMessageBox::about(this, "",
                "<p align=\"center\"><b>3DOM</b></p>"
                "<p>A 3D object measurment<br>"
                "<a href=\"https://github.com/ohad.menashe/3dom\""
                "   style=\"color: #93a1a1;\">https://github.com/ohad.menashe/3dom</a></p>"
                "<p>© 2018 Ohad Menashe<br>"
                "<a href=\"mailto:ohad@menashe.co\""
                "   style=\"color: #93a1a1;\">ohad@menashe.co</a></p>");
     
    }
    void privTextureFace_callback()   { m_canvas.setTextureType(0);m_canvas.update(); }
    void privTextureNormal_callback() { m_canvas.setTextureType(1);m_canvas.update(); }
    void privTextureNone_callback()   { m_canvas.setTextureType(2);m_canvas.update(); }
public:
	
	explicit Window(QObject *parent = 0):m_parent(parent)
        
    {
        setCentralWidget(&m_canvas);
        //-----------------file menu-----------------
        {
            auto menuH = menuBar()->addMenu(tr("&File"));
            menuH->addAction(privAddAction("Open", &Window::privOpenAction_callback, QKeySequence::Open));
            menuH->addAction(privAddAction("Quit", &Window::privQuit_callback, tr("CTRL+Q")));
        }
        //-----------------texture menu-----------------
        {
            auto menuH = menuBar()->addMenu(tr("&Texture"));
            menuH->addAction(privAddAction("Face", &Window::privTextureFace_callback, tr("CTRL+1")));
            menuH->addAction(privAddAction("Normal", &Window::privTextureNormal_callback, tr("CTRL+2")));
            menuH->addAction(privAddAction("None", &Window::privTextureNone_callback, tr("CTRL+3")));
        }


        //-----------------help menu-----------------
        {
            auto menuH = menuBar()->addMenu(tr("&Help"));
            menuH->addAction(privAddAction("About", &Window::privAbout_callback));
        }

  
        
        resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
        show();
        
        
    }
    void loadMeshFromFile(const QString& meshfn)
    {
        m_canvas.loadMeshFromFile(meshfn);
        
    }
protected:

   
};