#! /bin/bash
# Install libs required to install AllStar Debian
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on new Debian servers, or development Raspberry Pi's!
# You have been warned!

# Script Start
apt-get remove nfs-common -y
apt-get purge rpcbind -y
apt-get autoremove -y
apt-get install ntpdate -y
apt-get install g++ -y
apt-get install make -y
apt-get install build-essential automake -y
apt-get install git -y
apt-get install subversion -y
apt-get install sudo ca-certificates -y
apt-get install libusb-dev -y
apt-get install libnewt-dev -y
apt-get install libeditline0 -y
apt-get install libncurses5-dev -y
apt-get install bison -y
apt-get install libssl-dev -y
apt-get install libasound2-dev -y
apt-get install libcurl4-gnutls-dev -y
apt-get install php5-cli -y
apt-get install libiksemel-dev -y
apt-get install libvorbis-dev -y
apt-get install curl -y
apt-get install sox -y
apt-get install usbutils -y
apt-get install libsqlite3-dev sqlite3 -y
apt-get install screen -y
# link libraries
ln -fs /bin/ln /usr/bin
ln -fs /etc/network/if-up.d/ntpdate /etc/cron.hourly/ntpdate

