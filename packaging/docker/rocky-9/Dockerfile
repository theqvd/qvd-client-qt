FROM rockylinux:9.1

# Prepare system
RUN dnf update -y
RUN dnf config-manager --enable crb
RUN dnf install -y epel-release

# Base build tools
RUN dnf install -y rpm-build make cmake rpm-sign expect

# qvdclient-qt dependencies
RUN dnf install -y qt5-qtbase-devel qt5-qtcharts-devel qt5-linguist libX11-devel libXt-devel libxkbfile-devel


# nxproxy dependencies
RUN dnf install -y zlib-devel pixman-devel libxml2-devel libtool libtirpc-devel libpng-devel libXtst-devel libjpeg-devel libXrandr-devel libXpm-devel libXinerama-devel libXfont2-devel libXfixes-devel libXdmcp-devel libXdamage-devel libXcomposite-devel imake expat-devel automake autoconf platform-python-devel which quilt

RUN mkdir -p /root/rpmbuild/{SOURCES,SPECS,BUILD,BUILDROOT,RPMS,SRPMS}
#COPY /tmp/centos-files/nx-libs.spec                /root/rpmbuild/SPECS
#COPY /tmp/centos-files/nx-libs-FreeFontNames.patch /root/rpmbuild/SOURCES
#COPY /tmp/centos-files/nx-libs-3.5.99.25.tar.gz    /root/rpmbuild/SOURCES

#PATH=/usr/lib64/qt5/bin:$PATH

RUN mkdir /files
