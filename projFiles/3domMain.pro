QT += core gui opengl widgets

TARGET = 3dom
TEMPLATE = app

# Bump optimization up to -O3 in release builds
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += \
    ../src/main.cpp\

HEADERS  += \
    ../src/app.h\
    ../src/canvas.h \
    ../src/mesh.h \
    ../src/glmesh.h \
    ../src/loader.h \
    ../src/window.h \
    ../src/backdrop.h

CONFIG += c++11

RESOURCES += \
    qt.qrc \
    ../gl/gl.qrc

macx {
    QMAKE_INFO_PLIST = ../app/Info.plist
    ICON = ../app/3dom.icns
}

win32 {
    RC_FILE = ../exe/3dom.rc
}

linux {
    target.path = /usr/bin
    INSTALLS += target
}

static {
    CONFIG += static
}

LIBS += -lOpenGL32
