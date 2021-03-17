#! /bin/bash

export TARGET_PLATFORM="android"
export ARCH=arm64-v8a
export ANDROID_API=21

echo -e "Building dependencies for ${TARGET_PLATFORM}"

if [[ -z ${ANDROID_NDK_ROOT} ]]; then
    echo -e "(*) ANDROID_NDK_ROOT not defined\n"
    exit 1
fi

if [[ -z ${ARCH} ]]; then
    echo -e "(*) ARCH not defined\n"
    exit 1
fi

if [[ -z ${ANDROID_API} ]]; then
    echo -e "(*) API not defined\n"
    exit 1
fi

echo -e "\nBuilding for ${TARGET_PLATFORM} arch:${ARCH} platform on ANDROID_API level ${ANDROID_API}\n"

export PROJECT_ROOT_DIR=$(pwd)/..
source ${PROJECT_ROOT_DIR}/build-scripts/prepare-build.sh 2>&1 >> /dev/null

source ${PROJECT_ROOT_DIR}/build-scripts/android-setup.sh 2>&1 >> /dev/null

export TOOLCHAIN=$(get_toolchain)
export TOOLCHAIN_ARCH=$(get_toolchain_arch)

export INSTALL_BASE="${PROJECT_OUT_DIR}/prebuilt/${TARGET_PLATFORM}/${ARCH}"

# PREPARE PATHS
export PATH=$PATH:${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${TOOLCHAIN}/bin

BUILD_HOST=$(get_build_host)

export AR=${BUILD_HOST}-ar
export CC=$(get_clang_target_host)-clang
export CXX=$(get_clang_target_host)-clang++
export LD=${BUILD_HOST}-ld
export RANLIB=${BUILD_HOST}-ranlib
export STRIP=${BUILD_HOST}-strip

# PREPARING FLAGS
export BUILD_HOST=`get_build_host`
export CFLAGS=`get_cflags`
export CXXFLAGS=`get_cxxflags`
export LDFLAGS=`get_ldflags`

source ${PROJECT_ROOT_DIR}/build-scripts/build.sh 2>&1 >> /dev/null
