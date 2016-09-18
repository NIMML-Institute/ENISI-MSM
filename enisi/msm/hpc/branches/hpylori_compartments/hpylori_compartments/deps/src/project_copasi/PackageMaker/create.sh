#!/bin/bash
# Copyright (C) 2013 - 2015 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 


# PATH=$PATH:/bin:/usr/bin:/usr/local/bin

SCP=${COPASI_SCP:-scp}
AWK=${COPASI_AWK:-gawk}
SORT=${COPASI_SORT:-sort}
PACKAGE=${COPASI_PACKAGE:-Windows}

COMMON_ENVIRONMENT=${COPASI_COMMON_ENVIRONMENT:-"/Users/shoops/environment/10.6"}
BUILD=${COPASI_BUILD:-"${COMMON_ENVIRONMENT}/package"}
SOURCE=${COPASI_SOURCE:-"${COMMON_ENVIRONMENT}/COPASI"}
SETUP_DIR=${COPASI_SETUP_DIR:-"${COMMON_ENVIRONMENT}/setup"}

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
  MyAppVersion=${MyAppVersion}${buildname}
fi

echo mkdir -p "${SETUP_DIR}"
mkdir -p "${SETUP_DIR}"

case $1 in
  'COPASI')
    . ${SOURCE}/PackageMaker/COPASI.sh
    ;;

  'Java-Bindings')
    . ${SOURCE}/PackageMaker/Java-Bindings.sh
    ;;

  'C#-Bindings')
    . ${SOURCE}/PackageMaker/C#-Bindings.sh
    ;;

  'Python-Bindings')
    . ${SOURCE}/PackageMaker/Python-Bindings.sh
    ;;
esac

