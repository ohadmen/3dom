#!/bin/bash
./build.sh
TARGET_FOLDER=/opt/zview
mkdir -p $TARGET_FOLDER
cp -f bin/zview $TARGET_FOLDER
cp -f bin/libzview_inf.so.1.0.0 $TARGET_FOLDER
cp -f res/zview.png $TARGET_FOLDER

