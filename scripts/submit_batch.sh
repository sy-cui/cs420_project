#!/bin/bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
curr_dir=$(pwd)

required_dir="across_node_strong across_node_weak single_node_strong single_node_weak"

for dir in $required_dir; do
    if [ ! -d $script_dir/$dir ]; then
        mkdir $script_dir/$dir
    fi
done

error () {
    echo "Usage: bash submit_bash -[mode]"
    echo "mode: -oa (single node strong scaling)"
    echo "      -ow (single node weak scaling)"
    echo "      -ma (across node strong scaling)"
    echo "      -mw (across node weak scaling)"

    cd $curr_dir

    exit 1
}

if [ $# -ne 1 ]; then
    error
fi

if [ $1 = "-oa" ]; then
    cd $script_dir/..
    for file in scripts/single_node_strong/*; do
        sbatch $file
    done
elif [ $1 = "-ow" ]; then
    cd $script_dir/..
    for file in scripts/single_node_weak/*; do
        sbatch $file
    done
elif [ $1 = "-ma" ]; then
    cd $script_dir/..
    for file in scripts/across_node_strong/*; do
        sbatch $file
    done
elif [ $1 = "-mw" ]; then
    cd $script_dir/..
    for file in scripts/across_node_weak/*; do
        sbatch $file
    done
else
    error
fi

cd $curr_dir
