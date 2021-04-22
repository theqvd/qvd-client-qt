FROM centos:latest

RUN yum update -y && yum install -y epel-release git glibc-headers rpm-sign expect
RUN yum install -y mock
RUN useradd -m -G mock nxbuilder
RUN mkdir -p /builddir && chmod 777 /builddir 

USER nxbuilder
RUN cd $HOME && git clone https://github.com/ArcticaProject/nx-libs.git

# Build nx-libs
RUN mkdir -p ~/result && \
    cd $HOME/nx-libs && git archive -o ../nx-libs-3.5.99.25.tar.gz --prefix=nx-libs-3.5.99.25/ 3.6.x && \
    cd ..

ADD qvd-pub.gpg /root
ADD qvd-secret.gpg /root
COPY nx-libs.spec /home/nxbuilder
COPY build.sh /
