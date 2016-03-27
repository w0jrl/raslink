#!/bin/sh
echo "Welcome to the AllStar Debian installer."
echo "This script will install AllStar on your Debian server."
echo "If you don't want to install AllStar, press CTRL+c now to abort."
sleep 5
echo "downloading and unpacking source files."
echo "This might take a while."
cd /usr/src
wget -qq http://jlappliedtechnologies.com/wordpress/wp-content/uploads/allstar/sources-jessie.tar.gz -O- | tar xzv
echo "Done"
sleep 1
echo "Updating the system."
(apt-get update;apt-get dist-upgrade -y)
echo "Done"
echo "Installing kernel headers."
apt-get install linux-headers-$(uname -r) -y
echo "Done"
sleep 1
echo "Setting up log rotate."
echo "Logs will be rotated once a month."
chmod +x /usr/src/utils/AllStar-build/debian/mk-logrotate-asterisk.sh
/usr/src/utils/AllStar-build/debian/mk-logrotate-asterisk.sh
echo "Done"
sleep 1
echo "Installing packages."
chmod +x /usr/src/utils/AllStar-build/debian/required-tools.sh
/usr/src/utils/AllStar-build/debian/required-tools.sh
echo "Done"
sleep 1
# setup for stage two
cd /root
mv .bashrc .bashrc.orig
cat .bashrc.orig > .bashrc
echo "/usr/src/utils/AllStar-build/debian/debian-install-stage2.sh" >> .bashrc
echo "After the system reboots, you need to login as the root user to finish the installation."
echo "Rebooting to finish install."
sleep 1
sync
sudo reboot

