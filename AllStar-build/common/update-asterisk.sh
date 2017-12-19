#! /bin/bash
# update-asterisk.sh - Build asterisk for AllStar
#    Copyright (C) 2018  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Script Start
echo "Building asterisk..."
cd /usr/src/utils/astsrc/asterisk/
# Put git commit number where asterisk makefile expects it
git describe --always > .version
sleep 0.5s
# Configure the build process
(export PTLIB_CONFIG=/usr/share/ptlib/make/ptlib-config;./configure CXX=g++-4.8 CC=gcc-4.8)
# Optimize for the arm cpu if running on the Raspberry Pi
distro=$(lsb_release -is)
if [[ $distro = "Raspbian" ]]; then
  sed -i '/PROC\=/c\PROC\=arm' ./makeopts.in
fi
# Build and install Asterisk
(make;make install)
if [ -e /etc/init.d/asterisk ]; then
  (update-rc.d asterisk remove;rm /etc/init.d/asterisk)
fi
# Fix comment in rpt.conf
sed -i 's/Say phonetic call sign/Say call sign/' /etc/asterisk/rpt* | sed -i 's/say phonetic call sign/Say call sign/' /etc/asterisk/rpt*
# Load res_crypto module
sed -i '/res_crypto.so/c\load \=> res_crypto.so ;   Cryptographic Digital Signatures                  ' /etc/asterisk/modules.conf
# Load app_sendtext module
sed -i '/app_sendtext.so/c\load \=> app_sendtext.so ;   Send Text Applications                            ' /etc/asterisk/modules.conf
# Remove low pass and high pass filter configuration from usbradio
sed -i -e '/rxlpf \= 0/,+19d' /etc/asterisk/usbradio.conf
# Increase jbmaxsize in usbradio
sed -i 's/jbmaxsize \= 200/jbmaxsize \= 500/' /etc/asterisk/usbradio*
echo "Done."
exit 0
