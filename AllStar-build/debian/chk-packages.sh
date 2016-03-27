#!/bin/sh
# Check package status, and install needed packages
# Script Start
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
  echo "Done."
fi
sleep 1
echo "Checking curl..."
if [ -e $curl ]
then
  echo "Curl is already installed; skipping."
else
  echo "Installing curl..."
  apt-get install -y curl
  echo "Done."
fi
sleep 1
apt-get install ntpdate libtonezone-dev -y
ln -fs /etc/network/if-up.d/ntpdate /etc/cron.hourly/ntpdate
service cron restart
