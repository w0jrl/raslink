#!/bin/bash
# rm-pi.sh: Remove the pi user if needed
#   Copyright (C) 2016-2022  Jeremy Lincicome W0JRL
#   <https://jlappliedtechnologies.com>  <admin@jlappliedtechnologies.com>
#
#   This file is part of RasLink.
#   RasLink is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   RasLink is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with RasLink.  If not, see <http://www.gnu.org/licenses/>.
#
# Script Start
echo "Looking for the pi user..."
user=$( grep -ic "pi" /etc/passwd )
if [ "$user" == "1" ]; then
  echo -e "Removing the pi user; Not needed for RasLink.\n"
  deluser -remove-home pi
else
  echo -e "The pi user doesn't exist; Skipping.\n"
fi
exit 0
