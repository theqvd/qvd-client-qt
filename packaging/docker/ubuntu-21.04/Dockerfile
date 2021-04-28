FROM ubuntu:21.04

ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y build-essential gpgv2
RUN apt-get install -y qtbase5-dev qtdeclarative5-dev qttools5-dev 
RUN apt-get install -y devscripts lintian
RUN apt-get install -y dh-make libxt-dev libxkbfile-dev libsystemd-dev
