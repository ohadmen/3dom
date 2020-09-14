#!/bin/bash
### kill shared memory
#for x in $(ipcs -m | awk 'NR>1 {print $2}'); do ipcrm -m $x; done


if [[ $1 = "d" ]]; then
    echo "qmake debug.."
    qmake CONFIG+=debug zview.pro 
else
    echo "qmake release.."
    qmake CONFIG+=release zview.pro "DEFINES+=IDE_REVISION=$GIT_COMMIT" "QTC_PREFIX=/zview_bin" 

fi
echo "[-] running make"
make -j1 >/dev/null || make

cd interface_impl/linux/
./build.sh
