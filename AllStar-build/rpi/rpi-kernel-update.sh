#!/bin/bash
# update the kernel and headers on the Raspberry Pi

#script start
echo "Updateing the kernel..."
sleep 1
apt-get install raspberrypi-kernel raspberry-kernel-headers -y
sleep 1
apt-get remove --purge linux-image-rpi2-rpfv linux-headers-rpi2-rpfv
sleep 1
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
sleep 1
echo "Done"
echo "You can now run system-update."
