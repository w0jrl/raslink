#!/bin/bash
# Used to check a USB Radio Interface (URI) for problems
echo "Welcome to uricheck."
read -e -p "$(echo -e "Are you using a USB Radio interface (URI) with a loopback circuit connected? [Y/N]" ) " answer
if [[ $answer = y ]] || [[ $answer = Y ]]
then
  echo "Your node cannot be used durring this process."
  echo "Disabling your node..."
  (service asterisk stop;service dahdi stop)
  sleep 0.5
  echo "Done"
  echo "Please follow the on-screen instructions".
  echo "When done, press q then enter to quit."
  sleep 2
  /usr/bin/uridiag
  sleep 0.5
  echo "Re-enabling your node..."
  (service dahdi start;service asterisk start)
  sleep 0.5
  echo "Done"
  echo "Please disconnect your loopback circuit, and re-connect your radio."
  echo "Exiting"
  exit 0
else
  echo "You need to be using a USB Radio interface (URI) with a loopback circuit connected."
  echo "For assistance, visit:"
  echo "https://jlappliedtechnologies.com/raslink"
  echo "Exiting"
fi
exit 0
