!/bin/bash

cd /home/abuild
build --clean --nosignature --root /var/lib/obs-build/ /home/abuild/rpmbuild/SOURCES/nx-libs.spec

if [ $? -eq 0 ]; then
   cp -ar /var/lib/obs-build/home/abuild/rpmbuild/* /out/
fi
