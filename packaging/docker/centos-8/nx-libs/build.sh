#!/bin/bash

su - nxbuilder -c "cd /home/nxbuilder && mock --buildsrpm --spec ./nx-libs.spec --sources ./nx-libs-3.5.99.25.tar.gz --resultdir /home/nxbuilder/result"
su - nxbuilder -c "mock --rebuild /home/nxbuilder/result/nx-libs-3.5.99.25-0.0build1.el8.src.rpm --resultdir /home/nxbuilder/result"
cp -av /home/nxbuilder/result/* /builddir

gpg --import /root/qvd-pub.gpg
gpg --import /root/qvd-secret.gpg
echo "%_gpg_name Quality Virtual Desktop <qvd@qindel.com>" > /root/.rpmmacros

find /builddir -name '*.rpm' -exec rpm --addsign {} \;
