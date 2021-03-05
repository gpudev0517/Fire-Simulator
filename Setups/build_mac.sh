echo ..Building FRI3D Setup
echo ..QTDIR = $QTDIR
qtversion=5_11_0
qtpluginsdir=$QTDIR/plugins
datafolder=../../../Data
binarypath=../../build-Neutrino-Release/Neutrino.app
targetfolder=./Generator/packages/Neutrino/data
appbundlefolder=$targetfolder/neutrino.app
appdatafolder=$appbundlefolder/Contents/MacOS
frameworksfolder=$appbundlefolder/Contents/Frameworks
externallibsfolder=../External/lib/release

targetpluginsdir=$appbundlefolder/Contents/Plugins
macdeploy=$qtdir/bin/macdeployqt

rm -rf $appbundlefolder
rm -rf Mac/FRI3D_Mac

mkdir -p $targetfolder
mkdir -p $appbundlefolder
mkdir -p $appbundlefolder/Contents/MacOS/platforms
mkdir -p /usr/plugins/platforms

chmod 777 ./Generator/qtif_mac/binarycreator
chmod 777 ./Generator/qtif_mac/archivegen
chmod 777 ./Generator/qtif_mac/installerbase

echo ..Collecting files

cp -r $binarypath/* $appbundlefolder
cp -r $datafolder/* $appdatafolder

cp -r $externallibsfolder/* /usr/lib
cp $qtdir/plugins/platforms/libqcocoa.dylib /usr/plugins/platforms/libqcocoa.dylib
cp $qtdir/plugins/platforms/libqcocoa.dylib /usr/plugins/platforms/libcocoaprintersupport.dylib
$macdeploy $appbundlefolder


cp $externallibsfolder/* $frameworksfolder
cp ./Generator/Utility/neutrino.sh $appdatafolder
cp ./Generator/Utility/Info.plist $appbundlefolder/Contents
cp ./Generator/Utility/nescenefile.icns $appbundlefolder/Contents/Resources
cp $qtdir/plugins/platforms/libqcocoa.dylib $appbundlefolder/Contents/MacOS/platforms

echo ..Building FRI3D_Mac.app
./Generator/qtif_mac/binarycreator -c Generator/config/config_mac.xml -p Generator/packages Mac/FRI3D_Mac

echo FRI3D_Mac.app is generated under /Mac
