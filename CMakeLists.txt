cmake_minimum_required(VERSION 3.1.0)

project(testproject)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set (CMAKE_CXX_STANDARD 11)
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)

# Populate a CMake variable with the sources
set(helloworld_SRCS
    src/main.cpp
)
set(HEADER_FILES src/App src/Array2d src/Backdrop src/Canvas src/GLpainter src/loader src/MeasurmentGrid src/mesh src/Params src/Qmvp src/TrackBall src/TrackStates src/TrackUtils src/VertMap src/Window )

# Tell CMake to create the helloworld executable
add_executable(helloworld ${helloworld_SRCS} ${HEADER_FILES})
# Use the Widgets module from Qt 5
target_link_libraries(helloworld Qt5::Widgets)
target_include_directories(helloworld PRIVATE src/)
