#!/bin/sh

osReleasePath=/etc/os-release
issuePath=/etc/issue.net
if [ -f "${osReleasePath}" ]; then
  . ${osReleasePath} && echo "$PRETTY_NAME"
elif [ -f "${issuePath}" ]; then
  cat ${issuePath}
else
  uname -s
fi
