#!/bin/sh

outputDir="$(pwd)/cmake-build-billing"
buildType=MinSizeRel

# 可选32位 ./build.sh m32
# 可选shared ./build.sh shared
buildM32=
buildShared=
if [ $# -ge 1 ]; then
  for arg in $*; do
    if [ "$arg" = "m32" ]; then
      buildM32=-DBILLING_BUILD_M32=ON
    elif [ "$arg" = "shared" ]; then
      buildShared=-DBUILD_SHARED_LIBS=ON
    fi
  done
fi

mkdir -p "${outputDir}"
cd "${outputDir}" || exit
cmake -DCMAKE_BUILD_TYPE=${buildType} ${buildM32} ${buildShared} ..
cmake --build . --target billing
