#include "zview_inf.h"
#include "zview_inf_impl.h"
#include "zview/io/read_file_list.h"
#include <QStringList>

ZviewIntrace::ZviewIntrace()
{
    zv = new ZviewInfImpl;
}
ZviewIntrace::~ZviewIntrace()
{
    delete zv;
}

class ShapeAddVisitor
{
    ZviewInfImpl *m_zv;

public:
    ShapeAddVisitor(ZviewInfImpl *zv) : m_zv(zv) {}
    int operator()(const Types::Pcl &obj)
     {return m_zv->addPointsColor(obj.getName().c_str(),obj.v().size(),&obj.v()[0]); }
    int operator()(const Types::Edges &obj) 
    {return m_zv->addEdgesColor(obj.getName().c_str(),obj.v().size(),&obj.v()[0],obj.e().size(),&obj.e()[0]); }
    int operator()(const Types::Mesh &obj)
    {return m_zv->addMeshColor(obj.getName().c_str(),obj.v().size(),&obj.v()[0],obj.f().size(),&obj.f()[0]); }
};

bool ZviewIntrace::loadFile(const char *filename)
{
    QStringList list;
    list.push_back(filename);
    std::vector<Types::Shape> objs = io::readFileList(list);
    if(objs.empty())
        return false;
    ShapeAddVisitor w(zv);
    for (const auto &objv : objs)
    {

        int retval = std::visit(w, objv);
        if(retval==-1)
        return false;
    }
    return true;
}

    int ZviewIntrace::addPoints(const char* name,size_t npoints,const float* xyz)
    {return zv->addPoints(name,npoints,xyz);}
    int ZviewIntrace::addPointsColor(const char* name,size_t npoints,const void* xyzrgba)
    {return zv->addPointsColor(name,npoints,xyzrgba);}
    int ZviewIntrace::addMesh(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices)
    {return zv->addMesh(name,npoints,xyz,nfaces,indices);}
    int ZviewIntrace::addMeshColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices)
    {return zv->addMeshColor(name,npoints,xyzrgba,nfaces,indices);}
    int ZviewIntrace::addEdges(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices)
    {return zv->addEdges(name,npoints,xyz,nfaces,indices);}
    int ZviewIntrace::addEdgesColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices)
    {return zv->addEdgesColor(name,npoints,xyzrgba,nfaces,indices);}