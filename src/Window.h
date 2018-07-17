#pragma 

#include <QDesktopWidget>
#include <QMessageBox>
#include <qfiledialog.h>
#include <QMainWindow>
#include <QSurfaceFormat>
#include <QtCore\qmimedata.h>
#include <QMenuBar>  
#include <QToolBar>

#include <list>
#include "Canvas.h"
#include "IOStream.h"


class Window : public QMainWindow
{
	Q_OBJECT
      
private:

	Canvas m_canvas;
    const QObject m_parent;
    std::string m_recordOutpuDir;

    
    std::map<std::string,QAction*> m_menuActions;

    QAction* privAddAction(const QString& str, void(Window::* ff)(), QKeySequence q = QKeySequence::UnknownKey,const QString& iconStr="")
    {
        if (iconStr.length() == 0)
        {
            
            m_menuActions[str.toStdString()] = new QAction("&" + str, this);
        }
        else
        {
            auto icn = QIcon::fromTheme(iconStr);
            bool nameok = !icn.isNull();
            m_menuActions[str.toStdString()] = new QAction(icn, str, this);
        }
        QAction* p = m_menuActions[str.toStdString()];
        if (q != QKeySequence::StandardKey::UnknownKey)
            p->setShortcut(q);
        

        QObject::connect(p, &QAction::triggered, this, ff);
        return p;

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
                "   style=\"color: #93a1a1;\">https://github.com/ohadmen/3dom</a></p>"
                "<p>© 2018 Ohad Menashe<br>"
                "<a href=\"mailto:ohad@menashe.co\""
                "   style=\"color: #93a1a1;\">ohad@menashe.co</a></p>");
     
    }

    void privToggleViewTrackball_callback()
    {
        Params::viewTrackball(1 - Params::viewTrackball());
    }

    void privTextureFace_callback()   { m_canvas.setTextureType(1);m_canvas.update(); }
    void privTextureNormal_callback() { m_canvas.setTextureType(2);m_canvas.update(); }
    void privTextureSynth_callback()   { m_canvas.setTextureType(3);m_canvas.update(); }
    void privTextureX_callback() { m_canvas.setTextureType(4); m_canvas.update(); }
    void privTextureY_callback() { m_canvas.setTextureType(5); m_canvas.update(); }
    void privTextureZ_callback() { m_canvas.setTextureType(6); m_canvas.update(); }

    void prviStreamPlayPause()
    {
        QAction& p = *m_menuActions["Play/pause"];
        InputStream& cs = InputStream::i();
        if (!cs.isLoaded())
        {
            //no stream
            p.setIcon(QIcon::fromTheme(":/icons/play"));
            return;
        }
        if (cs.get()->isPaused())
        {
            //play stream
            p.setIcon(QIcon::fromTheme(":/icons/pause"));
            cs.get()->start();
        }
        else
        {
            //pause stream
            p.setIcon(QIcon::fromTheme(":/icons/play"));
            cs.get()->pause();
        }

    }

    void privStream_realSense() {
        InputStream& cs = InputStream::i();
        bool ok;
        ok = cs.load<InputStream::RealSenseStream>();
        if (!ok)
        {
            GLpainter::i().setStatus("Failed to start stream");
            return;
        }
        m_canvas.resetView();
        ok = cs.start<InputStream::RealSenseStream>();
        if (!ok)
        {
            GLpainter::i().setStatus("Failed to start stream");
            return;
        }

        
    }

    void privOpen3DS_callback() {
        QString str = QFileDialog::getOpenFileName(          this, "Load .3ds file", QString(), "*.3ds");
        loadMeshFromFile(str);
       
    }
  
    std::string privGetFileName()
    {
        if (m_recordOutpuDir.empty())
            m_recordOutpuDir = QFileDialog::getExistingDirectory(this, "set output dirctory").toStdString();
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << m_recordOutpuDir << "/";
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
        ss << ".3ds";
        return ss.str();

    }
    void prviStreamRecord()
    {
        QAction& p = *m_menuActions["Record"];
        
        auto& os = OutputStream::i();
       
        if (os.isOpen())
        {
            os.stop();
            p.setIcon(QIcon::fromTheme(":/icons/record"));
        }
        else
        {        //build file name
            

            std::string fn = privGetFileName();
            os.start(fn);
          
            p.setIcon(QIcon::fromTheme(":/icons/recordStop"));
        }
    }
        
    void privExportSTL_callback()
    {
        QString str = QFileDialog::getSaveFileName(this, "save .stl file", QString(), "*.stl");
        Zstream::i().writeSTL(str.toStdString());
    }

public:

     ~Window()
    {
         for (auto p : m_menuActions)
         {
             delete p.second;

         }

    }
   
	explicit Window(QObject *parent = 0):m_parent(parent)
        
    {
        setCentralWidget(&m_canvas);
        //-----------------stream menu-----------------
        {
            auto menuH = menuBar()->addMenu(tr("&Stream"));
            menuH->addAction(privAddAction("From file", &Window::privOpen3DS_callback, QKeySequence::Open));
            menuH->addAction(privAddAction("From RealSense camera", &Window::privStream_realSense));
            auto exportMenu = menuH->addMenu("Export");
            exportMenu->addAction(privAddAction("to stl", &Window::privExportSTL_callback, tr("CTRL+S")));
           
        }
        //-----------------view menu-----------------
        {
            auto menuH = menuBar()->addMenu(tr("&View"));
            menuH->addAction(privAddAction("Trackball", &Window::privToggleViewTrackball_callback));
            //menuH->addAction(privAddAction("camera", &Window::privViewCameraToggle_callback));
        }

       

        //-----------------texture menu-----------------
        {
            auto menuH = menuBar()->addMenu(tr("&Texture"));
            menuH->addAction(privAddAction("Face", &Window::privTextureFace_callback, tr("CTRL+1")));
            menuH->addAction(privAddAction("Normal", &Window::privTextureNormal_callback, tr("CTRL+2")));
            menuH->addAction(privAddAction("Synthetic", &Window::privTextureSynth_callback, tr("CTRL+3")));
            menuH->addAction(privAddAction("CoordsX", &Window::privTextureX_callback, tr("CTRL+4")));
            menuH->addAction(privAddAction("CoordsY", &Window::privTextureY_callback, tr("CTRL+5")));
            menuH->addAction(privAddAction("CoordsZ", &Window::privTextureZ_callback, tr("CTRL+6")));
        }


        //-----------------help menu-----------------
        {
            auto menuH = menuBar()->addMenu(tr("&Help"));
            menuH->addAction(privAddAction("About", &Window::privAbout_callback));
        }
        //-----------------toolbar-----------------
        QToolBar *fileToolBar = addToolBar(tr("File"));
        
        fileToolBar->addAction(privAddAction("Play/pause", &Window::prviStreamPlayPause, Qt::Key_Space,":/icons/play"));
        fileToolBar->addSeparator();
        fileToolBar->addAction(privAddAction("Record", &Window::prviStreamRecord, QKeySequence::UnknownKey, ":/icons/record"));

        resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
        show();
        
        
    }
    void loadMeshFromFile(const QString& meshfn)
    {
        InputStream& cs = InputStream::i();
        std::string str = meshfn.toStdString();
        cs.load<InputStream::FileStream>(reinterpret_cast<void*>(&str));
        cs.start<InputStream::FileStream>();
        m_canvas.resetView();
        m_menuActions["Play/pause"]->setIcon(QIcon::fromTheme(":/icons/pause"));
        
        
    }
    void resetView()
    {
        m_canvas.resetView();
    }
    void canvasUpdate()
    {
        m_canvas.update();
    }
protected:

   
};