#!/bin/bash

source ${PROJECT_ROOT_DIR}/build-scripts/utils.sh

cd ${PROJECT_EXT_DIR}/${LIB_NAME} || exit 1

# replace cmake script
cp ${PROJECT_ROOT_DIR}/build-scripts/cifer.cmake CMakeLists.txt || exit 1

mkdir build && cd build || exit 1
cmake \
    -DDEPS_DIR=${INSTALL_BASE} \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_BASE}/${LIB_NAME} .. || exit 1

make -j$(get_cpu_count) install || exit 1
