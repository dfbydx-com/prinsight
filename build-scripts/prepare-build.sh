#!/bin/bash

# install directory of external dependencies
export PROJECT_OUT_DIR=${PROJECT_ROOT_DIR}/out
# temp files such as downloaded source tarball
export PROJECT_TMP_DIR=${PROJECT_OUT_DIR}/.tmp
# source code for external dependencies
export PROJECT_EXT_DIR=${PROJECT_OUT_DIR}/external

# clean build
rm -rf "${PROJECT_OUT_DIR}/prebuilt"

if [[ ! -d ${PROJECT_OUT_DIR} ]]; then
    mkdir -p "${PROJECT_OUT_DIR}"
fi

if [[ ! -d ${PROJECT_TMP_DIR} ]]; then
    mkdir -p "${PROJECT_TMP_DIR}"
fi

if [[ ! -d ${PROJECT_EXT_DIR} ]]; then
    mkdir -p "${PROJECT_EXT_DIR}"
fi

# common functions
source ${PROJECT_ROOT_DIR}/build-scripts/utils.sh

# download gmp
gmp_version="6.2.1"
gmp_tarball_name="gmp-${gmp_version}.tar.xz"
gmp_mirror="https://gmplib.org/download/gmp/${gmp_tarball_name}"
download_extract_tar ${gmp_mirror} ${gmp_tarball_name} gmp || exit 1

# download libsodium
libsodium_git_repo="https://github.com/jedisct1/libsodium.git"
libsodium_release_tag="1.0.18-RELEASE"
clone_git_repository_with_tag ${libsodium_git_repo} ${libsodium_release_tag} libsodium || exit 1

# download amcl
rm -rf ${PROJECT_EXT_DIR}/amcl
amcl_git_repo="https://github.com/miracl/amcl.git"
amcl_commit_id="e39201e3d34f4406530c103bb01f50fd84253b48"
clone_git_repository_with_commit_id ${amcl_git_repo} ${amcl_commit_id} amcl || exit 1

# download cifer
cifer_git_repo="https://github.com/dev0x1/CiFEr.git"
cifer_commit_id="d8aefff7166be9454f1bd2e6c86656ad9710784a"
clone_git_repository_with_commit_id ${cifer_git_repo} ${cifer_commit_id} cifer || exit 1
