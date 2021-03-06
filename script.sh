#!/bin/bash

function config() {
    unlink ./utils/ethsnarks
    ln -fs $(pwd)/libs/ethsnarks/ethsnarks utils/ethsnarks
    cd ./build && cmake ..
}

function build() {
    cd ./build 
    make -j8
}

function test() {
    cd ./build/src/test && ./account_test
}

if [ $1 == "config" ]; then 
    config
elif [ $1 == "build" ]; then
    build
else 
    test
fi
