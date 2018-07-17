#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QFileOpenEvent>
    
#include <QDebug>
#include "loader.h"
#include "Window.h"




class App : public QApplication
{
	Q_OBJECT


  
public:




	explicit App(int& argc, char *argv[]) :	QApplication(argc, argv),m_window()
	{

        QString ver("0.8");
        QSurfaceFormat format;
		format.setDepthBufferSize(24);
		QSurfaceFormat::setDefaultFormat(format);

		setApplicationName("3Dom (v"+ ver+")");
		setApplicationVersion(ver);
		setOrganizationName("Ohad");
		setOrganizationDomain("www.ohad.co");
    
    /*    QList<QScreen *> screens = QGuiApplication::screens();
        QList<QPixmap> pixmapsList;
        for (int i = 0; i<screens.size(); i++)
        {
            const QRect r = screens[i]->geometry();
            QPixmap pm = screens[i]->grabWindow(0, r.x(), r.y(), r.width(), r.height());
            pixmapsList.push_back(pm);
        }
        QClipboard *clipboard = QApplication::clipboard();

        clipboard->setPixmap(pixmapsList[1]);*/
        
        if (argc > 1)
        {

            m_window.loadMeshFromFile(argv[1]);
        }
        
		
        
        
		
	

	}
    

protected:


private:

	Window m_window;

};

#endif // APP_H
