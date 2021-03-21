#!/bin/bash

########### setup ######################

dependency_libs=(gmp amcl libsodium cifer)
echo -e "\nINFO: Starting new build at "$(date)"\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1

for library in "${dependency_libs[@]}"
do
  export LIB_NAME=${library}
  echo -e "Building ${LIB_NAME}...\n"
  SCRIPT_PATH="${PROJECT_ROOT_DIR}/build-scripts/build-${LIB_NAME}.sh"
  cd ${PROJECT_ROOT_DIR}
  ${SCRIPT_PATH}  2>&1 >> ${PROJECT_ROOT_DIR}/build.log
  if [ $? -eq 0 ]; then
    echo -e "${LIB_NAME} built successfully\n"
  else
    echo -e "Building ${LIB_NAME} failed\n"
    exit 1
  fi
done

echo -e "\nINFO: Completed build for ${ARCH} at "$(date)"\n" 1>>${PROJECT_ROOT_DIR}/build.log 2>&1

# clean
#${PROJECT_TMP_DIR}
