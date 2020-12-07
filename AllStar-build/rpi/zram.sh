#!/bin/bash
# zram.sh - Use zram for swap space on Raspberry Pi
#    Copyright (C) 2021  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Script Start
sleep 2
loaded=$(lsmod|grep -ic 'zram')
if (( ${loaded} >= "1" )); then
  echo "Zram is already loaded; exiting."
  exit 0
fi
#
cores=$(nproc --all)
modprobe zram num_devices=${cores}
#
swapoff -a
#
totalmem=$(free | grep -e '^Mem:' | awk '{print $2}')
mem=$(( ($totalmem / $cores)* 1024 ))
#
core=0
while [ ${core} -lt ${cores} ]; do
  echo ${mem} > /sys/block/zram${core}/disksize
  mkswap /dev/zram${core}
  swapon -p 5 /dev/zram${core}
  let core=core+1
done
