#! /bin/bash
# update-libpri.sh - Patch and build Libpri for All Star
#    Copyright (C) 2017  Jeremy Lincicome (W0JRL)
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
echo "Building libpri..."
cd /usr/src/utils/astsrc/libpri/
# Patch libpri for use with AllStar
patch </usr/src/utils/AllStar-build/patches/patch-libpri-makefile
# Build and install libpri
(make clean;make all;make install)
echo "Done"
exit 0
