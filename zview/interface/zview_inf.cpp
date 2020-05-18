#include "zview_inf.h"
#include <QDebug>
ZviewIntrace::ZviewIntrace() {}

void ZviewIntrace::loadFile(const char *filename)
{
    qDebug() << "loadfile " << filename;
}