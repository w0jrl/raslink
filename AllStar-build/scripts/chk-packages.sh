#!/bin/sh
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
chmod +x /usr/src/utils/AllStar-build/scripts/add-pi.sh
/usr/src/utils/AllStar-build/scripts/add-pi.sh
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
echo "Checking sqlite3..."
if [ -e $sqlite3 ]
then
  echo "Sqlite3 is already installed; skipping."
else
  echo "Installing sqlite3..."
  apt-get install -y libsqlite3-dev sqlite3
  sleep 1
  echo "Done"
fi
sleep 1
echo "Checking curl..."
if [ -e $curl ]
then
  echo "Curl is already installed; skipping."
else
  echo "Installing curl..."
  apt-get install -y curl
  sleep 1
  echo "Done"
fi
sleep 1
apt-get install ntpdate libtonezone-dev -y
ln -fs /etc/network/if-up.d/ntpdate /etc/cron.hourly/ntpdate
service cron restart