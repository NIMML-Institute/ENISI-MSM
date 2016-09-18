#!/bin/bash
# Copyright (C) 2013 - 2015 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

PATH=$PATH:/bin:/usr/bin:/usr/local/bin

SCP=${COPASI_SCP:-scp}
AWK=${COPASI_AWK:-gawk}
SORT=${COPASI_SORT:-sort}
PACKAGE=${COPASI_PACKAGE:-Windows}

COMMON_ENVIRONMENT=${COPASI_COMMON_ENVIRONMENT:-"/home/shoops/environment"}
SOURCE=${COPASI_SOURCE:-"${COMMON_ENVIRONMENT}/COPASI"}
BUILD_32=${COPASI_BUILD_32:-"${COMMON_ENVIRONMENT}/win32-icc-32/package-32"}
BUILD_64=${COPASI_BUILD_64:-"${COMMON_ENVIRONMENT}/win32-icc-64/package-64"}
SETUP_DIR=${COPASI_SETUP_DIR:-"${COMMON_ENVIRONMENT}/setup"}

case ${TARGET_VS_ARCH} in
  'x86')
    Arch="32-bit"
    PythonArch="win32"
    ;;

  'amd64')
    Arch="64-bit"
    PythonArch="win-amd64"
    ;;

  *)
    ;;
esac

echo mkdir -p "${SETUP_DIR}"
mkdir -p "${SETUP_DIR}"

major=`${AWK} -- '$2 ~ "COPASI_VERSION_MAJOR" {print $3}' "${SOURCE}/copasi/CopasiVersion.h"`
minor=`${AWK} -- '$2 ~ "COPASI_VERSION_MINOR" {print $3}' "${SOURCE}/copasi/CopasiVersion.h"`
build=`${AWK} -- '$2 ~ "COPASI_VERSION_BUILD" {print $3}' "${SOURCE}/copasi/CopasiVersion.h"`
modified=`${AWK} -- '$2 ~ "COPASI_VERSION_MODIFIED" {print $3}' "${SOURCE}/copasi/CopasiVersion.h"`
comment=`${AWK} -- '$2 ~ "COPASI_VERSION_COMMENT" {print $3}' "${SOURCE}/copasi/CopasiVersion.h"`
buildname=${build}

if [ $modified == true ]; then
  buildname=${buildname}+
fi

MyAppVersion=${major}.${minor}.${build}

if [ x"${comment}" = x\"Snapshot\" ]; then
  MyAppVersion=${major}.
  [ ${#minor} = 1 ] && MyAppVersion=${MyAppVersion}0
  MyAppVersion=${MyAppVersion}${minor}.
  [ ${#build} = 1 ] && MyAppVersion=${MyAppVersion}0
  MyAppVersion=${MyAppVersion}${build}
fi

case $1 in
  'COPASI')
    . ${SOURCE}/InnoSetup/COPASI.sh
    ;;

  'Java-Bindings')
    . ${SOURCE}/InnoSetup/Java-Bindings.sh
    ;;

  'C#-Bindings')
    . ${SOURCE}/InnoSetup/C#-Bindings.sh
    ;;

  'Python-Bindings')
    . ${SOURCE}/InnoSetup/Python-Bindings.sh
    ;;
esac

