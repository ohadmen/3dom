QT += core 

DEFINES += QT_NO_VERSION_TAGGING

TEMPLATE = lib
CONFIG += c++17 
QMAKE_CXXFLAGS += -std=c++17

TARGET = zview_inf
DESTDIR = ./bin
OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc

win32:createdir.commands += $(CHK_DIR_EXISTS) $$OBJECTS_DIR(DESTDIR) & $(MKDIR) $$shell_path(DESTDIR)
linux:createdir.commands += $$shell_path(DESTDIR) & $(MKDIR) $$shell_path(DESTDIR)

win32:createdir.commands += $(CHK_DIR_EXISTS) $$OBJECTS_DIR(OBJECTS_DIR) & $(MKDIR) $$shell_path(OBJECTS_DIR)
linux:createdir.commands += $$shell_path(OBJECTS_DIR) & $(MKDIR) $$shell_path(OBJECTS_DIR)

win32:createdir.commands += $(CHK_DIR_EXISTS) $$OBJECTS_DIR(MOC_DIR) & $(MKDIR) $$shell_path(MOC_DIR)
linux:createdir.commands += $$shell_path(MOC_DIR) & $(MKDIR) $$shell_path(MOC_DIR)

win32:createdir.commands += $(CHK_DIR_EXISTS) $$OBJECTS_DIR(RCC_DIR) & $(MKDIR) $$shell_path(RCC_DIR)
linux:createdir.commands += $$shell_path(RCC_DIR) & $(MKDIR) $$shell_path(RCC_DIR)






# Input

SOURCES += \
    zview/interface/zview_inf_impl.cpp\
    zview/interface/zview_inf.cpp\
    zview/io/read_file_list.cpp \
    zview/io/readers/read_obj.cpp \
    zview/io/readers/read_ply.cpp \
    zview/io/readers/read_stl.cpp \
    zview/io/write_shape_to_file.cpp \



HEADERS += \
    zview/interface/zview_inf.h\
    zview/interface/zview_inf_impl.h\
    zview/io/read_file_list.h \
    zview/io/readers/read_obj.h \
    zview/io/readers/read_ply.h \
    zview/io/readers/read_stl.h \
    zview/io/write_shape_to_file.h \

