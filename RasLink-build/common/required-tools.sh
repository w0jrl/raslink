#! /bin/bash
# required-tools.sh: Install packages required for RasLink
#   Copyright (C) 2016-2022  Jeremy Lincicome W0JRL
#   <https://jlappliedtechnologies.com>  <admin@jlappliedtechnologies.com>
#
#   This file is part of RasLink.
#   RasLink is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   RasLink is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with RasLink.  If not, see <http://www.gnu.org/licenses/>.
#
# Script Start
apt-get -qq install -y lsb-release
rpi="$(cat /proc/device-tree/model &>/dev/null | grep -ic 'raspberry pi')"
release="$(lsb_release -cs)"
distro="$(lsb_release -is)"
sourcesList="$(grep -riIho --include="*.list*" "#deb-src\|# deb-src" /etc/apt/ | wc -l)"
if (( "$sourcesList" >= "1" )); then
    grep -riIl --include="*.list*" "#deb-src\|# deb-src" /etc/apt/ | xargs sed -i 's/#deb-src/deb-src/g; s/# deb-src/deb-src/g'
    apt-get -qq update
fi
apt-get -qqy build-dep fail2ban
apt-get -qqy --fix-missing --fix-broken install\
    preload- ntpdate g++ gcc make build-essential automake git tmux screen- ssh\
    sudo ca-certificates libusb-dev libreadline-dev libedit-dev libgsm1-dev\
    libncurses5-dev libncursesw5-dev bison libasound2-dev\
    libcurl4-openssl-dev libiksemel-dev libvorbis-dev libjack-dev libss7-dev\
    curl sox libsox-fmt-all usbutils libsqlite3-dev\
    fxload bc alsa-utils dnsutils libtool autoconf gawk\
    libical-dev libspandsp-dev libxml2-dev pkg-config libspeex-dev libspeexdsp-dev unixodbc\
    unixodbc-dev uuid uuid-dev dkms\
    libatomic-ops-dev libatomic1 irqbalance libresample-dev libgmime-*-dev debhelper libsnmp-dev\
    zlib1g-dev procps alsa-oss libpri-dev\
    autotools-dev libtonezone-dev\
    dh-autoreconf dh-strip-nondeterminism dwz groff intltool-debian\
    libarchive-zip-perl libfile-stripnondeterminism-perl libice6 libnewt-dev\
    libpng-dev libslang2-dev libsm6 libtool libusb-1.0-0-dev libxaw7 libpopt-dev\
    libxml2-utils libxmu6 libxt6 m4 module-assistant po-debconf x11-common freetds-dev freetds-common freetds-bin binutils binutils-dev libcap-dev libpq-dev libblkid-dev\
    pulseaudio osspd osspd-pulseaudio dahdi-source i2c-tools libi2c-dev libjansson-dev\
    libboost-all-dev libgmp-dev swig python3-numpy\
    php-cli php-curl libssl1.1 libpth-dev\
    default-libmysqlclient-dev libneon27-dev libneon27 libssl-dev libssl-doc libsrtp2-dev corosync cluster-glue\
    asciidoc-base asciidoc-common asciidoc flex
if [[ "$release" = "stretch" ]]; then
    apt-get -qqy --fix-missing --fix-broken install\
    libcurl0 libcurl-dev
fi
if [[ "$rpi" = "1" ]]; then
    apt-get -qqy --fix-missing --fix-broken install\
    fake-hwclock
fi
if [[ "$distro" = "Raspbian" ]]; then
    apt-get -qq -y --fix-missing --fix-broken install raspberrypi-kernel raspberrypi-kernel-headers
    dpkg-reconfigure perl
else
    apt-get -qqy --fix-missing --fix-broken install\
    linux-headers-$(uname -r) linux-image-$(uname -r)
fi
# put the linker where Asterisk expects it
ln -sf /bin/ln /usr/bin &>/dev/null
exit 0
