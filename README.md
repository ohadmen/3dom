[![Build Status](https://travis-ci.com/ohadmen/zview.svg?branch=master)](https://travis-ci.com/ohadmen/zview)

# Zview -Generic mesh&cloud visualization tool
![Zview screenshot](/res/zview256.png)

## changelog version 1.41
	* get handlenum from string

## changelog version 1.4
	* debian packge support
	* expose keyboard interface through dll

## changelog version 1.33
	* print color data


## changelog version 1.32
	* fix normal direction
	* support for mesh vis through python
	* fix thread hang bug



## changelog version 1.31
	* depthmap shader
	* remove objects from python
	* camera control from python


## changelog version 1.3
	* interface bugfix
	* point picking with nan points

## changelog version 1.2
	* full python binding support
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
    * sudo apt install qt5-default build-essential
    * If needed:
	* sudo apt-get update && sudo apt-get install libgl1-mesa-dev
	* sudo rm /usr/lib/x86_64-linux-gnu/libGL.so 
	* sudo ln -s /usr/lib/libGL.so.1 /usr/lib/x86_64-linux-gnu/libGL.so 
  * for python dependencies:
  	* sudo apt-get install python-pybind11
	

* ./configure
* make
* make install
* bin/zview
### Windows
* Install qt https://www.qt.io/download
* build using qt creator
* from qt bin dir run windeployqt.exe PATH_TO_ZVIEW_GUI_EXE

![Zview screenshot](/res/screenshot.png)
