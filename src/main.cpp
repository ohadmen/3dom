

#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>



#include "Canvas.h"
#include "loader.h"



int main(int argc, char *argv[])
{
	
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("cube");
    app.setApplicationVersion("0.1");
	Loader m_loader;
	int token = m_loader.load(Loader::default3DmodelFilename());

#ifndef QT_NO_OPENGL
    Canvas widget;
	
    widget.show();
	widget.setToken(token);

#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}
