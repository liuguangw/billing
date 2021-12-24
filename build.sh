#!/bin/sh

outputDir="$(pwd)/cmake-build-billing"
buildType=MinSizeRel

# 可选32位 ./build.sh m32
buildOpt=
if [ $# -ge 1 ]; then
  if [ "$1" = "m32" ]; then
    buildOpt=-DBILLING_BUILD_M32=ON
  fi
fi

mkdir -p "${outputDir}"
cd "${outputDir}" || exit
cmake -DCMAKE_BUILD_TYPE=${buildType} ${buildOpt} ..
cmake --build . --target billing
