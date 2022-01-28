#!/bin/bash
# wifi-setup.sh: Setup Wi-fi connections
#   Copyright (C) 2016-2022  Jeremy Lincicome W0JRL
#   <https://jlappliedtechnologies.com>  <admin@jlappliedtechnologies.com>
#
#   Wi-fi card selection and loop scan contributed by Skyler Fennell W0SKY
#   <https://www.youtube.com/channel/UCyQzGw5fvymeCcafN-J7krQ>
#
#   This file is part of RasLink.
#   RasLink is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   RasLink is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with RasLink.  If not, see <http://www.gnu.org/licenses/>.
#
# Script Start
echo "Welcome to Wi-Fi setup."
sleep 0.5s
country=$( cat /etc/wpa_supplicant/wpa_supplicant.conf | grep "country=" | sed 's/country\=//' )
read -e -p "$( echo -e "Your country is currently set to: $country\n Do you want to change it? [Y/N]" )" changeCountry
if [[ "$changeCountry" = "y" ]] || [[ "$changeCountry" = "Y" ]]; then
  echo "Your two letter country code must be uppercase."
  echo "Example: US"
  read -e -p "Please enter your country code: " countryCode
  sed -i "s/${country}/${countryCode}/" /etc/wpa_supplicant/wpa_supplicant.conf
  echo "Country updated"
else
  echo "Country not changed"
fi
sleep 0.5s
echo "Please enter the Wi-Fi card name from the list below:"
ip a | grep 'wlan' | sed 's/^..//;s/:.*//;s/inet.*//'
read -e -p "[wlan0] : " wificard
cardnum=$(ip a | grep "${wificard}" | sed 's/:.*//;s/inet.*//')
if [ "${wificard}" = "" ]; then
  wificard=wlan0
fi
sleep 0.5s
scan=1
while [ "${scan}" == "1" ]; do
  echo "Scanning for networks..."
  echo "___________________________________"
  sleep 0.5s
  ifconfig ${wificard} up &>/dev/null
  sleep 1s
  iwlist ${wificard} scan | grep 'ESSID' | sed 's/ESSID://g;s/"//g;s/^ *//;s/ *$//'
  sleep 0.5s
  echo "Scan complete"
  sleep 0.5s
  read -e -p "Do you want to scan again? [y/N]" YN
  if [[ "${YN}" = "y" ]] || [[ "${YN}" = "Y}" ]];  then
    scan=1
  else
    scan=0
  fi
done
sleep 0.5s
read -e -p "Please enter the name of the network you want to connect to: " networkName
echo "You entered: ${networkName}"
read -e -p "Please enter the password for the network: " networkPass
echo "Password accepted"
sleep 0.5s
echo "Setting up connection..."
echo "network={
 ssid=\"$networkName\"
 psk=\"$networkPass\"
}" >> /etc/wpa_supplicant/wpa_supplicant.conf
echo "Done"
sleep 0.5s
echo "Activating connection; Please wait..."
wpa_cli -i ${wificard} reconfigure &>/dev/null
waitcount="0"
while [[ "${waitcount}" != "20" ]]; do
  if [[ "${waitcount}"  == "20" ]]; then
    continue
  else
    echo -n '.--.-..- '
    sleep 1s
    waitcount=$((waitcount+1))
  fi
done
if (( "$(ip a | grep "${wificard}" | grep -ic "inet")" >= "1" )); then
    echo ""; echo "***Connection Active***"
    sleep 0.5s
    echo "displaying connection information"
    sleep 0.5s
    (ip a | grep "${wificard}" | sed 's/^..//';ip a | grep "${cardnum}:" | grep 'inet6')
else
    echo ""; echo "***Connection Failed***" >&2
    echo "See <https://jlappliedtechnologies.com/raslink/> if you need assistance." >&2
    exit 1
fi
sleep 0.5s
echo "If you want to setup another connection, run wifi-setup again."
echo "To remove a network, edit /etc/wpa_supplicant/wpa_supplicant.conf."
echo "See <https://jlappliedtechnologies.com/raslink/> if you need assistance." 
echo "Exiting"
exit 0
