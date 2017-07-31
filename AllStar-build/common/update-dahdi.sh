#! /bin/bash
# update-dahdi.sh - Build Dahdi for AllStar
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
echo "Downloading and unpacking dahdi..."
cd /usr/src/utils/
if [ -e /usr/src/utils/astsrc/dahdi* ]
then
  rm -rf /usr/src/utils/astsrc/dahdi*
fi
wget http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/dahdi-linux-complete-current.tar.gz &>/dev/null
cd /usr/src/utils/astsrc/
tar zxvf /usr/src/utils/dahdi-linux-complete-current.tar.gz &>/dev/null
mv dahdi* dahdi
rm -rf /usr/src/utils/*.tar.gz
echo "Done"
cd /usr/src/utils/astsrc/dahdi/
echo "Building dahdi..."
# Patch dahdi for use with AllStar
# https://allstarlink.org/dude-dahdi-2.10.0.1-patches-20150306
patch -p1 < /usr/src/utils/AllStar-build/patches/patch-dahdi-dude-current
# Remove setting the owner to asterisk
patch -p0 < /usr/src/utils/AllStar-build/patches/patch-dahdi.rules
# Build and install dahdi
(make all;make install;make config)
if [ "$(grep -ic "dahdi" /etc/modules)" == "1" ]; then
  sed -i '/dahdi/d' /etc/modules
fi
if [ -f /etc/init.d/dahdi ]; then
  update-rc.d dahdi remove
  rm -rf /etc/init.d/dahdi
  systemctl disable dahdi.timer
  rm -rf /etc/systemd/system/dahdi.timer
  systemctl daemon-reload
fi
echo "Done"
exit 0
