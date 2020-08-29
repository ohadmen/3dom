
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

    template<class T>
    static T* arr2ptr(const py::array_t<T>& arr)
    {
        py::buffer_info buff = arr.request();
        return static_cast<T*>(buff.ptr);
    }

    template<class T>
    size_t getrows(const py::array_t<T>& arr)
    {
        py::buffer_info buff = arr.request();
        return buff.shape[0];
    }
    template<class T>
    size_t getcols(const py::array_t<T>& arr)
    {
        py::buffer_info buff = arr.request();
        return buff.shape[1];
    }

    template<class T>
    size_t getNdims(const py::array_t<T>& arr)
    {
        py::buffer_info buff = arr.request();
        return buff.shape.size();
    }
    template<class T>
    void dataChk(const py::array_t<T>& arr,size_t expectedCols)
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

    int addMesh(const char *name, py::array_t<float>& xyz, py::array_t<int32_t>& indices) {dataChk(xyz,3); dataChk(indices,3);  return m_zvi->addMesh(name, getrows(xyz), arr2ptr(xyz), getrows(indices), arr2ptr(indices)); }
    int addColoredMesh(const char *name, py::array_t<float>&xyzrgba, py::array_t<int32_t>& indices) {dataChk(xyzrgba,4); dataChk(indices,3);  return m_zvi->addColoredMesh(name, getrows(xyzrgba), arr2ptr(xyzrgba),getrows(indices), arr2ptr(indices)); }

    int addEdges(const char *name,  py::array_t<float>& xyz, py::array_t<int32_t>& indices) {dataChk(xyz,3); dataChk(indices,2); return m_zvi->addEdges(name,getrows(xyz), arr2ptr(xyz), getrows(indices), arr2ptr(indices)); }
    int addColoredEdges(const char *name, py::array_t<float>&xyzrgba, py::array_t<int32_t>& indices) {dataChk(xyzrgba,4); dataChk(indices,2); return m_zvi->addColoredEdges(name, getrows(xyzrgba), arr2ptr(xyzrgba),getrows(indices), arr2ptr(indices)); }
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
        .def("addMesh", &ZviewInfWrapper::addMesh)
        .def("addColoredMesh", &ZviewInfWrapper::addColoredMesh)
        .def("addEdges", &ZviewInfWrapper::addEdges)
        .def("addColoredEdges", &ZviewInfWrapper::addColoredEdges)
        .def("loadFile", &ZviewInfWrapper::loadFile)
        .def("removeShape", &ZviewInfWrapper::removeShape);
}