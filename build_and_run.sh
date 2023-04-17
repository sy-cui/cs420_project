#!/bin/bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
curr_dir="$(pwd)"

cd $script_dir
required_dir="build data result"

for dir in $required_dir;
do
    if [ ! -d $dir ]
    then
        mkdir $dir
    fi 
done

cd build/
cmake ..
cmake --build .

./Main --help

cd $curr_dir
