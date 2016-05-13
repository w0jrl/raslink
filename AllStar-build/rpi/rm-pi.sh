#!/bin/bash
# removes the pi user if needed

# script start
echo "Looking for the pi user..."
user=$( grep -ic "pi" /etc/passwd )
if [ $user -eq 1 ]
then
  echo "Removing the pi user; Not needed for AllStar."
  deluser -remove-home pi
  echo "Done"
else
  echo "The pi user doesn't exist; Skipping."
fi
