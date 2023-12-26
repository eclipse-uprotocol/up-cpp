# Copyright 2019 Adrian de Vera Alonzo
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

# This is a multi-stage build. This file creates intermediate images and
# discards some intermediate artifacts to produce an overall smaller image.

ARG DISTRO_VERSION=22.04
FROM ubuntu:${DISTRO_VERSION} AS devtools

# Install typical development tools needed for grpc and protobuf
# Tell apt-get we're never going to be able to give manual
# feedback:
RUN export DEBIAN_FRONTEND=noninteractive
RUN export MY_INSTALL_DIR=$HOME/.local
RUN export PATH="$PATH:$MY_INSTALL_DIR/bin"
RUN apt-get update && apt-get upgrade
RUN apt -y install apt-transport-https ca-certificates gpg wget libbsd-dev make ninja-build build-essential
RUN apt -y install clang-format gcc uuid uuid-dev openssl libssl-dev g++-11 gcc-11 git libstdc++-11-dev 
RUN apt -y install libgtest-dev rapidjson-dev cgreen1  libspdlog-dev libcgreen1-dev wget curl clang-12 clang-format-12 clang-tidy-12 gdb libclang-cpp12-dev libc++-12-dev sudo

# Update the package listing, so we know what package exist:
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | sudo tee /etc/apt/sources.list.d/kitware.list >/dev/null
RUN apt-get -y update && apt-get -y upgrade && apt-get -y install kitware-archive-keyring cmake
RUN update-ca-certificates
RUN mkdir -p /opt/protobuf
WORKDIR /opt/protobuf
RUN git clone --progress -b v3.21.12  https://github.com/protocolbuffers/protobuf && cd protobuf && git submodule update --init --recursive
WORKDIR /opt/protobuf/protobuf
RUN cmake . -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_CXX_STANDARD=14 \
 -DCMAKE_BUILD_TYPE=Debug -Dprotobuf_BUILD_SHARED_LIBS=ON -Dprotobuf_BUILD_TESTS=OFF && cmake --build . && make -j4 install

# delete cached files we no longer need
RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/*

#FROM devtools AS with_grpc

# Clone grpc's git and locally install protobuf and grpc

WORKDIR /
