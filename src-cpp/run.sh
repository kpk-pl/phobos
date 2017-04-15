#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/config.sh

cd $PROJECT_DIR/Build/$RELEASE
./Phobos
cd - &>/dev/null
