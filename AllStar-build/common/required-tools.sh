#! /bin/bash
# required-tools.sh - Install packages required to install AllStar Debian/Raspbian

# Script Start
distro=$(lsb_release -is)
apt-get install -y ntpdate g++ make build-essential automake git\
  sudo ca-certificates libusb-dev libnewt-dev libeditline0\
  libncurses5-dev bison libssl-dev libasound2-dev\
  libcurl4-openssl-dev php5-cli libiksemel-dev libvorbis-dev\
  curl sox libsox-fmt-all usbutils libsqlite3-dev sqlite3 screen\
  fxload bc alsa-utils dnsutils php5-curl libtool autoconf gawk\
  libical-dev libspandsp-dev libneon27-dev libxml2-dev pkg-config unixodbc-dev uuid uuid-dev libsrtp0-dev dkms
apt-get build-dep dahdi -y
if [[ $distro = "Raspbian" ]]; then
  apt-get install i2c-tools libi2c-dev
fi
# put the linker where Asterisk expects it
ln -fs /bin/ln /usr/bin
exit 0
