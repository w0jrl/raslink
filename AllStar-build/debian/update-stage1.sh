#!/bin/bash
# Used to update the system
# Stage One

# Script Start
echo "Starting system update."
echo "This will take a while."
echo "You can continue using your node during this process."
sleep 2
# get new sources
echo "Updating source files for All Star..."
cd /usr/src/utils
git pull
sleep 0.5
echo "Done"
# update the system
echo "Updating system software..."
(apt-get update;apt-get dist-upgrade -y)
sleep 0.5
echo "Done"
# clean the package database
echo "Cleaning up unneeded software..."
(apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
sleep 0.5
echo "Done"
# setup for stage two
cd /root
mv .bashrc .bashrc.orig
cat .bashrc.orig > .bashrc
echo "/usr/src/utils/AllStar-build/debian/update-stage2.sh" >> .bashrc
echo "Rebooting to finish install."
echo "When your node reboots, you need to login"
echo "to finish the update."
sync
sudo reboot
exit 0
