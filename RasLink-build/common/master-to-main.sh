#!/bin/bash
# master-to-main.sh: Rename the master branch to main,
# and update the tracking URL
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
cd /usr/src/utils
oldbranchcount="$(git branch -l | grep -Ec "(^| )master( |$)")"
oldbranch="master"
newbranch="main"
echo "Checking repository branch configuration..."
if [ "${oldbranchcount}" = "1" ]; then
    echo "Updating repository branch configuration and data..."
    git branch -m ${oldbranch} ${newbranch} &>/dev/null
    git fetch origin &>/dev/null
    git branch --unset-upstream ${newbranch} &>/dev/null
    git branch --set-upstream-to=origin/${newbranch} ${newbranch} &>/dev/null
    echo -e "Done\n"
else
    echo -e "The repository branch configuration is up-to-date; Skipping.\n"
fi
exit 0
