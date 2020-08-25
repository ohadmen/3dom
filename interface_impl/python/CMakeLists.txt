cmake_minimum_required(VERSION 2.8.12)
project(pyzview)

find_package(pybind11 REQUIRED)
pybind11_add_module(pyzview pyzview.cpp)

target_include_directories(pyzview PRIVATE ../../zview/interface)
find_library(ZVIEW_LIBRARY zview_inf HINTS ../../bin)
target_link_libraries(pyzview PUBLIC ${ZVIEW_LIBRARY})

