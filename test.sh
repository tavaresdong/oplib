#!/bin/bash

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=build

cd $BUILD_DIR \
    && make test
