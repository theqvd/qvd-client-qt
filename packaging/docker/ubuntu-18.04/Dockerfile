FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt upgrade -y
RUN apt install -y build-essential \
                   qtbase5-dev \ 
                   qtdeclarative5-dev \
                   qttools5-dev \
                   libqt5charts5-dev \
                   devscripts \
                   lintian \
                   dh-make \
                   libxt-dev \
                   libxkbfile-dev \
                   dwz \
                   cmake 

RUN curl http://security.ubuntu.com/ubuntu/pool/main/d/debhelper/debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb \
    -o debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb && \ 
    dpkg -i debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb
