#!/bin/bash
# raslink-pc-install.sh - Install AllStar on Debian
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
echo "Welcome to the RasLink-pc installer."
echo "This script will install AllStar on your Debian server."
read -e -p "$(echo -e "Do you want to continue? [Y/N]" ) " install
if [[ "$install" = "y" ]] || [[ "$install" = "Y" ]]; then
  echo " Installing Git..."
  apt-get install git -y
  echo "Done"
  echo "downloading and unpacking source files."
  echo "This might take awhile."
  cd /usr/src
  git clone https://gitlab.com/w0jrl/raslink.git utils
  echo "Done"
  sleep 0.5
  echo "Updating the system..."
  (apt-get update;apt-get dist-upgrade -y)
  echo "Done"
  sleep 0.5
  echo "Setting up log rotate..."
  echo "Logs will be rotated once a month."
  chmod +x /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  echo "Done"
  sleep 0.5
  echo "Configuring packages..."
  chmod +x /usr/src/utils/AllStar-build/debian/chk-packages.sh
  /usr/src/utils/AllStar-build/debian/chk-packages.sh
  echo "Done"
  sleep 0.5
# Setup for stage two
  cd /root
  mv .bashrc .bashrc.orig
  cat .bashrc.orig > .bashrc
  chmod +x /usr/src/utils/AllStar-build/debian/raslink-pc-install-stage2.sh
  echo "/usr/src/utils/AllStar-build/debian/raslink-pc-install-stage2.sh" >> .bashrc
  echo "After the system reboots, you need to log in as the root user to finish the installation."
  echo "Rebooting to finish install."
  sync
  sudo reboot
  exit 0
else
  echo "AllStar will not be installed."
  echo "When you're ready to install AllStar, run this script again."
  echo "Exiting"
fi
exit 1
