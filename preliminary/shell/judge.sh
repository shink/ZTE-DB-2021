#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")

cd $BASEDIR
cd ..

sh shell/build.sh
#sh shell/generate.sh
python3 judge/judge.py
