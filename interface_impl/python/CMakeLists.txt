cmake_minimum_required(VERSION 2.8.12)
project(zview_bindings)

find_package(pybind11 REQUIRED)
pybind11_add_module(zview_bindings zview_bindings.cpp)

target_include_directories(zview_bindings PRIVATE ../../zview/interface)

find_library(ZVIEW_LIBRARY zview_inf HINTS ../../bin)
target_link_libraries(zview_bindings PUBLIC ${ZVIEW_LIBRARY})
