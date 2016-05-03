#!/bin/sh -e
# adds the pi user if needed

# script start
echo "Looking for the pi user..."
user=$( grep -ic "pi" /etc/passwd )
if [ $user -eq 0 ]
then
  echo "Adding user 'pi' with password 'raspberry'"
  adduser --quiet --disabled-password -shell /bin/bash --home /home/pi --gecos "User" pi
  echo "pi:raspberry" | chpasswd
  echo "Done"
elif [ $user -eq 1 ]
then
if [ ! -d /home/pi/ ]
then
  echo "pi's home directory doesn't exist; Creating /home/pi and setting permissions."
  mkdir /home/pi
  chown -R pi. /home/pi
  echo "Done"
else
  echo "The pi user is set up correctly; Skipping."
fi
fi
