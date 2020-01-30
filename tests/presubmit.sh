#!/bin/bash

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")"/.. >/dev/null 2>&1 && pwd )"
SRC_DIR=${ROOT_DIR}/src
TESTS_DIR=${ROOT_DIR}/tests

# Presubmit Checks Script.
CLANG_FORMAT=${CLANG_FORMAT:-clang-format}
GOFMT=${GOFMT:-gofmt}

if test -t 1; then
  ncolors=$(tput colors)
  if test -n "$ncolors" && test $ncolors -ge 8; then
    normal="$(tput sgr0)"
    red="$(tput setaf 1)"
    green="$(tput setaf 2)"
  fi
fi

function check() {
  local name=$1; shift
  echo -n "Running check $name... "

  if ! "$@"; then
    echo "${red}FAILED${normal}"
    echo "  Error executing: $@";
    exit 1
  fi

  if ! git diff --quiet HEAD; then
    echo "${red}FAILED${normal}"
    echo "  Git workspace not clean:"
    git --no-pager diff -p HEAD
    echo "${red}Check $name failed.${normal}"
    exit 1
  fi

  echo "${green}OK${normal}"
}

# Validate commit message
function run_bug_in_commit_msg() {
  git log -1 --pretty=%B | grep -E '^(Bug|Issue|Fixes):(\s?)(((b\/)|(\w+:))([0-9]+)|[^0-9]+)$|(^Regres:)|(^PiperOrigin-RevId:)'

  if [ $? -ne 0 ]
  then
    echo "${red}Git commit message must have a Bug: line"
    echo "followed by a bug ID in the form b/# for Buganizer bugs or"
    echo "project:# for Monorail bugs (e.g. 'Bug: chromium:123')."
    echo "Omit any digits when no ID is required (e.g. 'Bug: fix build').${normal}"
    return 1
  fi
}

function run_copyright_headers() {
  tmpfile=`mktemp`
  for suffix in "cpp" "hpp" "go" "h"; do
    # Grep flag '-L' print files that DO NOT match the copyright regex
    # Grep seems to match "(standard input)", filter this out in the for loop output
    find ${SRC_DIR} -type f -name "*.${suffix}" | xargs grep -L "Copyright .* The SwiftShader Authors\|Microsoft Visual C++ generated\|GNU Bison"
  done | grep -v "(standard input)" > ${tmpfile}
  if test -s ${tmpfile}; then
    # tempfile is NOT empty
    echo "${red}Copyright issue in these files:"
    cat ${tmpfile}
    rm ${tmpfile}
    echo "${normal}"
    return 1
  else
    rm ${tmpfile}
    return 0
  fi
}

function run_clang_format() {
  ${SRC_DIR}/clang-format-all.sh
}

function run_gofmt() {
  find ${SRC_DIR} ${TESTS_DIR} -name "*.go" | xargs $GOFMT -w
}

# Ensure we are clean to start out with.
check "git workspace must be clean" true

# Check for 'Bug: ' line in commit
check bug-in-commi-msg run_bug_in_commit_msg

# Check copyright headers
check copyright-headers run_copyright_headers

# Check clang-format.
check clang-format run_clang_format

# Check gofmt.
check gofmt run_gofmt

echo
echo "${green}All check completed successfully.${normal}"
