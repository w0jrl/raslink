#!/bin/bash
# system-update-messages.sh: Show important announcements
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
# Script Start
if [ ! -f /root/.nodelistsupport ]; then
    cat << _MSG_
***PLEASE READ***
This server has been updated to support nodelists from both PTTLink and AllStarLink.
PTTLink remains the default network for RasLink.
If any of your nodes are registered with AllStarLink, and you would like to use the AllStarLink node list instead of the PttLink version, you can enable the AllStarLink node list for each node
by searching for
"extnodes ="
in </etc/asterisk/rpt.conf>.
_MSG_
    echo "PRESS ENTER TO CONTINUE"
    read
    touch /root/.nodelistsupport
fi
exit 0
