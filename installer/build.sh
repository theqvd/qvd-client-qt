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

NXPROXY="nxproxy"
PULSEAUDIO="pulseaudio"
BINARY_QVD_DEPS=($NXPROXY $PULSEAUDIO)

error_deps () {
	echo "$1 is a QVD Client dependency, you can install it using MacPorts like this 'sudo port install $1'. (https://www.macports.org/)"
        exit 2
}

find_qvd_client_binary_deps () {
   for DEP in "${BINARY_QVD_DEPS[@]}" ; do
       INSTALLED=$(port -q installed $DEP)
       [ -n "$INSTALLED" ] || error_deps $DEP
   done
}

if type -P port > /dev/null 2>&1 ; then 
   PORT_BIN_PREFIX=$(dirname `type -P port`)
else
   echo "You need to install MacPorts for validate some QVD Client Dependencies. (https://www.macports.org/)"
   exit 1
fi
   
find_qvd_client_binary_deps

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
	cp -v   $tempdir/libqvdclient/libqvdclient.dylib      packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/libqvdclient.1.dylib
        cp -Rpv $PORT_BIN_PREFIX/$NXPROXY		      packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/$NXPROXY
        cp -Rpv $PORT_BIN_PREFIX/$PULSEAUDIO                  packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/$PULSEAUDIO
	install_name_tool -change "libqvdclient.1.dylib" "@executable_path/libqvdclient.1.dylib"  packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/QVD_Client
        install_name_tool -change "$NXPROXY"    "@executable_path/$NXPROXY" packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/QVD_Client
        install_name_tool -change "$PULSEAUDIO" "@executable_path/$PULSEAUDIO" packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/QVD_Client
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
	for format in UDBZ ; do # UDZO UDBZ ULFO ULMO ; do
		hdiutil create -volname "QVD Client" -srcfolder packages/com.qindel.qvd/data/QVD_Client.app -ov -format $format qvd-client-installer-$format.dmg
	done
fi

rm -rf "$tempdir"

