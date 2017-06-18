#!/bin/bash
# debian-allstar-install.sh - Used to install AllStar on Debian

# Script Start
echo "Welcome to the AllStar Debian installer."
echo "This script will install AllStar on your Debian server."
read -e -p "$(echo -e "Do you want to continue? [Y/N]" ) " install
if [[ "$install" = "y" ]] || [[ "$install" = "Y" ]]; then
  echo " Installing Git..."
  apt-get install git -y
  echo "Done"
  echo "downloading and unpacking source files."
  echo "This might take a while."
  cd /usr/src
  git clone https://github.com/w0jrl/raslink.git utils
  echo "Done"
  sleep 0.5
  echo "Updating the system..."
  (apt-get update;apt-get dist-upgrade -y)
  echo "Done"
  echo "Installing kernel headers..."
  apt-get install linux-headers-$(uname -r) -y
  echo "Done"
  sleep 0.5
  echo "Setting up log rotate..."
  echo "Logs will be rotated once a month."
  chmod +x /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
  echo "Done"
  sleep 0.5
  echo "Installing packages needed for stage two..."
  chmod +x /usr/src/utils/AllStar-build/common/required-tools.sh
  /usr/src/utils/AllStar-build/common/required-tools.sh
  echo "Done"
  sleep 0.5
# Setup for stage two
  cd /root
  mv .bashrc .bashrc.orig
  cat .bashrc.orig > .bashrc
  chmod +x /usr/src/utils/AllStar-build/debian/debian-install-stage2.sh
  echo "/usr/src/utils/AllStar-build/debian/debian-install-stage2.sh" >> .bashrc
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
