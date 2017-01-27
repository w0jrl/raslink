#!/bin/bash
echo "Welcome to Wi-Fi setup."
sleep 0.5
country=$( cat /etc/wpa_supplicant/wpa_supplicant.conf | grep "country=" | sed 's/country\=//' )
read -e -p "$(echo -e "Your country is currently set to: $country\n Do you want to change it? [Y/N]" )" changeCountry
if [[ $changeCountry = y ]] || [[ $changeCountry = Y ]]
then
  echo "Your two letter country code must be uppercase."
  echo "Example: US"
  read -e -p "Please enter your country code: " countryCode
  sed -i "s/${country}/${countryCode}/" /etc/wpa_supplicant/wpa_supplicant.conf
  echo "Country updated"
else
  echo "Country not changed"
fi
sleep 0.5
echo "Please enter the WiFi card name from the list below:"
ifconfig | grep wlan
read -e -p "[wlan0] : " wificard
if [ "$wificard" = "" ]
then
  wificard=wlan0
fi
sleep 0.5
scan=1
while [ "$scan" == "1" ]
 do
  echo "Scanning for networks..."
  echo "___________________________________"
  sleep 0.5
  iwlist $wificard scan | grep "ESSID" | sed 's/ESSID://g;s/"//g;s/^ *//;s/ *$//'
  sleep 0.5
  echo "Scan complete"
  sleep 0.5
  read -e -p "Do you want to scan again? [y/N]" YN
  if [[ "$YN" = "y" ]] || [[ "$YN" = "Y" ]]
  then
   scan=1
  else
   scan=0
     fi
done
sleep 0.5
read -e -p "Please enter the name of the network you want to connect to: " networkName
echo "You entered: $networkName"
read -e -p "Please enter the password for the network: " networkPass
echo "Password accepted"
sleep 0.5
echo "Setting up connection..."
wpa_passphrase $networkName $networkPass >> /etc/wpa_supplicant/wpa_supplicant.conf
echo "Done"
sleep 0.5
echo "Activating connection; Please wait..."
ifdown $wificard
sleep 0.5
ifup $wificard
sleep 10
if ifconfig $wificard | grep -q "addr:"
then
  echo "***Connection Active***"
  sleep 0.5
  echo "displaying connection information"
  ifconfig $wificard | grep "inet addr.*"
else
  echo "***Connection Failed***"
  exit 1
fi
sleep 0.5
echo "If you want to setup another connection, run wifi-setup again."
echo "To remove a network, edit /etc/wpa_supplicant/wpa_supplicant.conf."
echo "See https://jlappliedtechnologies.com if you need assistance." 
echo "Exiting"
exit 0
