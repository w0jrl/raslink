#!/bin/bash
# Used to update the system
# Stage Two

# Script Start
echo "Running update, stage two."
echo "This will take a while."
(killall rc.local;systemctl stop dahdi.timer;systemctl stop asterisk.timer;systemctl stop asterisk.service;systemctl stop dahdi) &>/dev/null
sleep 1
echo "Your node can not be used durring this process. It has been disabled."
sleep 1
chmod +x /usr/src/utils/AllStar-build/rpi/chk-packages.sh
/usr/src/utils/AllStar-build/rpi/chk-packages.sh
sleep 1
chmod +x /usr/src/utils/AllStar-build/common/update-dahdi.sh
/usr/src/utils/AllStar-build/common/update-dahdi.sh
sleep 1
chmod +x /usr/src/utils/AllStar-build/common/update-libpri.sh
/usr/src/utils/AllStar-build/common/update-libpri.sh
sleep 1
chmod +x /usr/src/utils/AllStar-build/common/update-asterisk.sh
/usr/src/utils/AllStar-build/common/update-asterisk.sh
sleep 1
echo "Building URI diag..."
cd /usr/src/utils/astsrc/uridiag
make install
echo "Done"
sleep 1
# make sure configuration files and scripts are loaded
echo "Updating start up scripts..."
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
chmod +x /usr/src/utils/AllStar-build/rpi/make-links.sh
/usr/src/utils/AllStar-build/rpi/make-links.sh
echo "Done"
sleep 1
echo "Resetting compiler flags..."
cd /usr/src/utils
git clean -fd
echo "Done"
sleep 1
echo "Updating system boot configuration..."
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
systemctl daemon-reload
systemctl enable asterisk.timer
systemctl enable dahdi.timer
systemctl enable updatenodelist.service
startup=$(grep -ic "/usr/local/bin/rc.allstar" /etc/rc.local )
if [ $startup -eq 1 ]
then
  sed -i '/\# start allstar/d' /etc/rc.local
  sed -i '/sleep 30/d' /etc/rc.local
  sed -i '/\/usr\/local\/bin\/rc.allstar/d' /etc/rc.local
  sed -i 's/^ *//; s/ *$//; /^$/d; /^\s*$/d' /etc/rc.local
  sed -i '$ i\' /etc/rc.local
fi
if [ -e /usr/local/rc.allstar ]
then
  rm /usr/local/bin/rc.allstar
fi
echo "Done"
sleep 1
echo "The update is complete."
echo "You can run this tool at any time by typing 'system-update' at a root prompt."
sleep 1
echo "Rebooting your node to apply the new boot configuration."
# restore bashrc
mv /root/.bashrc.orig /root/.bashrc
sync
sudo reboot

