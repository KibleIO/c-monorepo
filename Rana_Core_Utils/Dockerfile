from debian:11

RUN export DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt -y install g++ make libcurl4-openssl-dev libx11-dev libxtst-dev uuid-dev libsodium-dev libc-ares-dev libswscale-dev libasound2-dev
RUN apt -y install libopus-dev libx264-dev
RUN apt -y install libopusfile-dev
RUN apt -y install qt5-qmake qtbase5-dev qtmultimedia5-dev
RUN apt -y install libavformat-dev libavcodec-dev
RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/*
COPY ./librana.so /usr/local/lib
COPY ./Networking /usr/local/include/rana/Networking
COPY ./Utilities /usr/local/include/rana/Utilities
WORKDIR /root
