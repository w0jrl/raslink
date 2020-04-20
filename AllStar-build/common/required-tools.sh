#! /bin/bash
# required-tools.sh - Install packages required to install AllStar Debian/RasLink
#    Copyright (C) 2020  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Script Start
apt-get -qq install -y lsb-release
distro=$(lsb_release -is)
release=$(lsb_release -cs)
apt-get -qq --fix-missing --fix-broken install -y -ntpdate g++ gcc make build-essential automake git screen ssh\
  sudo ca-certificates libusb-dev libeditline-dev libreadline-dev libedit-dev libgsm1-dev\
  libncurses5-dev libncursesw5-dev bison libasound2-dev\
  libcurl4-openssl-dev libiksemel-dev libvorbis-dev libjack-dev libss7-dev\
  curl sox libsox-fmt-all usbutils libsqlite-dev libsqlite3-dev sqlite3-\
  fxload bc alsa-utils dnsutils libtool autoconf gawk\
  libical-dev libspandsp-dev libxml2-dev pkg-config libspeex-dev libspeexdsp-dev unixodbc unixodbc-dev uuid uuid-dev libsrtp0-dev dkms\
  libatomic-ops-dev libatomic1 irqbalance libresample-dev libgmime-2.6-dev debhelper libsnmp-dev\
  zlib1g-dev procps alsa-oss libpri-dev corosync-dev\
  autoconf autotools-dev libtonezone-dev\
  dh-autoreconf dh-strip-nondeterminism dwz groff intltool-debian\
  libarchive-zip-perl libfile-stripnondeterminism-perl libice6 libnewt-dev\
  libpng-dev libslang2-dev libsm6 libtool libusb-1.0-0-dev libxaw7 libpopt-dev\
  libxml2-utils libxmu6 libxt6 m4 module-assistant po-debconf x11-common freetds-dev freetds-common freetds-bin binutils binutils-dev libcap-dev libpq-dev
if [[ $release = "stretch" ]]; then
  apt-get -qq --fix-missing --fix-broken install -y php-cli php-curl libssl1.0-dev libpt-dev libneon27-gnutls-dev libneon27-gnutls default-libmysqlclient-dev libneon27-dev- libneon27- libssl-dev- libssl-doc-
else
  apt-get -qq --fix-missing --fix-broken install -y php5-cli php5-curl libssl-dev libpt-dev libneon27-gnutls libneon27-gnutls-dev libmysqlclient-dev libneon27-dev- libneon27-
fi
if [[ $distro = "Raspbian" ]]; then
  apt-get -qq --fix-missing --fix-broken install -y i2c-tools libi2c-dev raspberrypi-kernel raspberrypi-kernel-headers fake-hwclock
  dpkg-reconfigure perl
else
  apt-get -qq --fix-missing --fix-broken install -y linux-headers-$(uname -r) linux-image-$(uname -r)
fi
# put the linker where Asterisk expects it
ln -fs /bin/ln /usr/bin
exit 0
