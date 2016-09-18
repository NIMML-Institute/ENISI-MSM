#!/bin/bash
# Copyright (C) 2015 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

PYTHON_VERSION="$(python --version 2>&1)"
PYTHON_VERSION=${PYTHON_VERSION#Python }
PYTHON_VERSION=${PYTHON_VERSION:0:3}

PACKAGE_NAME="COPASI-${MyAppVersion}-Python-${PYTHON_VERSION}-Bindings-Darwin"

echo mkdir -p "${SETUP_DIR}/${PACKAGE_NAME}"
mkdir -p "${SETUP_DIR}/${PACKAGE_NAME}"

pushd "${SETUP_DIR}"
pushd "${PACKAGE_NAME}"
rm -rf *

# Create directory structure
echo mkdir examples
mkdir examples
chmod -R 755 *

# Copy license
echo cp "${SOURCE}"/copasi/ArtisticLicense.txt LICENSE
cp "${SOURCE}"/copasi/ArtisticLicense.txt LICENSE
chmod 644 LICENSE

# Copy COPASI.py
echo cp "${BUILD}"/copasi/bindings/python/COPASI.py  .
cp "${BUILD}"/copasi/bindings/python/COPASI.py .
chmod 644 COPASI.py

# Copy _COPASI.so
echo cp "${BUILD}"/copasi/bindings/python/_COPASI.so  .
cp "${BUILD}"/copasi/bindings/python/_COPASI.so .
chmod 644 _COPASI.so

# Copy examples
echo cp "${SOURCE}"/copasi/bindings/python/examples/*.py examples
cp "${SOURCE}"/copasi/bindings/python/examples/*.py examples
chmod 644 examples/*

popd

echo tar -cvzf ${PACKAGE_NAME}.tar.gz ${PACKAGE_NAME}
tar -czvf ${PACKAGE_NAME}.tar.gz ${PACKAGE_NAME}

popd
