#!/bin/bash

# update-marl merges the latest changes from the github.com/google/marl into
# third_party/marl. This script copies the change descriptions from the squash
# change into the top merge change, along with a standardized description.
REASON=$1

if [ ! -z "$REASON" ]; then
  REASON="\n$REASON\n"
fi

git subtree pull --prefix third_party/marl https://github.com/google/marl main --squash -m "Update marl"

ALL_CHANGES=`git log -n 1 HEAD^2 | egrep '^(\s{4}[0-9a-f]{9}\s*.*)$'`
HEAD_CHANGE=`echo "$ALL_CHANGES" | egrep '[0-9a-f]{9}' -o -m 1`
LOG_MSG=`echo -e "Update Marl to $HEAD_CHANGE\n${REASON}\nChanges:\n$ALL_CHANGES\n\nCommands:\n    ./third_party/update-marl.sh --squash\n\nBug: b/140546382"`
git commit --amend -m "$LOG_MSG"
