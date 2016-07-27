#!/bin/bash
# Check package status, and install needed packages
# Script Start
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
  echo "Done."
fi
sleep 1
echo "Checking Curl..."
if [ -e $curl ]
then
  echo "Curl is already installed; skipping."
else
  echo "Installing Curl..."
  apt-get install -y curl
  echo "Done."
fi
sleep 1
echo "Checking Screen..."
if [ -e $screen ]
then
  echo "Screen is already installed; skipping."
else
  echo "Installing Screen..."
  apt-get install -y screen
  echo "Done."
fi
sleep 1
echo "checking Asterisk, Libpri, and Dahdi dependencies..."
apt-get install ntpdate libtonezone-dev automake fxload php5-curl libtool autoconf libical-dev libspandsp-dev libneon27-dev libxml2-dev pkg-config unixodbc-dev uuid uuid-dev libsrtp0-dev -y
sourcesList=$( grep -ic "#deb-src" /etc/apt/sources.list )
if [ $sourcesList -eq 1 ]; then
  sed -i 's/#deb-src/deb-src/' /etc/apt/sources.list
  echo "Installing new dependencies."
  apt-get update; apt-get build-dep dahdi -y
else
  apt-get build-dep dahdi -y
fi
ln -fs /etc/network/if-up.d/ntpdate /etc/cron.hourly/ntpdate
service cron restart
echo "Done"
exit 0
