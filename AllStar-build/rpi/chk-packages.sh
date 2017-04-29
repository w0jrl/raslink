#!/bin/bash
# chk-packages.sh - Check package status, and remove packages that aren't needed.

# Script Start
echo "Removing unneeded packages and sources."
echo "Looking for Sense-hat..."
if [ -e /usr/src/sense-hat ]; then
  echo "Removing unneeded Sense-hat directory..."
  rm -rf /usr/src/sense-hat
  echo "Done"
else
  echo "Sense-hat doesn't exist on your system; Skipping."
fi
echo "Looking for Libreoffice..."
if [ -e /usr/bin/libreoffice ]; then
  echo "Uninstalling Libreoffice; Not needed for AllStar."
  apt-get purge -y libreoffice*
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  echo "Done"
else
  echo "Libreoffice isn't installed; Skipping."
fi
echo "Looking for Minecraft..."
if [ -e /usr/bin/minecraft-pi ]; then
  echo "Uninstalling Minecraft; Not needed for AllStar."
  apt-get purge -y minecraft-pi
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  echo "Done"
else
  echo "Minecraft isn't installed; Skipping."
fi
echo "Looking for Wolfram Engine..."
if [ -e /usr/bin/wolfram ]; then
  echo "Uninstalling Wolfram Engine; Not needed for AllStar."
  apt-get purge -y wolfram-engine
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  echo "Removing Wolfram Engine data..."
  rm -rf /opt/Wolfram
  echo "Done"
else
  echo "Wolfram Engine isn't installed; Skipping."
fi
echo "Looking for Penguins Puzzle..."
if [ -e /usr/bin/penguinspuzzle ]; then
  echo "Uninstalling Penguins Puzzle; Not needed for AllStar."
  apt-get purge -y penguinspuzzle
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  echo "Done"
else
  echo "Penguins Puzzle isn't installed; Skipping."
fi
echo "Looking for Red Notebook..."
if [ -e /usr/bin/rednotebook ]; then
  echo "Uninstalling red Notebook; Not needed for AllStar."
  apt-get purge -y rednotebook
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  echo "Done"
else
  echo "Red Notebook isn't installed; Skipping."
fi
chmod +x /usr/src/utils/AllStar-build/rpi/rm-pi.sh
/usr/src/utils/AllStar-build/rpi/rm-pi.sh
echo "Looking for RPI Update..."
if [ -e /usr/bin/rpi-update ]; then
  echo "Uninstalling RPI Update; No longer needed for AllStar."
  apt-get purge -y rpi-update
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  echo "Done"
else
  echo "RPI Update isn't installed; Skipping."
fi
subversion=/usr/bin/svn
echo "Checking Subversion..."
if [ -e "$subversion" ]; then
  echo "Removing Subversion; No longer needed for AllStar."
  apt-get autoremove --purge -y subversion
  rm -rf /root/.subversion
  echo "Done"
else
  echo "Subversion isn't installed; Skipping."
fi
echo "Checking status of required packages..."
sourcesList=$( grep -ic "#deb-src" /etc/apt/sources.list )
if [ "$sourcesList" == "1" ]; then
  sed -i 's/#deb-src/deb-src/' /etc/apt/sources.list
fi
chmod +x /usr/src/utils/AllStar-build/common/required-tools.sh
/usr/src/utils/AllStar-build/common/required-tools.sh
echo "Done"
echo "checking Logrotate status..."
fstab=$(grep -ic "/var/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0" /etc/fstab )
if [ "$fstab" == "1" ]; then
  echo "Old fstab log setting detected; Removing."
  sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0/d' /etc/fstab
  echo "Done"
fi
if [[ -e /etc/logrotate.d/asterisk ]] && [[ "$(grep -ic "compress" /etc/logrotate.d/asterisk)" == "1" ]]; then
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
