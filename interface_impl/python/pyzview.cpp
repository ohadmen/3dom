
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "zview_inf.h"
#include <iostream>

namespace py = pybind11;

class ZviewInfWrapper
{
    
    ZviewInf* m_zvi;
    
    
    //non copyable
    ZviewInfWrapper(const ZviewInfWrapper&) = delete; 
    ZviewInfWrapper& operator=(const ZviewInfWrapper&) = delete;

    static float* arr2ptr(const py::array_t<float>& arr)
    {
        py::buffer_info buff = arr.request();
        return static_cast<float*>(buff.ptr);
    }

    size_t getrows(const py::array_t<float>& arr)
    {
        py::buffer_info buff = arr.request();
        return buff.shape[0];
    }
    size_t getcols(const py::array_t<float>& arr)
    {
        py::buffer_info buff = arr.request();
        return buff.shape[1];
    }
    size_t getNdims(const py::array_t<float>& arr)
    {
        py::buffer_info buff = arr.request();
        return buff.shape.size();
    }
    void dataChk(const py::array_t<float>& arr,size_t expectedCols)
    {
        if(getNdims(arr)!=2)
        {
            throw std::runtime_error("data should be 2d matrix");
        }
        if(getcols(arr)!=expectedCols)
        {
            throw std::runtime_error("data should be Nx"+std::to_string(expectedCols));
        }
    }

public:
    ZviewInfWrapper() : m_zvi(ZviewInf::create())
     
     {}
    ~ZviewInfWrapper()
    {
        m_zvi->destroy();
        m_zvi = nullptr;
        
    }

    bool savePly(const char *fn) { return m_zvi->savePly(fn); }
    int addPoints(const char *name, const py::array_t<float>& xyz) {dataChk(xyz,3); return m_zvi->addPoints(name, getrows(xyz), arr2ptr(xyz)); }
    int addColoredPoints(const char *name, py::array_t<float>&xyzrgba) {dataChk(xyzrgba,4); return m_zvi->addColoredPoints(name, getrows(xyzrgba), arr2ptr(xyzrgba)); }
    bool updatePoints(int key, py::array_t<float>&xyz) {dataChk(xyz,3);  return m_zvi->updatePoints(key,getrows(xyz), arr2ptr(xyz)); }
    bool updateColoredPoints(int key, py::array_t<float>&xyzrgba) {dataChk(xyzrgba,4);  return m_zvi->updateColoredPoints(key, getrows(xyzrgba), arr2ptr(xyzrgba)); }

    // int addMesh(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices) { return m_zvi->addMesh(name, npoints, xyz, nfaces, indices); }
    // int addMeshColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices) { return m_zvi->addMeshColor(name, npoints, xyzrgba, nfaces, indices); }
    // int addEdges(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices) { return m_zvi->addEdges(name, npoints, xyz, nfaces, indices); }
    // int addEdgesColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices) { return m_zvi->addEdgesColor(name, npoints, xyzrgba, nfaces, indices); }
    bool loadFile(const char *filename) {        return m_zvi->loadFile(filename);          }
    bool removeShape(int key) { return m_zvi->removeShape(key); }
    bool setCameraLookAt(py::array_t<float>& e,py::array_t<float>& c,py::array_t<float>& u)
    {
        float* ep = arr2ptr(e);
        float* cp = arr2ptr(c);
        float* up = arr2ptr(u);
        return m_zvi->setCameraLookAt(ep[0],ep[1],ep[2],cp[0],cp[1],cp[2],up[0],up[1],up[2]);
    }
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
        // .def("addMesh", &ZviewInfWrapper::addMesh)
        // .def("addMeshColor", &ZviewInfWrapper::addMeshColor)
        // .def("addEdges", &ZviewInfWrapper::addEdges)
        // .def("addEdgesColor", &ZviewInfWrapper::addEdgesColor)
        .def("addColoredPoints", &ZviewInfWrapper::addColoredPoints)
        .def("loadFile", &ZviewInfWrapper::loadFile)
        .def("removeShape", &ZviewInfWrapper::removeShape);
}