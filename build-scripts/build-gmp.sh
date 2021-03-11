#!/bin/bash

source ${PROJECT_ROOT_DIR}/build-scripts/utils.sh

cd ${PROJECT_EXT_DIR}/${LIB_NAME} || exit 1

make distclean 2>/dev/null 1>/dev/null

./configure \
    --prefix=${INSTALL_BASE}/${LIB_NAME} \
    --with-pic \
    --enable-cxx \
    --disable-static \
    --enable-shared \
    --disable-maintainer-mode \
    CFLAGS=-g CXXFLAGS=-g || exit 1

make -j$(get_cpu_count) install || exit 1