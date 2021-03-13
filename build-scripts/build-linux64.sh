#!/bin/bash

export TARGET_PLATFORM=linux
export ARCH=x86_64

echo -e "Building dependencies for ${TARGET_PLATFORM}"

export PROJECT_ROOT_DIR=$(pwd)/..
source ${PROJECT_ROOT_DIR}/build-scripts/prepare-build.sh 2>&1 >> /dev/null

export INSTALL_BASE="${PROJECT_OUT_DIR}/prebuilt/${TARGET_PLATFORM}"
source ${PROJECT_ROOT_DIR}/build-scripts/build.sh 2>&1 >> /dev/null

