


#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>
#include "App.h"


#include "Canvas.h"
#include "loader.h"



int main(int argc, char *argv[])
{
    App app(argc, argv);
   // QApplication app(argc, argv);
   // QSurfaceFormat format;
   // format.setDepthBufferSize(24);
   // QSurfaceFormat::setDefaultFormat(format);
   //
   // app.setApplicationName("3Dom");
   // app.setApplicationVersion("0.1");
   // Loader m_loader;
   // int token = m_loader.load(Loader::default3DmodelFilename());
   //
   //
   // Canvas widget;
   // 
   // widget.show();
   // 
   // widget.setToken(token);
   // widget.cam2geometry();


    return app.exec();
}
