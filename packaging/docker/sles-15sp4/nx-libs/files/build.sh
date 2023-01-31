#!/bin/bash

cd /home/abuild
build --dist sles15sp4 --clean --nosignature --root /var/lib/obs-build/ /home/abuild/rpmbuild/SOURCES/nx-libs.spec

if [ $? -eq 0 ]; then
   cp -ar /var/lib/obs-build/home/abuild/rpmbuild/* /out/
fi

# Sign rpms
gpg --import /root/qvd-pub.gpg
gpg --import /root/qvd-secret.gpg
echo "%_gpg_name Quality Virtual Desktop <qvd@qindel.com>" > /root/.rpmmacros
find /out -name *.rpm -exec rpm --addsign {} \;
