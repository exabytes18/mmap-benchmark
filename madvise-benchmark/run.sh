#!/bin/bash

run() {
    local SIZE=$1
    local PASSES=$2
    ./sequential-access-benchmark $SIZE $PASSES
    for THREADS in 1 2 4 8 16 32 64; do
        ./random-access-benchmark $SIZE $PASSES $THREADS
    done
}

> output.csv
run 1gb 2 >> output.csv
run 32gb 2 >> output.csv
