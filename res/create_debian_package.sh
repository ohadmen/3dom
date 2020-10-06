ver=1.4
#! /bin/bash
rm -rf pack
mkdir pack 
cd pack
taget_base=zview-$ver

target_dir=$taget_base/opt/zview
mkdir -p $target_dir
cp ../../bin/zview $target_dir
cp ../zview.png $target_dir


target_dir=$taget_base/usr/lib/x86_64-linux-gnu
mkdir -p $target_dir
cp -P ../../bin/libzview_inf.so* $target_dir

target_dir=$taget_base/usr/include/
mkdir -p $target_dir
cp ../../zview/interface/zview_inf.h $target_dir

target_dir=$taget_base/usr/share/applications/
mkdir -p $target_dir
cp ../zview.desktop $target_dir


target_dir=$taget_base/DEBIAN
mkdir -p $target_dir
control_file=$target_dir/control
echo "Source: zview" >> $control_file
echo "Version: 1.5" >> $control_file
echo "Section: visualization" >> $control_file
echo "Priority: optional" >> $control_file
echo "Maintainer: Ohad <ohad.men@gmail.com>" >> $control_file
echo "Build-Depends: debhelper (>= 10), autotools-dev" >> $control_file
echo "Standards-Version: 4.1.2" >> $control_file
echo "Homepage: https://github.com/ohadmen/zview" >> $control_file
echo "Package: zview" >> $control_file
echo "Architecture: amd64" >> $control_file
echo "Depends: qt5-default" >> $control_file
echo "Description: Generic mesh&cloud visualization tool" >> $control_file
echo " Zview - a general 3d view (version 1.5)" >> $control_file
echo " Zview was created as a tool to reflect the true state of 3d point cloud/mesh data stored in a file or on the heap." >> $control_file
echo " Implementation was written by modern c++ and OpenGL ES, with an effort to minimize cpu load and memory signiture." >> $control_file
echo " App can open a layered ply file - a ply file that contains multiple ply files, with hirarchical layer names." >> $control_file
echo " In addition, app support c++, Python, Matlab(WIP), and gdb(WIP) APIs to directly inject point clouds through inter-process communication" >> $control_file
echo "" >> $control_file
dpkg-deb --build $taget_base
dpkg-deb -c $taget_base.deb

rm -rf $taget_base


