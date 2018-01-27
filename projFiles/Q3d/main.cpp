#include "QGLMainWindow.h"
#include <QtWidgets/QApplication>

//#include <vld.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CALDLMainWindow w;
    w.show();
    return a.exec();
}
