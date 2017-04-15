#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/config.sh

mkdir -p $PROJECT_DIR/Build
cd $PROJECT_DIR/Build

$CMAKE_BIN/qmake CONFIG+=debug $PROJECT_DIR/$PROJECT_NAME/$PROJECT_NAME.pro
make -j2 "$@"

cd - &>/dev/null
