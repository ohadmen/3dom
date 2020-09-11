TEMPLATE = subdirs
SUBDIRS = gui inf



gui.file = zview_gui.pro
inf.file = zview_inf.pro

INSTALL_ROOT=/opt/zview

target.path = $$INSTALL_ROOT
target.files += $$OUT_PWD/bin/zview
target.files += $$OUT_PWD/res/zview.png
target.files += $$OUT_PWD/zview/interface/zview_inf.h
target.files += $$OUT_PWD/bin/libzview_inf.so.1.0.0
target.files += $$OUT_PWD/bin/libzview_inf.so.1.0
target.files += $$OUT_PWD/bin/libzview_inf.so.1
target.files += $$OUT_PWD/bin/libzview_inf.so



INSTALLS += target
