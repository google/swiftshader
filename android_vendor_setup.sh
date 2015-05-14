#!/bin/bash

# Invoke this from the root of an internal Android tree. It will create
# links to build swiftshader as vendor code in vendor/swiftshader

set -o errexit

pushd $(dirname "$0") > /dev/null 2>&1
DIR="$(pwd)"
popd > /dev/null 2>&1

OUT="$(pwd)/vendor/transgaming/swiftshader-src"

JOBS=$(grep '^processor' /proc/cpuinfo | wc -l)

rm -rf "${OUT}"
mkdir -p "${OUT}"
ln -s "${DIR}/.dir-locals.el" "${OUT}"

IFS=$'\n'
for i in $(find "${DIR}/src" -name Android.mk -print); do
  ln -s "$(dirname "${i}" )" "${OUT}"
done
unset IFS
. build/envsetup.sh
lunch gce_x86-userdebug
. device/google/gce_x86/configure_java.sh
rm -rf vendor/transgaming/swiftshader/x86
make -j ${JOBS} \
    libEGL_swiftshader_vendor_debug \
    libEGL_swiftshader_vendor_release \
    libGLESv1_CM_swiftshader_vendor_debug \
    libGLESv1_CM_swiftshader_vendor_release \
    libGLESv2_swiftshader_vendor_debug \
    libGLESv2_swiftshader_vendor_release

rm -rf vendor/transgaming/swiftshader/x86/*/obj

# JBMR2 (and earlier?) doesn't allow the library name to differ from the
# module name. Rename the generated libraries.
IFS=$'\n'
for i in $(find vendor/transgaming/swiftshader/x86 -name '*_vendor_*'); do
  j="${i/_vendor_debug/}"
  j="${j/_vendor_release/}"
  if [[ "$i" != "$j" ]]; then
    mv "$i" "$j"
  fi
done
unset IFS
