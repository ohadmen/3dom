#!/bin/bash
### kill shared memory
#for x in $(ipcs -m | awk 'NR>1 {print $2}'); do ipcrm -m $x; done

##-----------BUILD VIEWER-----------
if [[ $1 = "d" ]]; then
    echo "qmake debug.."
    qmake CONFIG+=debug zview_gui.pro 
else
    echo "qmake release.."
    qmake CONFIG+=release zview_gui.pro "DEFINES+=IDE_REVISION=$GIT_COMMIT" "QTC_PREFIX=/zview_bin" 
fi
echo "[-] running make (gui)"
make -j8 >/dev/null || make


##-----------BUILD INTERFACE-----------
if [[ $1 = "d" ]]; then
    echo "qmake debug.."
    qmake CONFIG+=debug zview_inf.pro
else
    echo "qmake release.."
    qmake CONFIG+=release zview_inf.pro "DEFINES+=IDE_REVISION=$GIT_COMMIT" "QTC_PREFIX=/zview_bin"
fi
echo "[-] running make (interface)"
make -j8 >/dev/null || make

