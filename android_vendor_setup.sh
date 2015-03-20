#!/bin/bash

# Invoke this from the root of an internal Android tree. It will create
# links to build swiftshader as vendor code in vendor/swiftshader

pushd $(dirname "$0") > /dev/null 2>&1
DIR="$(pwd)"
popd > /dev/null 2>&1

OUT="$(pwd)/vendor/swiftshader"
mkdir -p "${OUT}"

IFS=$'\n'
for i in $(find "${DIR}/src" -name Android.mk -print); do
  ln -s "$(dirname "${i}" )" "${OUT}"
done
unset IFS
