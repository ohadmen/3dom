QT += core gui  widgets


CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
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
    zview/zview.cpp
    zview/backend/vp_mat.cpp

HEADERS += \
    third_party/tinyply/source/tinyply.h \
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
    zview/io/writers/write_ply.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    zview/shaders/shaders.qrc

