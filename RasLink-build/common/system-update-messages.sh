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
if [ ! -f /root/.nodeconverted ]; then
    cat << _MSG_
***PLEASE READ***
This server has been updated to use the PTTLink network <https://pttlink.org/>.
You don't need to do anything.
If you would like to use <https://allstarlink.org/> instead of PTTLink,
you should uninstall RasLink, and install their image.
No support will be given from JL Applied Technologies for doing this.
The AllStarLink network may be readded in the future as a secondary network.
_MSG_
    echo "PRESS ENTER TO CONTINUE"
    read
    touch /root/.nodeconverted
fi
exit 0
