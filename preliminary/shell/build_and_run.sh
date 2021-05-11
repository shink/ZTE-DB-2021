#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
cd $BASEDIR
cd ..

if [ ! -f log.txt ]
then
  touch log.txt
fi

if [ ! -f output.txt ]
then
  touch output.txt
fi

sh shell/build.sh
bin/DB-2021
