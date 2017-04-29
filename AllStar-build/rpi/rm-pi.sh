#!/bin/bash
# rm-pi.sh - Removes the pi user if needed

# Script Start
echo "Looking for the pi user..."
user=$( grep -ic "pi" /etc/passwd )
if [ "$user" == "1" ]; then
  echo "Removing the pi user; Not needed for AllStar."
  deluser -remove-home pi
  echo "Done"
else
  echo "The pi user doesn't exist; Skipping."
fi
exit 0
