# Build scripts are adopted from this project
# https://github.com/tanersener/mobile-ffmpeg
#!/bin/bash

get_build_host() {
    case ${ARCH} in
        arm-v7a | arm-v7a-neon)
            echo "arm-linux-androideabi"
        ;;
        arm64-v8a)
            echo "aarch64-linux-android"
        ;;
        x86)
            echo "i686-linux-android"
        ;;
        x86-64)
            echo "x86_64-linux-android"
        ;;
    esac
}


get_clang_target_host() {
    case ${ARCH} in
        arm-v7a | arm-v7a-neon)
            echo "armv7a-linux-androideabi${ANDROID_API}"
        ;;
        arm64-v8a)
            echo "aarch64-linux-android${ANDROID_API}"
        ;;
        x86)
            echo "i686-linux-android${ANDROID_API}"
        ;;
        x86-64)
            echo "x86_64-linux-android${ANDROID_API}"
        ;;
    esac
}

get_toolchain() {
    HOST_OS=$(uname -s)
    case ${HOST_OS} in
        Darwin) HOST_OS=darwin;;
        Linux) HOST_OS=linux;;
        FreeBsd) HOST_OS=freebsd;;
        CYGWIN*|*_NT-*) HOST_OS=cygwin;;
    esac

    HOST_ARCH=$(uname -m)
    case ${HOST_ARCH} in
        i?86) HOST_ARCH=x86;;
        x86_64|amd64) HOST_ARCH=x86_64;;
    esac

    echo "${HOST_OS}-${HOST_ARCH}"
}

get_cmake_target_processor() {
    case ${ARCH} in
        arm-v7a | arm-v7a-neon)
            echo "arm"
        ;;
        arm64-v8a)
            echo "aarch64"
        ;;
        x86)
            echo "x86"
        ;;
        x86-64)
            echo "x86_64"
        ;;
    esac
}

get_target_build() {
    case ${ARCH} in
        arm-v7a)
            echo "arm"
        ;;
        arm-v7a-neon)
            echo "arm/neon"
        ;;
        arm64-v8a)
            echo "arm64"
        ;;
        x86)
            echo "x86"
        ;;
        x86-64)
            echo "x86_64"
        ;;
    esac
}

get_toolchain_arch() {
    case ${ARCH} in
        arm-v7a | arm-v7a-neon)
            echo "arm"
        ;;
        arm64-v8a)
            echo "arm64"
        ;;
        x86)
            echo "x86"
        ;;
        x86-64)
            echo "x86_64"
        ;;
    esac
}

get_common_includes() {
    echo ""
}

get_common_cflags() {
    echo "-fno-integrated-as -fstrict-aliasing -fPIC -DANDROID -D__ANDROID__ -D__ANDROID_API__=${ANDROID_API}"
}

get_arch_specific_cflags() {
    case ${ARCH} in
        arm-v7a)
            echo "-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp"
        ;;
        arm-v7a-neon)
            echo "-march=armv7-a -mfpu=neon -mfloat-abi=softfp"
        ;;
        arm64-v8a)
            echo "-march=armv8-a"
        ;;
        x86)
            echo "-march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32"
        ;;
        x86-64)
            echo "-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel"
        ;;
    esac
}

get_size_optimization_cflags() {
    if [[ ${LINK_TIME_OPTIMIZATION} ]]; then
        local LINK_TIME_OPTIMIZATION_FLAGS="-flto"
    fi

    local ARCH_OPTIMIZATION="-Os -ffunction-sections -fdata-sections"

    echo "${LINK_TIME_OPTIMIZATION_FLAGS} ${ARCH_OPTIMIZATION}"
}

get_app_specific_cflags() {
    local APP_FLAGS="-std=c99 -Wno-unused-function"

    echo "${APP_FLAGS}"
}

get_cflags() {
    local ARCH_FLAGS=$(get_arch_specific_cflags)
    local APP_FLAGS=$(get_app_specific_cflags $1)
    local COMMON_FLAGS=$(get_common_cflags)
    if [[ -z ${LIBS_DEBUG} ]]; then
        local OPTIMIZATION_FLAGS=$(get_size_optimization_cflags $1)
    else
        local OPTIMIZATION_FLAGS="${LIBS_DEBUG}"
    fi
    local COMMON_INCLUDES=$(get_common_includes)

    echo "${ARCH_FLAGS} ${APP_FLAGS} ${COMMON_FLAGS} ${OPTIMIZATION_FLAGS} ${COMMON_INCLUDES}"
}

