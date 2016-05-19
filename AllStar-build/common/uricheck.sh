#!/bin/bash
# Used to check a USB Radio Interface (URI) for problems
echo "Welcome to uricheck."
echo "if you are not using a USB Radio interface (URI) or do not have your loopback circuit connected, press CTRL+c now to abort."
sleep 5
echo "Your node cannot be used durring this process."
echo "Disabling your node..."
(service asterisk stop;service dahdi stop)
sleep 1
echo "Done"
echo "Please follow the on-screen instructions".
echo "When done, press q then enter to quit."
sleep 2
/usr/bin/uridiag
sleep 2
echo "Re-enabling your node..."
(service dahdi start;service asterisk start)
sleep 1
echo "Done"
echo "Please disconnect your loopback circuit, and re-connect your radio."
echo "Exiting"
