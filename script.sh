#!/bin/bash

function config() {
    cd ./build && cmake ..
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
