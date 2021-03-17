#!/bin/bash

source ${PROJECT_ROOT_DIR}/build-scripts/utils.sh

cd ${PROJECT_EXT_DIR}/${LIB_NAME} 1>>${PROJECT_ROOT_DIR}/build.log 2>&1

make distclean 1>>${PROJECT_ROOT_DIR}/build.log 2>&1

config_args=""
config_args+=" --prefix=${INSTALL_BASE}/${LIB_NAME} --with-pic --disable-static --disable-maintainer-mode "

if [[ "${TARGET_PLATFORM}" == "android" ]]; then
config_args+=" --with-sysroot=${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${TOOLCHAIN}/sysroot --host=${BUILD_HOST} "

export LIBGMP_LDFLAGS='-avoid-version'
export LIBGMPXX_LDFLAGS='-avoid-version'

fi

./configure ${config_args} || exit 1

make -j$(get_cpu_count) install || exit 1