#!/bin/sh

outputDir="$(pwd)/cmake-build-billing"
buildType=MinSizeRel

mkdir -p "${outputDir}"
cd "${outputDir}" || exit
cmake -DCMAKE_BUILD_TYPE=${buildType} ..
cmake --build . --target billing
