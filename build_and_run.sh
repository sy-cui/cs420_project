#!/bin/bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
curr_dir="$(pwd)"

cd $script_dir
if [ ! -d "build" ]
then
    mkdir build
fi

if [ ! -d "data" ]
then
    mkdir data
fi

cd build/
cmake ..
cmake --build .

cd $curr_dir
