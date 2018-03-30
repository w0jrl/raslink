#! /bin/bash
# required-tools.sh - Install packages required to install AllStar Debian/RasLink
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
apt-get -qq update
apt-get -qq install -y lsb-release
distro=$(lsb_release -is)
release=$(lsb_release -cs)
apt-get -qq install -y ntpdate g++-4.8  g++ make build-essential automake git\
  sudo ca-certificates libusb-dev libnewt-dev libeditline-dev libedit-dev\
  libncurses5-dev libncursesw5-dev bison libasound2-dev\
  libcurl4-openssl-dev libiksemel-dev libvorbis-dev\
  curl sox libsox-fmt-all usbutils libsqlite3-dev sqlite3 screen\
  fxload bc alsa-utils dnsutils libtool autoconf gawk\
  libical-dev libspandsp-dev libxml2-dev pkg-config libspeex-dev unixodbc unixodbc-dev uuid uuid-dev libsrtp0-dev dkms\
  libatomic-ops-dev libatomic1 irqbalance libresample-dev libgmime-2.6-dev ssh libssl-dev libneon27-dev debhelper libsnmp-dev
apt-get -qq build-dep dahdi -y
if [[ $release = "stretch" ]]; then
  apt-get -qq install -y php-cli php-curl
else
  apt-get -qq install -y php5-cli php5-curl
fi
if [[ $distro = "Raspbian" ]]; then
  apt-get -qq install -y i2c-tools libi2c-dev raspberrypi-kernel raspberrypi-kernel-headers fake-hwclock
else
  apt-get -qq install -y linux-headers-$(uname -r)
fi
(apt-get -qq autoremove --purge -y;apt-get -qq clean;apt-get -qq autoclean)
# put the linker where Asterisk expects it
ln -fs /bin/ln /usr/bin
exit 0
