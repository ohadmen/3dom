
QT += core gui  widgets

CONFIG += c++17 
QMAKE_CXXFLAGS += -std=c++17

TARGET = zview
DESTDIR = ./bin
OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc

SOURCES += \
    zview/interface/zview_inf_impl.cpp\
    zview/backend/shared_memory/cmd_query_thread.cpp \
    zview/backend/shared_memory/shared_memory_manager.cpp \
    zview/backend/state_machine/state_machine.cpp \
    zview/backend/state_machine/track_state_abs.cpp \
    zview/backend/state_machine/track_state_idle.cpp \
    zview/backend/state_machine/track_state_measure_distance.cpp \
    zview/backend/state_machine/track_state_retarget.cpp \
    zview/backend/state_machine/track_state_rotate.cpp \
    zview/backend/state_machine/track_state_translate.cpp \
    zview/backend/state_machine/track_state_zoom.cpp \
    zview/backend/tree_model/tree_item.cpp \
    zview/backend/tree_model/tree_model.cpp \
    zview/backend/tree_model/tree_view_signaled.cpp \
    zview/backend/vp_mat.cpp \
    zview/gui/canvas.cpp \
    zview/gui/drawables/axes.cpp \
    zview/gui/drawables/backdrop.cpp \
    zview/gui/drawables/drawable_base.cpp \
    zview/gui/drawables/drawable_common.cpp \
    zview/gui/drawables/drawable_edges.cpp \
    zview/gui/drawables/drawable_mesh.cpp \
    zview/gui/drawables/drawable_pcl.cpp \
    zview/gui/drawables/drawables_buffer.cpp \
    zview/gui/drawables/grid.cpp \
    zview/gui/main_window.cpp \
    zview/io/read_file_list.cpp \
    zview/io/readers/read_obj.cpp \
    zview/io/readers/read_ply.cpp \
    zview/io/readers/read_stl.cpp \
    zview/io/write_shape_to_file.cpp \
    zview/gui/zview_main.cpp
    zview/backend/vp_mat.cpp

HEADERS += \
    zview/interface/zview_inf_impl.h\
    zview/backend/shared_memory/cmd_query_thread.h \
    zview/backend/shared_memory/shared_memory_manager.h \
    zview/backend/state_machine/state_machine.h \
    zview/backend/state_machine/track_state_abs.h \
    zview/backend/state_machine/track_state_idle.h \
    zview/backend/state_machine/track_state_measure_distance.h \
    zview/backend/state_machine/track_state_retarget.h \
    zview/backend/state_machine/track_state_rotate.h \
    zview/backend/state_machine/track_state_translate.h \
    zview/backend/state_machine/track_state_zoom.h \
    zview/backend/tree_model/tree_item.h \
    zview/backend/tree_model/tree_model.h \
    zview/backend/tree_model/tree_view_signaled.h \
    zview/backend/vp_mat.h \
    zview/common/common_types.h \
    zview/common/params.h \
    zview/gui/canvas.h \
    zview/gui/drawables/axes.h \
    zview/gui/drawables/backdrop.h \
    zview/gui/drawables/drawable_base.h \
    zview/gui/drawables/drawable_common.h \
    zview/gui/drawables/drawable_edges.h \
    zview/gui/drawables/drawable_mesh.h \
    zview/gui/drawables/drawable_pcl.h \
    zview/gui/drawables/drawables_buffer.h \
    zview/gui/drawables/grid.h \
    zview/gui/main_window.h \
    zview/io/read_file_list.h \
    zview/io/readers/read_obj.h \
    zview/io/readers/read_ply.h \
    zview/io/readers/read_stl.h \
    zview/io/write_shape_to_file.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    zview/shaders/shaders.qrc\
    res/res.qrc


#----------CUDA-----------
#CUDA_SOURCES += zview/gui/drawables/picking.cc
#CUDA_ARCH = sm_75
#unix:
#{
#
#CUDA_DIR = /usr/local/cuda
#
#NVCCFLAGS = --compiler-options -use-fast-math --Wno-deprecated-gpu-targets
#INCLUDEPATH += $$CUDA_DIR/include
#QMAKE_LIBDIR += $$CUDA_DIR/lib64
#LIBS += -lcudart
#CUDA_INC = $$join(INCLUDEPATH,' -I','-I',' ')
#cuda.input = CUDA_SOURCES
#cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.o
#cuda.commands = $$CUDA_DIR/bin/nvcc -m64 -g -G -arch=$$CUDA_ARCH -c $$NVCCFLAGS $$CUDA_INC $$LIBS  ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
#cuda.dependency_type = TYPE_C
#cuda.depend_command = $$CUDA_DIR/bin/nvcc -g -G -M $$CUDA_INC $$NVCCFLAGS   ${QMAKE_FILE_NAME}
#QMAKE_EXTRA_UNIX_COMPILERS += cuda
#}
#win32:
#{
#
#}
#