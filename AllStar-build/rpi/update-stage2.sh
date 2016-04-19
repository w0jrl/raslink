#!/bin/sh
# Used to update the system
# Stage Two

# Script Start
echo "Running update, stage two."
echo "This will take a while."
(systemctl stop asterisk;systemctl stop dahdi)
sleep 1
echo "Your node can not be used durring this process. It has been disabled."
sleep 1
chmod +x /usr/src/utils/AllStar-build/scripts/chk-packages.sh
/usr/src/utils/AllStar-build/scripts/chk-packages.sh
sleep 1
echo "Building Dahdi..."
sleep 1
cd /usr/src/utils/astsrc
		cd ./dahdi*
make clean
make
make install
echo "Done"
sleep 1
echo "Building Libpri..."
sleep 1
cd ../libpri
make clean
make
make install
sleep 1
systemctl start dahdi
echo "Done"
sleep 1
echo "Building Asterisk..."
sleep 1
cd ../asterisk
./configure
make clean
make menuselect.makeopts
make
make install
echo "Done"
sleep 1
echo "Building URI diag...{
cd ../uridiag
make
chmod +x uridiag
cp uridiag /usr/local/bin/uridiag
echo "Done"
sleep 1
# restore bashrc
mv /root/.bashrc.orig /root/.bashrc
# make sure configuration files and scripts are loaded
echo "Updating start up scripts..."
cp /usr/src/utils/AllStar-build/configs/modules.conf /etc/asterisk/modules.conf
(cp /usr/src/utils/AllStar-build/common/rc.allstar /usr/local/bin/rc.allstar;chmod +x /usr/local/bin/rc.allstar)
killall rc.updatenodelist
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
systemctl stop dahdi
chmod +x /usr/src/utils/AllStar-build/rpi/make-links.sh
/usr/src/utils/AllStar-build/rpi/make-links.sh
echo "Done"
sleep 1
echo "Resetting compiler flags..."
cd /usr/src/utils
git reset --hard HEAD
echo "Done"
sleep 1
echo "Updating system boot configuration..."
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
cp /usr/src/utils/AllStar-build/common/rc.local /etc/rc.local
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
systemctl daemon-reload
echo "Done"
sleep 1
echo "The update is complete."
echo "You can run this tool at any time by typing 'system-update' at a root prompt."
sleep 1
echo "Rebooting your node to apply the new boot configuration."
sync
sudo reboot

