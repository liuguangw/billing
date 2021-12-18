#/bin/sh

rootDir=$(pwd)
mkdir -p ${rootDir}/build/mysql
cd ${rootDir}/build/mysql
cmake -DCMAKE_BUILD_TYPE=Release ${rootDir}/deps/mysql-connector-c-6.1.11-src
make

