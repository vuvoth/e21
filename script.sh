#!/bin/bash

function config() {
    LD_LIBRARY_PATH=/usr/local/opt/openssl/lib:"${LD_LIBRARY_PATH}"
    CPATH=/usr/local/opt/openssl/include:"${CPATH}"
    PKG_CONFIG_PATH=/usr/local/opt/openssl/lib/pkgconfig:"${PKG_CONFIG_PATH}"
    export LD_LIBRARY_PATH CPATH PKG_CONFIG_PATH
    cd build && CPPFLAGS=-I/usr/local/opt/openssl/include LDFLAGS=-L/usr/local/opt/openssl/lib PKG_CONFIG_PATH=/usr/local/opt/openssl/lib/pkgconfig cmake -DWITH_PROCPS=OFF -DWITH_SUPERCOP=OFF ..
}


function build() {
    cd ./build 
    make -j8
}


if [ $1 == "config" ]; then 
    config
else 
    build
fi
