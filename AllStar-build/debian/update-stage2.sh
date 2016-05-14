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
chmod +x /usr/src/utils/AllStar-build/debian/chk-packages.sh
/usr/src/utils/AllStar-build/debian/chk-packages.sh
sleep 1
echo "Building Dahdi..."
sleep 1
cd /usr/src/utils/astsrc
cd ./dahdi*
make
make install
echo "Done"
sleep 1
echo "Building libpri..."
sleep 1
cd ../libpri
make
make install
sleep 1
systemctl start dahdi
echo "Done"
sleep 1
echo "Building asterisk..."
sleep 1
cd ../asterisk
./configure
make
make install
echo "Done"
sleep 1
echo "Building URI diag..."
cd ../uridiag
make install
echo "Done"
sleep 1
# make sure configuration files and scripts are loaded
echo "Updating start up scripts..."
cp /usr/src/utils/AllStar-build/configs/modules.conf /etc/asterisk/modules.conf
(cp /usr/src/utils/AllStar-build/common/rc.allstar /usr/local/bin/rc.allstar;chmod +x /usr/local/bin/rc.allstar)
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
chmod +x /usr/src/utils/AllStar-build/common/asterisk-restart.sh
ln -fs /usr/src/utils/AllStar-build/common/asterisk-restart.sh /usr/bin/asterisk-restart
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
chmod +x /usr/src/utils/AllStar-build/common/uricheck.sh
ln -fs /usr/src/utils/AllStar-build/common/uricheck.sh /usr/bin/uricheck
echo "Done"
sleep 1
echo "Resetting compiler flags..."
cd /usr/src/utils
git reset --hard HEAD
echo "Done"
sleep 1
echo "Updating system boot configuration..."
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi /etc/init.d
update-rc.d dahdi defaults
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
echo "The update is complete..."
echo "You can run this tool at any time by typing 'system-update' at a root prompt."
sleep 1
echo "Rebooting your node to apply the new boot configuration."
# restore bashrc
mv /root/.bashrc.orig /root/.bashrc
sync
sudo reboot

