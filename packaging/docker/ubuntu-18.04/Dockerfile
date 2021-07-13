FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt upgrade -y
RUN apt update &&  apt install -y build-essential \
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
                   wget \
                   curl

RUN curl http://security.ubuntu.com/ubuntu/pool/main/d/debhelper/debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb \
    -o debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb && \ 
    dpkg -i debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add - && \
    echo 'deb https://apt.kitware.com/ubuntu/ bionic main' >  /etc/apt/sources.list.d/cmake.list && \
    apt-get update && apt-get install -y cmake
