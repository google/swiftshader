#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"

set -e # Fail on first failure

# The Docker image should have set these.
[ -x $CLANG_FORMAT ]
[ -x $GOFMT ]

# Run presubmit tests
cd "$SCRIPT_DIR/../../.."
./tests/presubmit.sh
