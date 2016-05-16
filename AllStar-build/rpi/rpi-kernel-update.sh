#!/bin/bash
# update the kernel and headers on the Raspberry Pi

#script start
echo "Updating the kernel..."
sleep 1
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
apt-get update
echo "Rebooting to finish kernel update."
echo "When the system reboots, you need to login to finish the update."
sleep 1
cd /root
mv .bashrc .bashrc.orig
cat .bashrc.orig > .bashrc
echo "echo \"Finishing kernel update...\"" >> .bashrc
echo "apt-get install raspberrypi-kernel-headers -y" >> .bashrc
echo "apt-get remove --purge linux-image-rpi2-rpfv linux-headers-rpi2-rpfv -y" >> .bashrc
echo "rm -rf /usr/src/linux-headers-3.18.0-trunk-common" >> .bashrc
echo "rm -rf /usr/src/linux-headers-3.18.0-trunk-rpi2" >> .bashrc
echo "rm -rf /usr/src/linux-kbuild-3.18" >> .bashrc
echo "echo \"Done\"" >> .bashrc
echo "echo \"You can now run system-update.\"" .. .bashrc
echo "mv /root/.bashrc.orig /root/.bashrc" >> .bashrc
sudo reboot

