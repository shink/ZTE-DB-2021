#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
cd $BASEDIR
cd ..

if [ ! -d DB-2021 ]
then
    echo "ERROR: $BASEDIR is not a valid directory of DB-2021."
    exit -1
fi
cmake --version 2>&1
tmp=$?
if [ ${tmp} -ne 0 ]
then
    echo "ERROR: You should install cmake(2.8 or later) first."
    echo "  Please goto https://cmake.org to download and install it."
    exit
fi

rm -rf bin
mkdir bin
rm -rf build
mkdir build

# sed -i "s/^#define TEST/\/\/ #define TEST/" DB-2021/main.cpp

cd build
cmake ../DB-2021
tmp=$?
echo "cmake return:" ${tmp}
if [ ${tmp} -ne 0 ]
then
 echo "cmake <>return:" ${tmp}
 exit -1
fi

make
tmp=$?
 echo "make return:" ${tmp}
if [ ${tmp} -ne 0 ]
then
echo "make <>return:" ${tmp}
 exit -1
fi

# cd ..
# sed -i "s/^\/\/ \#define TEST/\#define TEST/" DB-2021/main.cpp
