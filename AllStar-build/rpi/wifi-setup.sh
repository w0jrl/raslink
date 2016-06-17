#!/bin/bash
echo "Welcome to wify setup."
sleep 1
country=$( cat /etc/wpa_supplicant/wpa_supplicant.conf | grep "country=" | sed 's/country\=//' )
read -e -p "$(echo -e "Your country is currently set to: $country\n Do you want to change it? [Y/N]" )" changeCountry
if [[ $changeCountry = y ]] || [[ $changecountry = Y ]]
then
  echo "Your two letter country code must be uppercase."
  echo "Example: US"
  read -e -p "Please enter your country code: " countryCode
  sed -i "s/${country}/${countryCode}/" /etc/wpa_supplicant/wpa_supplicant.conf
  echo "Country updated"
else
  echo "Country not changed"
fi
sleep 1
echo "Scanning for networks..."
sleep 1
iwlist wlan0 scan | grep "ESSID" | sed 's/ESSID://g;s/"//g;s/^ *//;s/ *$//'
sleep 1
echo "Scan complete"
sleep 1
read -e -p "Please enter the name of the network you want to connect to: " networkName
echo "You entered: $networkName"
read -e -p "Please enter the password for the network: " networkPass
echo "Password accepted"
sleep 1
echo "Setting up connection..."
wpa_passphrase $networkName $networkPass >> /etc/wpa_supplicant/wpa_supplicant.conf
sleep 1
echo "Done"
sleep 1
echo "Activating connection; Please wait..."
ifdown wlan0
ifup wlan0
sleep 10
echo "Connection active"
sleep 1
echo "displaying connection information"
ifconfig wlan0 | grep 'inet addr.*'
sleep 2
echo "If you want to setup another connection, run wifi-setup again."
echo "To remove a network, edit /etc/wpa_supplicant/wpa_supplicant.conf."
echo "See http://jlappliedtechnologies.com if you need assistance." 
echo "Exiting"
exit 0
