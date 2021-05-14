#!/bin/sh
echo ..Building FRI3D Linux Setup

export QTDIR=/opt/Qt5.11.1/5.11/gcc_64/
qtdir=$QTDIR
QTBINDIR=$QTDIR/lib/
datafolder=../../../Data
targetfolder=./Generator/packages/FRI3D/data
externallibsfolder=../Build/Release
externallinuxlibsfolder=../External/Linux/lib

binarypath=$targetfolder/Libs/Neutrino

rm -fr $targetfolder
mkdir -p $targetfolder
mkdir -p $targetfolder/platforms

chmod 777 ./Generator/qtif_linux/binarycreator
chmod 777 ./Generator/qtif_linux/archivegen
chmod 777 ./Generator/qtif_linux/installerbase

echo ..Collecting files
cp ./Generator/Utility/nescene-mime.xml $targetfolder
cp ./Generator/Utility/neutrino.png $targetfolder
cp ./Generator/Utility/application-x-nescene.png $targetfolder
cp ./Generator/Utility/neutrino.sh $targetfolder
cp -r $datafolder/* $targetfolder

echo ...Libs
mkdir -p $targetfolder/Libs

cp $QTBINDIR/libQt5Core*.so $targetfolder/Libs
cp $QTBINDIR/libQt5Gui*.so $targetfolder/Libs
cp $QTBINDIR/libQt5OpenGL*.so $targetfolder/Libs
cp $QTBINDIR/libQt5Widgets*.so $targetfolder/Libs
cp $QTBINDIR/libQt5Network*.so $targetfolder/Libs
cp $QTBINDIR/libQt5Xml*.so $targetfolder/Libs
cp $QTBINDIR/libQt5Test*.so $targetfolder/Libs

cp -R $externallibsfolder/* $targetfolder/Libs
cp -R $externallinuxlibsfolder/* $targetfolder/Libs
cp $qtdir/lib/libQt5DBus.so.* $targetfolder/Libs
cp $qtdir/plugins/platforms/libqxcb.so $targetfolder/platforms
mv $binarypath $targetfolder

{
  chrpath -r \$ORIGIN/../Libs $targetfolder/platforms/libqxcb.so
#  ldd $targetfolder/Neutrino | grep "=> /" | awk '{print $3}' | grep -v '\/usr\/lib' | grep -v '\/lib/' | xargs -I '{}' cp -v '{}' $targetfolder/Libs
} &> /dev/null

tgl=$(date +'%Y-%m-%d')
binfilename=`echo $tgl`FRI3D_Linux_gcc48.run
echo ..Building FRI3D_Linux.run
mkdir -p Linux


./Generator/qtif_linux/repogen -p  Generator/packages/ FRI3D_Online
./Generator/qtif_linux/binarycreator --online-only -c Generator/config/config_x11_online.xml -p Generator/packages Linux/FRI3D_Linux.run

#./Generator/qtif_linux/binarycreator -c Generator/config/config_x11.xml -p Generator/packages Linux/`echo $binfilename`

echo Neutrino_Linux.run is generated under /Linux
