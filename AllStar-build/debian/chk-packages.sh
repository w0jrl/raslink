#!/bin/bash
# Check package status, and install needed packages
# Script Start
subversion=/usr/bin/svn
echo "Checking Subversion..."
if [ -e $subversion ]
then
  echo "Removing Subversion; No longer needed for AllStar."
  apt-get autoremove --purge -y subversion
  rm -rf /root/.subversion
  echo "Done"
else
  echo "Subversion isn't installed; Skipping."
fi
echo "Checking status of required packages..."
sourcesList=$( grep -ic "#deb-src" /etc/apt/sources.list )
if [ $sourcesList -eq 1 ]; then
  sed -i 's/#deb-src/deb-src/' /etc/apt/sources.list
fi
chmod +x /usr/src/utils/AllStar-build/common/required-tools.sh
/usr/src/utils/AllStar-build/common/required-tools.sh
echo "Done"
exit 0
