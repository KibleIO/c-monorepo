from debian:11

RUN export DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt -y install g++ make libcurl4-openssl-dev libx11-dev libxtst-dev uuid-dev libsodium-dev libc-ares-dev libswscale-dev libasound2-dev
RUN apt -y install libopus-dev libx264-dev
RUN apt -y install libopusfile-dev
RUN apt -y install libavformat-dev libavcodec-dev
RUN apt -y install libjson-c-dev libpulse-dev
RUN apt -y install cmake git
RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/*
RUN mkdir -p /root/.local
WORKDIR /root
RUN git clone --recurse-submodules -b v1.43.0 https://github.com/grpc/grpc
WORKDIR /root/grpc
RUN mkdir -p cmake/build
WORKDIR /root/grpc/cmake/build
RUN cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/root/.local ../..
RUN make
RUN make install
RUN mkdir -p /root/code
RUN apt update
RUN apt -y install rename
WORKDIR /root/code
RUN rm -rf /root/grpc
COPY ./librana.so /usr/local/lib
COPY ./Networking/*.h /usr/local/include/rana/Networking/
COPY ./Utilities/*.h /usr/local/include/rana/Utilities/
COPY ./Hermes/*.h /usr/local/include/rana/Hermes/
COPY ./idl/cpp/gen/*.h /usr/local/include/rana/idl/cpp/gen/