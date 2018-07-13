#define QT_NO_FOREACH 
#include "librealsense2/rs.hpp" // Include RealSense Cross Platform API

#include <future>
#include <thread>
#include <chrono>
#include <functional>


#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>
#include "App.h"


#include "Canvas.h"
#include "loader.h"





int main(int argc, char *argv[])
{
   

    App app(argc, argv);
   
    int r = app.exec();
   
    return r;


    
   
    
}
