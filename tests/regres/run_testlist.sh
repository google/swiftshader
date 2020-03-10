#!/bin/bash

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"

go run $ROOT_DIR/cmd/run_testlist/main.go --test-list=$ROOT_DIR/testlists/vk-master.txt $@
