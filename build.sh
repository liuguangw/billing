#!/bin/sh

outputDir="$(pwd)/build"
buildType=MinSizeRel

mkdir -p "${outputDir}"
cd "${outputDir}" || exit
cmake -DCMAKE_BUILD_TYPE=${buildType} .
make
