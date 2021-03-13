#!/bin/bash

get_cpu_count() {
  if [ "$(uname)" == "Darwin" ]; then
    echo $(sysctl -n hw.physicalcpu)
  else
    echo $(nproc)
  fi
}

#
# 1. <repo url>
# 2. <commit id>
# 3. <local folder path>
#
clone_git_repository_with_commit_id() {
  local RC
  if [ ! -d "${PROJECT_EXT_DIR}/${3}" ]; then
     (mkdir -p "${PROJECT_EXT_DIR}/${3}" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)
  else
    echo -e "\nDEBUG: project already exist, cleaning it\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    cd "${PROJECT_EXT_DIR}/${3}"
    git clean -fdx
    echo ${RC}
    return
  fi

  RC=$?

  if [ ${RC} -ne 0 ]; then
    echo -e "\nDEBUG: Failed to create local directory $2\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    rm -rf $3 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    echo ${RC}
    return
  fi

  (git clone $1 "${PROJECT_EXT_DIR}/${3}" --depth 1 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)

  RC=$?

  if [ ${RC} -ne 0 ]; then
    echo -e "\nDEBUG: Failed to clone $1\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    rm -rf "${PROJECT_EXT_DIR}/${3}" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    echo ${RC}
    return
  fi

  cd "${PROJECT_EXT_DIR}/${3}" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1

  RC=$?

  if [ ${RC} -ne 0 ]; then
    echo -e "\nDEBUG: Failed to cd into ${PROJECT_EXT_DIR}/${3}\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    rm -rf "${PROJECT_EXT_DIR}/${3}" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    echo ${RC}
    return
  fi

  (git fetch --depth 1 origin $2 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)

  RC=$?

  if [ ${RC} -ne 0 ]; then
    echo -e "\nDEBUG: Failed to fetch commit id $2 from $1\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    rm -rf "${PROJECT_EXT_DIR}/${3}" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    echo ${RC}
    return
  fi

  (git checkout $2 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)

  RC=$?

  if [ ${RC} -ne 0 ]; then
    echo -e "\nDEBUG: Failed to checkout commit id $2 from $1\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    echo ${RC}
    return
  fi

  echo ${RC}
}

#
# 1. <repo url>
# 2. <tag name>
# 3. <local folder path>
#
clone_git_repository_with_tag() {
  local RC
  if [ ! -d "${PROJECT_EXT_DIR}/${3}" ]; then
     (mkdir -p "${PROJECT_EXT_DIR}/${3}" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)
  else
    echo -e "\nDEBUG: project already exist, cleaning it\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    cd "${PROJECT_EXT_DIR}/${3}"
    git clean -fdx
    echo ${RC}
    return
  fi

  RC=$?

  if [ ${RC} -ne 0 ]; then
    echo -e "\nDEBUG: Failed to create local directory $3\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    rm -rf $3 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    echo ${RC}
    return
  fi

  (git clone --depth 1 --branch $2 $1 "${PROJECT_EXT_DIR}/${3}" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)

  RC=$?

  if [ ${RC} -ne 0 ]; then
    echo -e "\nDEBUG: Failed to clone $1 -> $2\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    rm -rf $3 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    echo ${RC}
    return
  fi

  echo ${RC}
}

#
# 1. <url>
# 2. <local tarball name>
# 3. <local extracted dir name>
#
download_extract_tar() {
  # don't download if file is already available
  if [ ! -f "${PROJECT_TMP_DIR}/$2" ]; then
    (curl --fail --location $1 -o ${PROJECT_TMP_DIR}/$2 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)
  fi

  mkdir -p "${PROJECT_EXT_DIR}/${3}"

  (tar -xf ${PROJECT_TMP_DIR}/${gmp_tarball_name} -C ${PROJECT_EXT_DIR}/${3} --strip-components=1 1>>${PROJECT_ROOT_DIR}/build.log 2>&1)

  local RC=$?

  if [ ${RC} -eq 0 ]; then
    echo -e "\nDEBUG: Downloaded/extracted $1 to ${PROJECT_EXT_DIR}/$3\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
  else
    rm -f ${PROJECT_TMP_DIR}/$2 1>>${PROJECT_ROOT_DIR}/build.log 2>&1

    echo -e -n "\nINFO: Failed to download $1 to ${PROJECT_TMP_DIR}/$2, rc=${RC}. " 1>>${PROJECT_ROOT_DIR}/build.log 2>&1

    echo -e "DEBUG: Build will now exit.\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1
    exit 1

  fi

  echo ${RC}
}

