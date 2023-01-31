FROM registry.qindel.com:5000/sles12_updated:sp2

#### Configure packages repo ####
RUN zypper ar -G http://cpan-mirror.qindel.com/sles12-sp2-media/dvd1/ DVD1suse
RUN zypper ar -G http://cpan-mirror.qindel.com/sles12-sp2-media/sdk1/ SDK1suse
RUN zypper ar -G http://hulkbuster.qindel.com/sles/12SP2/QVD-4.1/ QVD
RUN zypper --non-interactive ref && zypper update -y

# Base build tools
RUN zypper in -y rpm-build make git curl tree vim

# Install CLANG 7
RUN zypper ar -G http://hulkbuster/sles/12SP2/CLANG-7/ CLANG7
RUN zypper in -y clang7

# Install cmake 3.16.0
RUN curl -k -L https://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0-linux-x86_64.tar.gz > /tmp/cmake-3.16.0-linux-x86_64.tar.gz && \
    tar -zxf /tmp/cmake-3.16.0-linux-x86_64.tar.gz -C /opt/ && \
    ln -s /opt/cmake-3.16.0-Linux-x86_64 /opt/cmake

# Create base directories to build rpms 
RUN mkdir -p /root/rpmbuild/
RUN ln -s /usr/src/packages/{SOURCES,SPECS,BUILD,BUILDROOT,RPMS,SRPMS} /root/rpmbuild/

# qvdclient-qt dependencies
RUN zypper in -y libqt5-qtbase-devel libqt5-linguist-devel libX11-devel libXt-devel libxkbfile-devel 
ENV PATH=/usr/lib64/qt5/bin:/opt/cmake/bin:$PATH
ENV GIT_SSL_NO_VERIFY=true
RUN git clone https://github.com/qt/qtcharts.git -b 5.6 && \
    mv /qtcharts /qtcharts-opensource-src-5.6.1 && \
    tar -Jcvf /usr/src/packages/SOURCES/qtcharts-opensource-src-5.6.1.tar.xz /qtcharts-opensource-src-5.6.1 && \
    cd /qtcharts-opensource-src-5.6.1 && qmake && make && make install

# Set CLANG 7 as default compiler
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 0
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/clang 0

# Remove old cmake
RUN mv /usr/bin/cmake /usr/bin/cmake.old
