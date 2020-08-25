cmake_minimum_required(VERSION 3.1)
project(pyzview)
set(CMAKE_CXX_STANDARD 14)
find_package(pybind11 REQUIRED)
pybind11_add_module(pyzview pyzview.cpp)

target_include_directories(pyzview PRIVATE ../../zview/interface)
find_library(ZVIEW_LIBRARY zview_inf HINTS ../../bin)
target_link_libraries(pyzview PUBLIC ${ZVIEW_LIBRARY})

