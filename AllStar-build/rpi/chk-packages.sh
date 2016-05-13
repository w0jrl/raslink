#!/bin/bash
# Check package status, and remove packages that aren't needed.

# Script Start
echo "Removing unneeded packages and sources."
sleep 1
echo "Looking for Sense-hat..."
if [ -e /usr/src/sense-hat ]
then
  echo "Removing unneeded Sense-hat directory..."
  rm -rf /usr/src/sense-hat
  echo "Done"
else
  echo "Sense-hat doesn't exist on your system; Skipping."
fi
sleep 1
echo "Looking for Libreoffice..."
if [ -e /usr/bin/libreoffice ]
then
  echo "Uninstalling Libreoffice; Not needed for AllStar."
  apt-get purge -y libreoffice*
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  sleep 1
  echo "Done"
else
  echo "Libreoffice isn't installed; Skipping."
fi
sleep 1
echo "Looking for Minecraft..."
if [ -e /usr/bin/minecraft-pi ]
then
  echo "Uninstalling Minecraft; Not needed for AllStar."
  apt-get purge -y minecraft-pi
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  sleep 1
  echo "Done"
else
  echo "Minecraft isn't installed; Skipping."
fi
sleep 1
echo "Looking for Wolfram Engine..."
if [ -e /usr/bin/wolfram ]
then
  echo "Uninstalling Wolfram Engine; Not needed for AllStar."
  apt-get purge -y wolfram-engine
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  sleep 1
  echo "Removing Wolfram Engine data..."
  rm -rf /opt/Wolfram
  sleep 1
  echo "Done"
else
  echo "Wolfram Engine isn't installed; Skipping."
fi
sleep 1
echo "Looking for Penguins Puzzle..."
if [ -e /usr/bin/penguinspuzzle ]
then
  echo "Uninstalling Penguins Puzzle; Not needed for AllStar."
  apt-get purge -y penguinspuzzle
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  sleep 1
  echo "Done"
else
  echo "Penguins Puzzle isn't installed; Skipping."
fi
sleep 1
echo "Looking for Red Notebook..."
if [ -e /usr/bin/rednotebook ]
then
  echo "Uninstalling red Notebook; Not needed for AllStar."
  apt-get purge -y rednotebook
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
  sleep 1
  echo "Done"
else
  echo "Red Notebook isn't installed; Skipping."
fi
sleep 1
chmod +x /usr/src/utils/AllStar-build/rpi/add-pi.sh
/usr/src/utils/AllStar-build/rpi/add-pi.sh
sleep 1
echo "Looking for RPI Update..."
if [ -e /usr/bin/rpi-update ]
then
  echo "Uninstalling RPI Update; No longer needed for AllStar."
  apt-get purge -y rpi-update
  echo "Cleaning the database..."
  (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean;apt-get update;apt-get dist-upgrade -y)
  sleep 1
  echo "Done"
else
  echo "RPI Update isn't installed; Skipping."
fi
sleep 1
sources=/opt/vc/src
echo "Looking for $sources..."
if [ -e $sources ]
then
  echo "Removing $sources; Not needed for AllStar."
  rm -rf $sources
  sleep 1
  echo "Done"
else
  echo "$sources doesn't exist on your system; Skipping."
fi
sleep 1
echo "Checking status of required packages."
sleep 1
curl=/usr/bin/curl
sqlite3=/usr/bin/sqlite3
screen=/usr/bin/screen
echo "Checking Sqlite3..."
if [ -e $sqlite3 ]
then
  echo "Sqlite3 is already installed; skipping."
else
  echo "Installing Sqlite3..."
  apt-get install -y libsqlite3-dev sqlite3
  sleep 1
  echo "Done"
fi
sleep 1
echo "Checking Curl..."
if [ -e $curl ]
then
  echo "Curl is already installed; skipping."
else
  echo "Installing Curl..."
  apt-get install -y curl
  sleep 1
  echo "Done"
fi
sleep 1
echo "Checking Screen..."
if [ -e $screen ]
then
  echo "Screen is already installed; skipping."
else
  echo "Installing Screen..."
  apt-get install -y screen
  sleep 1
  echo "Done."
fi
sleep 1
echo "checking Logrotate status..."
if [ -e /etc/logrotate.d/asterisk ]
then
  echo "Logrotate parameters are already up to date; skipping."
else
  echo "updating Logrotate parameters..."
  sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=100m 0 0/d' /etc/fstab
  chmod +x /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  sleep 1
  echo "Logs will be rotated once a month."
  sleep 2
  echo "Done"
fi
sleep 1
apt-get install ntpdate libtonezone-dev automake automake1.9 -y
ln -fs /etc/network/if-up.d/ntpdate /etc/cron.hourly/ntpdate
service cron restart
