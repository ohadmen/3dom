# Zview -Generic mesh&cloud visualization tool
![Zview screenshot](/res/zview256.png)

## changelog version 1.1
* full c++ support on windows/Linux
	* add meshs,edges,points, and reading files API
	* camera movments API
	* save file API
	* usage example


## changelog version 1.0
* official first release!
	* ply support multi-type properties
	* can save/load layered ply files
	* interface over shared-memory with c++ interface


## changelog version 0.2
* deprecate bzel support
	* moved to branch
	* lack of cuda support, not staightforward to compile under windows

## changelog version 0.1
* support deletion from object list
* load obj/stl/ply file types
* save last visited directory
* add axes
* add grid
* enable drag and drop
* screenshot support

## how to run
### Linux
* Install qt:
    * sudo apt install qt5-default
* ./build.sh
* bin/zview
### Windows
* Install qt https://www.qt.io/download
* build using qt creator
* from qt bin dir run windeployqt.exe PATH_TO_ZVIEW_GUI_EXE

![Zview screenshot](/res/screenshot.png)