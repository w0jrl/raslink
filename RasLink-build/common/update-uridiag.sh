#!/bin/bash
# update-uridiag.sh - Build Uridiag
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
# Needed for Uricheck to work
# Script Start
status() {
    "$@"
    if [ $? -ne 0 ]; then
        echo -e "***URIDIAG FAILED TO INSTALL***\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
        exit 1
    fi
}
echo "Building and installing Uridiag..."
cd /usr/src/utils/src/uridiag/
status make
status make install
echo -e "Done\n"
exit 0
