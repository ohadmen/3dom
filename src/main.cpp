#include <QApplication>

#include "app.h"

int main(int argc, char *argv[])
{
    App a(argc, argv);
	QSurfaceFormat format;
	format.setVersion(4, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	QSurfaceFormat::setDefaultFormat(format);
    return a.exec();
}
