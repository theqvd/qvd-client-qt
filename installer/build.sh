#!/bin/bash
set -e

NXPROXY="nxproxy"
PULSEAUDIO="pulseaudio"
BINARY_QVD_DEPS=($NXPROXY $PULSEAUDIO)
SIGN_KEY="Apple Development: qvd@qindel.com (J2WDF6AV6S)"


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

if [ "$1" == "--skip-build" ] ; then
	shift
	skip_build=1
fi



rootdir=`pwd`

if [ -z "$skip_build" ] ; then
	build_dir=`mktemp -d`
else
	build_dir="$rootdir/build"
	mkdir -p "$build_dir"	
fi

inst_dir=`mktemp -d`
source_dir="$rootdir/.."
data="$rootdir/packages/com.qindel.qvd/data"
cert="$HOME/signing_certificate.p12"


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

#if type -P port > /dev/null 2>&1 ; then 
#   PORT_BIN_PREFIX=$(dirname `type -P port`)
#else
#   echo "You need to install MacPorts for validate some QVD Client Dependencies. (https://www.macports.org/)"
#   exit 1
#fi
   
#find_qvd_client_binary_deps



cd "$build_dir"
cmake "$rootdir../../qtclient/"
make -j24
make install DESTDIR="$inst_dir"

cd "$rootdir"

if [ -d "$data" ] ; then
	rm -rf "$data"
fi

mkdir -p "$data"

if [ -n "$OSX" ] ; then
	app="$data/QVD_Client.app"
	mkdir -p "$app" "$app/Contents" "$app/Contents/MacOS" "$app/Contents/Resources" "$app/Contents/Frameworks"

	cp -v "$source_dir/qtclient/gui/Info.plist"                "$app/Contents"
	mv -v "$inst_dir/usr/local/bin/QVD_Client"                 "$app/Contents/MacOS"
	mv -v "$inst_dir/usr/local/lib/libqvdclient.dylib"         "$app/Contents/Frameworks"
	mv -v "$inst_dir/usr/local/share/qvd.icns"                 "$app/Contents/Resources/QVD_Client.icns"

	install_name_tool -add_rpath "@executable_path/../Frameworks" "$app/Contents/MacOS/QVD_Client"

#	cp -Rpv $build_dir/gui/QVD_Client.app                   packages/com.qindel.qvd/data/
#	cp -v   $build_dir/libqvdclient/libqvdclient.dylib      packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/libqvdclient.1.dylib

#        cp -Rpv $PORT_BIN_PREFIX/$NXPROXY		      packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/$NXPROXY
#        cp -Rpv $PORT_BIN_PREFIX/$PULSEAUDIO                  packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/$PULSEAUDIO
#	install_name_tool -change "libqvdclient.1.dylib" "@executable_path/libqvdclient.1.dylib"  packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/QVD_Client
#       install_name_tool -change "$NXPROXY"    "@executable_path/$NXPROXY" packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/QVD_Client
#        install_name_tool -change "$PULSEAUDIO" "@executable_path/$PULSEAUDIO" packages/com.qindel.qvd/data/QVD_Client.app/Contents/MacOS/QVD_Client

	# Drag and drop install
	ln -s "/Applications" "$data"


	macdeployqt packages/com.qindel.qvd/data/QVD_Client.app -hardened-runtime -appstore-compliant -timestamp "-codesign=$SIGN_KEY"
#	codesign --timestamp -o runtime -s "$SIGN_KEY" --deep "$app"


else
	cp -v $build_dir/libqvdclient/libqvdclient.${LIB_EXT}*   packages/com.qindel.qvd/data/
	cp -v $build_dir/gui/$APP_PREFIX/QVD_Client              packages/com.qindel.qvd/data/
	cp -av $build_dir/gui/i18n                               packages/com.qindel.qvd/data/
fi

echo ================== Contents ===================
tree -C packages/com.qindel.qvd/

echo
echo
echo
#binarycreator -v -c config/config.xml -p packages qvd-client-installer

if [ -n "$OSX" ] ; then
	for format in UDBZ ; do # UDZO UDBZ ULFO ULMO ; do
		hdiutil create -volname "QVD Client" -srcfolder "$data" -ov -format $format qvd-client-installer-$format.dmg
	done
fi

rm -rf "$build_dir"
rm -rf "$inst_dir"

