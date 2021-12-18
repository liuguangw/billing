#!/bin/sh

rootDir=$(pwd)
buildType=Release

if [ ! -f "${rootDir}/build/mysql/libmysql/libmysqlclient.a" ]; then
	mkdir -p "${rootDir}"/build/mysql
	cd "${rootDir}"/build/mysql || exit
	cmake -DCMAKE_BUILD_TYPE=${buildType} "${rootDir}"/deps/mysql-connector-c-6.1.11-src
	cmake --build . --target mysqlclient
fi

mkdir -p "${rootDir}"/build/billing
cd "${rootDir}"/build/billing || exit
cmake -DCMAKE_BUILD_TYPE=${buildType} "${rootDir}"
make
