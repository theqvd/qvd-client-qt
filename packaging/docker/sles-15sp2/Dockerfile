FROM registry.suse.com/suse/sle15:15.2

# Prepare system
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1 installer-full-Media1
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Basesystem/ Module-Basesystem
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Development-Tools/ Module-Development-Tools
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Python2/ Module-Python2
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Desktop-Applications/ Module-Desktop-Applications
RUN zypper ar -G -K http://download.opensuse.org/repositories/openSUSE:/Backports:/SLE-15-SP2/standard/ Backports\ SLE-15-SP2
RUN zypper ref && zypper update -y

# Base build tools
RUN zypper in -y rpm-build make cmake

# qvdclient-qt dependencies
RUN zypper in -y  libqt5-qtbase-devel libQt5Charts5-devel libqt5-linguist-devel libX11-devel libXt-devel libxkbfile-devel 


# nxproxy dependencies
RUN zypper in -y zlib-devel libxml2-devel libpixman-1-0-devel libtool libtirpc-devel libpng16-devel libXtst-devel libjpeg62-devel libXrandr-devel libXpm-devel libXinerama-devel libXfont2-devel libXdmcp-devel libXcomposite-devel imake libexpat-devel automake autoconf python-devel quilt

RUN mkdir -p /root/rpmbuild/
RUN ln -s /usr/src/packages/{SOURCES,SPECS,BUILD,BUILDROOT,RPMS,SRPMS} /root/rpmbuild/

#PATH=/usr/lib64/qt5/bin:$PATH

