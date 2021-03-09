#!/bin/bash

source ${PROJECT_ROOT_DIR}/build-scripts/utils.sh

# miracl in-source build system doesn't work for rebuiding
# create a temp build dir
TEMP=${PROJECT_TMP_DIR}/${LIB_NAME}
rm -rf ${TEMP}
mkdir ${TEMP} && cd ${TEMP} || exit 1
cp -r ${PROJECT_EXT_DIR}/${LIB_NAME}/version3/c . || exit 1

# build
cd c || exit 1
sed -ie 's/ -O3/ -O3 -fPIC/g' config64.py || exit 1
echo -e "18\n0" | python config64.py || exit 1

# install
mkdir -p ${INSTALL_BASE}/${LIB_NAME}/include ${INSTALL_BASE}/${LIB_NAME}/lib || exit 1
cp *.h ${INSTALL_BASE}/${LIB_NAME}/include || exit 1
cp amcl.a ${INSTALL_BASE}/${LIB_NAME}/lib/libamcl.a || exit 1

rm -rf ${TEMP}
