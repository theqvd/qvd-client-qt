FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt upgrade -y
RUN apt install -y build-essential
RUN apt install -y qtbase5-dev qtdeclarative5-dev qttools5-dev
RUN apt install -y devscripts lintian
RUN apt install -y dh-make libxt-dev libxkbfile-dev dwz
RUN curl http://security.ubuntu.com/ubuntu/pool/main/d/debhelper/debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb -o debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb && dpkg -i debhelper_12.1.1ubuntu1~ubuntu18.04.1_all.deb
