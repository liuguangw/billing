#!/bin/sh

osReleasePath=/etc/os-release
issuePath=/etc/issue.net
machineName=

if [ -f "${osReleasePath}" ]; then
  machineName=$(. ${osReleasePath} && echo "${PRETTY_NAME}")
elif [ -f "${issuePath}" ]; then
  machineName=$(cat ${issuePath})
else
  machineName=$(uname -s)
fi
if [ -n "${GITHUB_ACTIONS}" ]; then
 machineName="${machineName} (GitHub Actions)"
fi
echo "${machineName}"