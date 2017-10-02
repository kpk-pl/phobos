#!/bin/bash

grep -rn --exclude-dir={boost,opencv,easyloggingpp,cpptoml,qt_ext,icons} TODO Phobos/
