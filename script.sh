#!/bin/bash

function config() {
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
