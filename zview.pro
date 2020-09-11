TEMPLATE = subdirs
SUBDIRS = gui inf


gui.file = zview_gui.pro
inf.file = zview_inf.pro

target.path = /opt/zview
target.files += $$OUT_PWD/bin/zview
target.files += $$OUT_PWD/zview/interface/zview_inf.h
target.files += $$OUT_PWD/bin/libzview_inf.so.1.0.0
#target.files += $$OUT_PWD/bin/libzview_inf.so.1.0
#target.files += $$OUT_PWD/bin/libzview_inf.so.1
#target.files += $$OUT_PWD/bin/libzview_inf.so
target.files += $$OUT_PWD/res/zview.png
target.extra = ln -s $$target.path/libzview_inf.so.1.0.0 $$target.path/libzview_inf.so.1.0 && \
               ln -s $$target.path/libzview_inf.so.1.0.0 $$target.path/libzview_inf.so.1 && \
               ln -s $$target.path/libzview_inf.so.1.0.0 $$target.path/libzview_inf.so

message($$target.files)

INSTALLS += target
