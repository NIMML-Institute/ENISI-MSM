#!/bin/bash
# Copyright (C) 2015 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

PACKAGE_NAME="COPASI-${MyAppVersion}-Java-Bindings-Windows-${Arch}"
BUILD_ROOT="$(pwd)"

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

# Copy copasi.jar
echo cp "${BUILD_ROOT}"/copasi/bindings/java/copasi.jar  .
cp "${BUILD_ROOT}"/copasi/bindings/java/copasi.jar .
chmod 644 copasi.jar

# Copy copasi_gui.jar
echo cp "${BUILD_ROOT}"/copasi/bindings/java/copasi-gui.jar  .
cp "${BUILD_ROOT}"/copasi/bindings/java/copasi-gui.jar .
chmod 644 copasi_gui.jar

# Copy CopasiJava.dll
echo "${BUILD_ROOT}"/cp copasi/bindings/java/CopasiJava.dll  .
cp "${BUILD_ROOT}"/copasi/bindings/java/CopasiJava.dll .
chmod 644 CopasiJava.dll

# Copy examples
echo cp "${SOURCE}"/copasi/bindings/java/examples/*.java examples
cp "${SOURCE}"/copasi/bindings/java/examples/*.java examples
chmod 644 examples/*

popd

echo tar -cvzf ${PACKAGE_NAME}.tar.gz ${PACKAGE_NAME}
tar -czvf ${PACKAGE_NAME}.tar.gz ${PACKAGE_NAME}

popd
