#!/bin/sh

set -e

if [ ! -d build ]; then
  mkdir build
fi

cd build

cmake ..
make
./Experiments/Experiments portal

