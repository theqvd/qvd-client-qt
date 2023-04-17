FROM rockylinux:9.1

RUN yum update -y && yum install -y epel-release git glibc-headers rpm-sign expect
RUN sed -i 's/enabled=0/enabled=1/' /etc/yum.repos.d/rocky-devel.repo
RUN yum install -y 'dnf-command(config-manager)'
RUN yum  config-manager --add-repo http://yum.oracle.com/repo/OracleLinux/OL9/distro/builder/x86_64/
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
