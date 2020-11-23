#!/bin/bash
set -e

tempdir=`mktemp -d`
rootdir=`pwd`

cd "$tempdir"

qmake "$rootdir../../qtclient/QVD_Client.pro"
make -j24
cd "$rootdir"

rm -f packages/com.qindel.qvd/data/QVD_Client
rm -f packages/com.qindel.qvd/data/libqvdclient*

cp -v $tempdir/libqvdclient/libqvdclient.so*   packages/com.qindel.qvd/data/
cp -v $tempdir/gui/QVD_Client                  packages/com.qindel.qvd/data/

/usr/lib64/qt5/bin/binarycreator -v -c config/config.xml -p packages qvd-client-installer


rm -rf "$tempdir"

