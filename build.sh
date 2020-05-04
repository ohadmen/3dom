#!/bin/bash
rm -rf bin
mkdir bin
cd bin
if [[ $1 = "d" ]]; then
    echo "qmake debug.."
    qmake CONFIG+=debug ../zview.pro
else
    echo "qmake release.."
    qmake CONFIG+=release ../zview.pro "DEFINES+=IDE_REVISION=$GIT_COMMIT" "QTC_PREFIX=/zview_bin"
fi
echo "running make"
make -j8 >/dev/null || make
echo "done"