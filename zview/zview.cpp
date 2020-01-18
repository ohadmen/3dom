#include <QtWidgets/QApplication>
#include "zview/gui/main_window.h"


int main( int argc, char **argv )
{
    QApplication app(argc, argv);
    MainWindow win;
    win.show();
    return app.exec();
}