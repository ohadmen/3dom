#!/bin/bash
./build.sh
TAGET_FOLDER=/opt/zview
mkdir -p $TAGET_FOLDER
cp -f bin/zview $TAGET_FOLDER
cp -f bin/libzview_inf.so.1.0.0 $TAGET_FOLDER
cp -f res/zview.png $TAGET_FOLDER

