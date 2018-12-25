#!/bin/bash
# end.sh - Disconnect links from AllStar
#    Copyright (C) 2019  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Script Start
/usr/sbin/asterisk -rx "rpt localnodes" > /tmp/localnodes
remove()
{
    rm /tmp/localnodes &>/dev/null
}
help()
{
    cat << _HELP_
Usage:
End can be used in two ways.
If you type only the node to control,
End assumes that you want to drop all connected nodes from that node.
The node to control must be on your AllStar server.
example:
end 12345
If you type the node to control plus another node number,
End will drop that node.
example:
end 12345 67890
You must enter at least the node  to control.
_HELP_
}
if [[ "$1" = "" ]] || [[ "$1" = "0" ]]; then
  remove
  help
  exit 2
  elif [[ "$1" = "-h" ]] || [[ "$1" = "--help" ]] || [[ "$1" = "help" ]]; then
    remove
    help 
    exit 1
  elif [ "$1" != "$( grep $1 /tmp/localnodes )" ]; then
    echo "You cannot control $1.
$1 doesn't exist on this server."
    remove
    exit 2
  elif [[ "$2" = "0" ]] || [[ "$2" = "$1" ]]; then
    echo "You cannot disconnect a node from itself.
Try again."
    remove
    exit 2
  elif [ "$2" != "" ]; then
    /usr/sbin/asterisk -rx "rpt cmd $1 ilink 1 $2"
    echo "$2 disconnected from $1"
else
  /usr/sbin/asterisk -rx "rpt cmd $1 ilink 6 $1"
  echo "All links disconnected from $1"
fi
remove
exit 0
