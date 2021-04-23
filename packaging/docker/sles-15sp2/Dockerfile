FROM registry.suse.com/suse/sle15:15.2

# Prepare system
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1 installer-full-Media1
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Basesystem/ Module-Basesystem
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Development-Tools/ Module-Development-Tools
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Python2/ Module-Python2
RUN zypper ref && zypper update -y

# Base build tools
RUN zypper in -y rpm-build make 

# qvdclient-qt dependencies
RUN zypper in -y  libqt5-qtbase-devel libX11-devel libXt-devel libxkbfile-devel


# nxproxy dependencies
RUN zypper in -y zlib-devel libxml2-devel libpixman-1-0-devel libtool libtirpc-devel libpng16-devel libXtst-devel libjpeg62-devel libXrandr-devel libXpm-devel libXinerama-devel libXfont2-devel libXdmcp-devel libXcomposite-devel imake libexpat-devel automake autoconf python-devel quilt

RUN mkdir -p /root/rpmbuild/
RUN ln -s /usr/src/packages/{SOURCES,SPECS,BUILD,BUILDROOT,RPMS,SRPMS} /root/rpmbuild/
#COPY /tmp/centos-files/nx-libs.spec                /root/rpmbuild/SPECS
#COPY /tmp/centos-files/nx-libs-FreeFontNames.patch /root/rpmbuild/SOURCES
#COPY /tmp/centos-files/nx-libs-3.5.99.25.tar.gz    /root/rpmbuild/SOURCES

#PATH=/usr/lib64/qt5/bin:$PATH

