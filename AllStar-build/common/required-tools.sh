#! /bin/bash
# required-tools.sh - Install packages required to install AllStar Debian/RasLink
#    Copyright (C) 2017  Jeremy Lincicome (W0JRL)
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
distro=$(lsb_release -is)
apt-get install -y ntpdate g++ make build-essential automake git\
  sudo ca-certificates libusb-dev libnewt-dev libeditline0\
  libncurses5-dev bison libssl-dev libasound2-dev\
  libcurl4-openssl-dev php5-cli libiksemel-dev libvorbis-dev\
  curl sox libsox-fmt-all usbutils libsqlite3-dev sqlite3 screen\
  fxload bc alsa-utils dnsutils php5-curl libtool autoconf gawk\
  libical-dev libspandsp-dev libneon27-dev libxml2-dev pkg-config unixodbc-dev uuid uuid-dev libsrtp0-dev dkms\
  libatomic-ops-dev libatomic1 irqbalance
apt-get build-dep dahdi -y
if [[ $distro = "Raspbian" ]]; then
  apt-get install i2c-tools libi2c-dev
fi
# put the linker where Asterisk expects it
ln -fs /bin/ln /usr/bin
exit 0
