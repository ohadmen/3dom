# Zview -Generic mesh&cloud visualization tool

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
* install Bzael:
    * https://docs.bazel.build/versions/master/install-ubuntu.html
* Install qt:
    * sudo apt install qt5-default
* git clone git@github.com:ohadmen/zview.git
* cd zview
* bazel run zview