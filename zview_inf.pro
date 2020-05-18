TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++20

TARGET = zview_interface
DESTDIR = ./bin
OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc


# Input
HEADERS += zview/interface/zview_inf.h
SOURCES += zview/interface/zview_inf.cpp