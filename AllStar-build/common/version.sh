#!/bin/bash
# Used to display software version, and check
# for available updates
# Does not perform system update
# Will run automaticly at root login

# Script Start
cd /usr/src/utils
echo "Your software version is: `git describe --always`."
echo "Your last update was on: `cat /root/.lastupdate`."
echo "Checking for available updates..."
git fetch &>/dev/null
echo "Done"
echo "Your repository branch is set to: `git status | grep "branch" | sed 's/On branch//' | sed 's/git pull/system-update/' | sed 's/to update your local branch/to update your node/'`"
echo "You can review this information at any time by typing 'version' at a root prompt."
exit 0
