#! /bin/bash
# update-asterisk.sh - Used to build asterisk for AllStar

# Script Start
echo "Building asterisk..."
cd /usr/src/utils/astsrc/asterisk/
# Put git commit number where asterisk makefile expects it
git describe --always > .version
# Configure the build process
distro=$(lsb_release -is)
if [[ $distro = "Raspbian" ]]; then
  sed -i '/PROC\=/c\PROC\=arm' ./makeopts.in
fi
(make clean;./configure)
# Build and install Asterisk
(make all;make install)
# Fix comment in rpt.conf
sed -i 's/Say phonetic call sign/Say call sign/' /etc/asterisk/rpt* | sed -i 's/say phonetic call sign/Say call sign/' /etc/asterisk/rpt*
# Remove unneeded filter comments in usbradio.conf
sed -i '/Audio to internet/d' /etc/asterisk/usbradio*
sed -i '/Audio from internet/d' /etc/asterisk/usbradio*
echo "Done."
exit 0
