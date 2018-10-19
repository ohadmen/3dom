cmake_minimum_required(VERSION 3.1.0)

project(testproject)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)



# Create code from a list of Qt designer ui files
# set(CMAKE_AUTOUIC ON)
# set (CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)


# Populate a CMake variable with the sources
set(SOURCE_FILES src/main.cpp )
set(HEADER_FILES src/App src/Array2d src/Backdrop src/Canvas src/GLpainter src/loader src/MeasurmentGrid src/mesh src/Params src/Qmvp src/TrackBall src/TrackStates src/TrackUtils src/VertMap src/Window src/Q3d/QBitFlags  src/Q3d/QLine3D  src/Q3d/QPlane3D  src/Q3d/QPrimitive3D  src/Q3d/QRay3D  src/Q3d/QSphere3D  src/Q3d/QText3D  )

# Tell CMake to create the zv executable
add_executable(zv ${SOURCE_FILES} ${HEADER_FILES})



# Use the Widgets module from Qt 5
target_link_libraries(zv Qt5::Widgets)
target_include_directories(zv PRIVATE src/)

add_custom_command(TARGET zv PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:zv>/res)

