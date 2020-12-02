#!/bin/bash
set -e

if [ `uname` == "Darwin" ] ; then
	QT_VER=5.15.2
	QT_DIR="$HOME/Qt/"
	QT_BIN_DIR="$QT_DIR/$QT_VER/clang_64/bin"
	QT_INSTALLER_DIR="$QT_DIR/Tools/QtInstallerFramework/4.0/bin"
	LIB_EXT="dylib"
	OSX=1
else
	QT_VER=
	QT_DIR=
	QT_BIN_DIR="/usr/lib64/qt5/bin"
	QT_INSTALLER_DIR="$QT_BIN_DIR"
	LIB_EXT="so"
	APP_PREFIX=""
	OSX=""
fi

export PATH=$QT_BIN_DIR:$QT_INSTALLER_DIR:$PATH

tempdir=`mktemp -d`
rootdir=`pwd`

cd "$tempdir"

qmake "$rootdir../../qtclient/QVD_Client.pro"
make -j24
cd "$rootdir"

if [ -d "packages/com.qindel.qvd/data" ] ; then
	rm -rf packages/com.qindel.qvd/data/
fi

mkdir -p packages/com.qindel.qvd/data

if [ -n "$OSX" ] ; then
	cp -Rpv $tempdir/gui/QVD_Client.app                   packages/com.qindel.qvd/data/
        cp -v    $tempdir/libqvdclient/libqvdclient.dylib      packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/libqvdclient.1.dylib
	install_name_tool -change "libqvdclient.1.dylib" "@executable_path/libqvdclient.1.dylib"  packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/QVD_Client
	macdeployqt packages/com.qindel.qvd/data/QVD_Client.app
else
	cp -v $tempdir/libqvdclient/libqvdclient.${LIB_EXT}*   packages/com.qindel.qvd/data/
	cp -v $tempdir/gui/$APP_PREFIX/QVD_Client              packages/com.qindel.qvd/data/
fi

echo ================== Contents ===================
tree -C packages/com.qindel.qvd/

echo
echo
echo
binarycreator -v -c config/config.xml -p packages qvd-client-installer

if [ -n "$OSX" ] ; then
	for format in UDZO UDBZ ULFO ULMO ; do
		hdiutil create -volname "QVD Client" -srcfolder packages/com.qindel.qvd/data/QVD_Client.app -ov -format $format qvd-client-installer-$format.dmg
	done
fi

rm -rf "$tempdir"

