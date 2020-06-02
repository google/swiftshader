#!/bin/bash

set -e # Fail on any error.

ROOT_DIR=`pwd`
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"

docker run --rm -i \
  --volume "${ROOT_DIR}:${ROOT_DIR}" \
  --volume "${KOKORO_ARTIFACTS_DIR}:/mnt/artifacts" \
  --workdir "${ROOT_DIR}" \
  --env BUILD_SYSTEM=$BUILD_SYSTEM \
  --env BUILD_TOOLCHAIN=$BUILD_TOOLCHAIN \
  --env BUILD_TARGET_ARCH=$BUILD_TARGET_ARCH \
  --env BUILD_SANITIZER=$BUILD_SANITIZER \
  --entrypoint "${SCRIPT_DIR}/presubmit-docker.sh" \
  "gcr.io/shaderc-build/radial-build:latest"
