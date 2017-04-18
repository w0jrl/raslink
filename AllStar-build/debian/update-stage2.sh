#!/bin/bash
# Used to update the system
# Stage Two

# Script Start
echo "Running update, stage two."
echo "This will take a while."
service asterisk stop &>/dev/null
echo "You cannot use your node during this process."
echo "It has been disabled."
sleep 1
# restore bashrc
mv /root/.bashrc.orig /root/.bashrc
# make sure version runs at login
if [ "$(grep -ic "/usr/bin/version" /root/.bashrc)" == "1" ]; then
  sed -i '/\/usr\/bin\/version/d' /root/.bashrc
fi
if [ "$(grep -ic "/usr/bin/version" /root/.profile)" == "0" ]; then
  echo "/usr/bin/version" >> /root/.profile
fi
chmod +x /usr/src/utils/AllStar-build/debian/chk-packages.sh
/usr/src/utils/AllStar-build/debian/chk-packages.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-dahdi.sh
/usr/src/utils/AllStar-build/common/update-dahdi.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-libpri.sh
/usr/src/utils/AllStar-build/common/update-libpri.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-asterisk.sh
/usr/src/utils/AllStar-build/common/update-asterisk.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-uridiag.sh
/usr/src/utils/AllStar-build/common/update-uridiag.sh
sleep 0.5
# make sure configuration files and scripts are loaded
echo "Updating start up scripts..."
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
chmod +x /usr/src/utils/AllStar-build/debian/make-links.sh
/usr/src/utils/AllStar-build/debian/make-links.sh
cp -a /usr/src/utils/astsrc/sounds/* /var/lib/asterisk/sounds
gsmcount=`ls -1 /var/lib/asterisk/sounds/rpt/*.gsm 2>/dev/null | wc -l`
if [ "$gsmcount" != "0" ]; then
  rm -f /var/lib/asterisk/sounds/rpt/*.gsm
fi
echo "Done"
sleep 0.5
echo "Cleaning up object files..."
cd /usr/src/utils
(git clean -f;git checkout -f)
echo "Done"
sleep 0.5
echo "Updating system boot configuration..."
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
systemctl daemon-reload
systemctl enable asterisk.timer
systemctl enable updatenodelist.service
if [ "$(grep -ic "snd_bcm2835" /etc/modules)" == "1" ]; then
  sed -i '/snd_bcm2835/d' /etc/modules
fi
if [ "$(grep -ic "snd_pcm_oss" /etc/modules)" > "1" ]; then
  sed -i '/snd_pcm_oss/d' /etc/modules
  echo "snd_pcm_oss" >> /etc/modules
fi
echo "Done"
sleep 0.5
echo "The update is complete."
echo "You can run this tool at any time by typing 'system-update' at a root prompt."
echo "Re-enabling your node..."
sync
sleep 1
(service asterisk start;service updatenodelist start) &>/dev/null
echo "Done"
date > /root/.lastupdate
exit 0
