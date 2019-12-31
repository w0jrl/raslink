#!/bin/bash
# make-links.sh - Create system soft links for AllStar
#    Copyright (C) 2020  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com
#
#    This program is free software: you can redistribute it and/or modify
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
chmod +x /usr/src/utils/AllStar-build/debian/update-stage1.sh
ln -fs /usr/src/utils/AllStar-build/debian/update-stage1.sh /usr/bin/system-update
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
chmod +x /usr/src/utils/AllStar-build/common/asterisk-restart.sh
ln -fs /usr/src/utils/AllStar-build/common/asterisk-restart.sh /usr/bin/asterisk-restart
chmod +x /usr/src/utils/AllStar-build/common/uricheck.sh
ln -fs /usr/src/utils/AllStar-build/common/uricheck.sh /usr/bin/uricheck
chmod +x /usr/src/utils/AllStar-build/common/version.sh
ln -fs /usr/src/utils/AllStar-build/common/version.sh /usr/bin/version
chmod +x /usr/src/utils/AllStar-build/common/end.sh
ln -fs /usr/src/utils/AllStar-build/common/end.sh /usr/bin/end
chmod +x /usr/src/utils/AllStar-build/common/nodenames.sh
ln -fs /usr/src/utils/AllStar-build/common/nodenames.sh /usr/bin/nodenames
#chmod +x /usr/src/utils/AllStar-build/common/release-upgrade.sh
#ln -fs /usr/src/utils/AllStar-build/common/release-upgrade.sh /usr/bin/release-upgrade
unlink /usr/bin/release-upgrade &>/dev/null
unlink /etc/cron.hourly/ntpdate &>/dev/null
exit 0
