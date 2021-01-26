#!/bin/bash

cd $HOME && mock --buildsrpm --spec ./nx-libs.spec --sources ./nx-libs-3.5.99.25.tar.gz --resultdir ~/result
mock --rebuild ~/result/nx-libs-3.5.99.25-0.0build1.el8.src.rpm --resultdir ~/result
cp -av ~/result/* /builddir/build/RPMS/
