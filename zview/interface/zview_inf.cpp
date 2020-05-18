#include "zview_inf.h"
#include "zview_inf_impl.h"
#include "zview/io/read_file_list.h"
#include <QDebug>
ZviewIntrace::ZviewIntrace()
 {
     zv = new ZviewInfImpl;
 }
ZviewIntrace::~ZviewIntrace()
 {
     delete zv;
 }

void ZviewIntrace::loadFile(const char *filename)
{
    QStringList list;
    list.push_back(filename);
    std::vector <Types::Shape> objs = io::readFileList(list);
    qDebug() << "loaded file ";
    
}