#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"

set -e # Fail on first failure

. /bin/using.sh

set -x # Display commands being run.

using clang-13.0.1
using go-1.23.4

# Run presubmit tests
cd "$SCRIPT_DIR/../../.."
./tests/presubmit.sh
