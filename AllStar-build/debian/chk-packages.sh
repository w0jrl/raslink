#!/bin/bash
# Check package status, and install needed packages
# Script Start
subversion=/usr/bin/svn
echo "Checking Subversion..."
if [ -e $subversion ]; then
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
echo "checking Logrotate status..."
fstab=$(grep -ic "/var/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0" /etc/fstab )
if [ $fstab -eq 1 ]; then
  echo "Old fstab log setting detected; Removing."
  sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0/d' /etc/fstab
  echo "Done"
fi
if [[ -e /etc/logrotate.d/asterisk ]] && [[ `grep -ic "compress" /etc/logrotate.d/asterisk` -eq 1 ]]; then
  echo "Logrotate parameters are already up to date; Skipping."
else
  echo "updating Logrotate parameters..."
  sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=100m 0 0/d' /etc/fstab
  if [ -e /etc/logrotate.d/asterisk ]; then
    rm -rf /etc/logrotate.d/asterisk
  fi
  chmod +x /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  echo "Logs will be rotated once a month."
  echo "Done"
fi
exit 0
