#!/bin/bash
# uricheck.sh - Check a USB Radio Interface (URI) for problems
#    Copyright (C) 2021  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com
#
#    This program is free software; you can redistribute it and/or modify
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
# Requires Uridiag
# Script Start
echo "Welcome to uricheck."
read -e -p "$( echo -e "Are you using a USB Radio interface (URI) with a loopback circuit connected? [Y/N]" ) " answer
if [[ "$answer" = "y" ]] || [[ "$answer" = "Y" ]]; then
  echo "Your node cannot be used durring this process."
  echo "Disabling your node..."
  service asterisk stop &>/dev/null
  sleep 0.5
  echo "Done"
  echo "Please follow the on-screen instructions".
  echo "When done, press q then enter to quit."
  sleep 1
  /usr/bin/uridiag
  sleep 0.5
  echo "Re-enabling your node..."
  service asterisk start
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
exit 1
