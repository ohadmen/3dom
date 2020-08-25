
#include <pybind11/pybind11.h>
#include "zview_inf.h"
#include <iostream>
namespace py = pybind11;
class ZviewInfWrapper
{
    ZviewInf* m_zvi;
    
    
    //non copyable
    ZviewInfWrapper(const ZviewInfWrapper&) = delete; 
    ZviewInfWrapper& operator=(const ZviewInfWrapper&) = delete;
public:
    ZviewInfWrapper() : m_zvi(ZviewInf::create())
     
     {}
    ~ZviewInfWrapper()
    {
        m_zvi->destroy();
        m_zvi = nullptr;
        
    }

    bool savePly(const char *fn) { return m_zvi->savePly(fn); }
    bool setCameraLookAt(float ex, float ey, float ez, float cx, float cy, float cz, float ux, float uy, float uz) { return m_zvi->setCameraLookAt(ex, ey, ez, cx, cy, cz, ux, uy, uz); }
    bool updatePoints(int key, size_t npoints, const float *xyz) { return m_zvi->updatePoints(key, npoints, xyz); }
    bool updateColoredPoints(int key, size_t npoints, const void *xyzrgba) { return m_zvi->updateColoredPoints(key, npoints, xyzrgba); }
    int addPoints(const char *name, size_t npoints, const std::vector<float>& xyz) { return m_zvi->addPoints(name, npoints, &xyz[0]); }
    int addColoredPoints(const char *name, size_t npoints, const void *xyzrgba) { return m_zvi->addColoredPoints(name, npoints, xyzrgba); }
    int addMesh(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices) { return m_zvi->addMesh(name, npoints, xyz, nfaces, indices); }
    int addMeshColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices) { return m_zvi->addMeshColor(name, npoints, xyzrgba, nfaces, indices); }
    int addEdges(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices) { return m_zvi->addEdges(name, npoints, xyz, nfaces, indices); }
    int addEdgesColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices) { return m_zvi->addEdgesColor(name, npoints, xyzrgba, nfaces, indices); }
    bool loadFile(const char *filename) {        return m_zvi->loadFile(filename);          }
    bool removeShape(int key) { return m_zvi->removeShape(key); }
};



PYBIND11_MODULE(pyzview, m)
{
    py::class_<ZviewInfWrapper>(m, "interface")
        .def(py::init())
        .def("savePly", &ZviewInfWrapper::savePly)
        .def("setCameraLookAt", &ZviewInfWrapper::setCameraLookAt)
        .def("updatePoints", &ZviewInfWrapper::updatePoints)
        .def("updateColoredPoints", &ZviewInfWrapper::updateColoredPoints)
        .def("addPoints", &ZviewInfWrapper::addPoints)
        .def("addColoredPoints", &ZviewInfWrapper::addColoredPoints)
        .def("addMesh", &ZviewInfWrapper::addMesh)
        .def("addMeshColor", &ZviewInfWrapper::addMeshColor)
        .def("addEdges", &ZviewInfWrapper::addEdges)
        .def("addEdgesColor", &ZviewInfWrapper::addEdgesColor)
        .def("loadFile", &ZviewInfWrapper::loadFile)
        .def("removeShape", &ZviewInfWrapper::removeShape);
}