get_cxxflags() {
    if [[ -z ${NO_LINK_TIME_OPTIMIZATION} ]]; then
        local LINK_TIME_OPTIMIZATION_FLAGS="-flto"
    else
        local LINK_TIME_OPTIMIZATION_FLAGS=""
    fi

    if [[ -z ${LIBS_DEBUG} ]]; then
        local OPTIMIZATION_FLAGS="-Os -ffunction-sections -fdata-sections"
    else
        local OPTIMIZATION_FLAGS="${LIBS_DEBUG}"
    fi

    echo "-std=c++11 -fno-exceptions -fno-rtti ${OPTIMIZATION_FLAGS}"
}

get_common_linked_libraries() {
    local COMMON_LIBRARY_PATHS="-L${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${TOOLCHAIN}/${BUILD_HOST}/lib -L${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${TOOLCHAIN}/sysroot/usr/lib/${BUILD_HOST}/${ANDROID_API} -L${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${TOOLCHAIN}/lib"

    echo "-lc -lm -ldl -llog ${COMMON_LIBRARY_PATHS}"
}

get_size_optimization_ldflags() {
    if [[ -z ${NO_LINK_TIME_OPTIMIZATION} ]]; then
        local LINK_TIME_OPTIMIZATION_FLAGS="-flto"
    else
        local LINK_TIME_OPTIMIZATION_FLAGS=""
    fi

    case ${ARCH} in
        arm64-v8a)
            echo "-Wl,--gc-sections -Os -ffunction-sections -fdata-sections"
        ;;
        *)
            echo "-Wl,--gc-sections,--icf=safe -Os -ffunction-sections -fdata-sections"
        ;;
    esac
}

get_arch_specific_ldflags() {
    case ${ARCH} in
        arm-v7a)
            echo "-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8"
        ;;
        arm-v7a-neon)
            echo "-march=armv7-a -mfpu=neon -mfloat-abi=softfp -Wl,--fix-cortex-a8"
        ;;
        arm64-v8a)
            echo "-march=armv8-a"
        ;;
        x86)
            echo "-march=i686"
        ;;
        x86-64)
            echo "-march=x86-64"
        ;;
    esac
}

get_ldflags() {
    local ARCH_FLAGS=$(get_arch_specific_ldflags)
    if [[ -z ${LIBS_DEBUG} ]]; then
        local OPTIMIZATION_FLAGS="$(get_size_optimization_ldflags)"
    else
        local OPTIMIZATION_FLAGS="${LIBS_DEBUG}"
    fi
    local COMMON_LINKED_LIBS=$(get_common_linked_libraries)

    echo "${ARCH_FLAGS} ${OPTIMIZATION_FLAGS} ${COMMON_LINKED_LIBS} -Wl,--hash-style=both -Wl,--exclude-libs,libgcc.a -Wl,--exclude-libs,libunwind.a"
}


android_ndk_abi() {
    # to be used with CMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake
    case ${ARCH} in
        arm-v7a | arm-v7a-neon)
            echo "armeabi-v7a"
        ;;
        arm64-v8a)
            echo "arm64-v8a"
        ;;
        x86)
            echo "x86"
        ;;
        x86-64)
            echo "x86_64"
        ;;
    esac
}

android_build_dir() {
  echo ${PROJECT_ROOT_DIR}/android/build/${LIB_NAME}/$(get_target_build)
}

android_ndk_cmake() {
    local cmake=$(find ${ANDROID_HOME}/cmake -path \*/bin/cmake -type f -print -quit)
    if [[ -z ${cmake} ]]; then
        cmake=$(which cmake)
    fi
    if [[ -z ${cmake} ]]; then
        cmake="missing_cmake"
    fi

    echo ${cmake} \
  -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake \
  -H${PROJECT_ROOT_DIR}/src/${LIB_NAME} \
  -B$(android_build_dir) \
  -DANDROID_ABI=$(android_ndk_abi) \
  -DANDROID_PLATFORM=android-${ANDROID_API} \
  -DCMAKE_INSTALL_PREFIX=${PROJECT_ROOT_DIR}/prebuilt/android-$(get_target_build)/${LIB_NAME}
}

set_toolchain_clang_paths() {
    export PATH=$PATH:${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${TOOLCHAIN}/bin

    BUILD_HOST=$(get_build_host)

    export AR=${BUILD_HOST}-ar
    export CC=$(get_clang_target_host)-clang
    export CXX=$(get_clang_target_host)-clang++
    export LD=${BUILD_HOST}-ld
    export RANLIB=${BUILD_HOST}-ranlib
    export STRIP=${BUILD_HOST}-strip
}