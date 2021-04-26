FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive 

RUN apt update
RUN apt upgrade -y
RUN apt install -y build-essential
RUN apt install -y qtbase5-dev qtdeclarative5-dev qttools5-dev 
RUN apt install -y devscripts lintian
RUN apt install -y dh-make libxt-dev libxkbfile-dev libsystemd-dev
RUN apt install -y menu
