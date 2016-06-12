#!/bin/bash
echo "Welcome to wify setup"
sleep 1
echo "Scanning for networks"
sleep 1
iwlist wlan0 scan | grep ESSID
sleep 1
echo "Scan complete"
sleep 1
read -e -p "Please enter the name of the network you want to connect to: " network_name
echo "You entered: $network_name"
read -e -p "Please enter the password for the network: " network_pass
echo "Password accepted"
sleep 1
echo "Setting up connection"
wpa_passphrase $network_name $network_pass >> /etc/wpa_supplicant/wpa_supplicant.conf
sleep 1
echo "Activating connection"
ifdown wlan0
ifup wlan0
sleep 10
echo "Connection active"
sleep 1
echo "displaying connection information"
ifconfig wlan0 | grep 'inet addr.*'
sleep 2
echo "If you want to setup another connection, run this tool again"
echo "Exiting"

