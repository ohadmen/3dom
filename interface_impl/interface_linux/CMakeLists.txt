cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_BUILD_TYPE Debug)

project (zview_inf_test)
add_executable(main zview_inf_test.cpp)
target_include_directories(main PRIVATE ../../zview/interface)

find_library(ZVIEW_LIBRARY zview_inf HINTS ../../bin)
target_link_libraries(main PUBLIC ${ZVIEW_LIBRARY})
