#! /bin/sh -e
# Used to download sources for Dahdi, Libpri, and Asterisk
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi's!
# You have been warned!

# Script Start
cd /usr/src/utils
rm -rf astsrc
svn checkout http://svn.ohnosec.org/svn/projects/allstar/astsrc-1.4.23-pre/trunk astsrc
# grab the svn version number and put it where asterisk/Makefile expects it.
cd /usr/src/utils/astsrc
svnversion >asterisk/.version
# download uridiag
svn co http://svn.ohnosec.org/svn/projects/allstar/uridiag/trunk uridiag
cd /usr/src/utils
wget http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/dahdi-linux-complete-current.tar.gz
# Clean out unneeded source and add dahdi source
cd /usr/src/utils/astsrc
rm -rf zaptel
tar zxvf /usr/src/utils/dahdi-linux-complete-current.tar.gz
mv dahdi-linux-complete* dahdi-linux-complete
cd /usr/src/utils/
rm -rf *.tar.gz

