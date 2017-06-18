#!/bin/bash
# remote-fetch.sh - Update git repository URL if needed

# Script Start
cd /usr/src/utils
echo "Checking repository URL..."
if [ "$(git config --get remote.origin.url | grep -ic "AllStar-raspbian")" == "1" ]; then
  echo "Updating repository URL and data..."
  git remote set-url origin https://github.com/w0jrl/raslink.git
  git pull &>/dev/null
  echo "Done"
else
  echo "The repository URL is up-to-date; Skipping."
fi
exit 0
