#!/bin/bash
# make-links.sh: Create system soft links for RasLink
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
chmod +x /usr/src/utils/RasLink-build/debian/update-stage1.sh
ln -fs /usr/src/utils/RasLink-build/debian/update-stage1.sh /usr/bin/system-update
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
ln -fs /tmp/rpt_allstarlink /var/lib/asterisk/rpt_allstarlink
chmod +x /usr/src/utils/RasLink-build/common/asterisk-restart.sh
ln -fs /usr/src/utils/RasLink-build/common/asterisk-restart.sh /usr/bin/asterisk-restart
chmod +x /usr/src/utils/RasLink-build/common/uricheck.sh
ln -fs /usr/src/utils/RasLink-build/common/uricheck.sh /usr/bin/uricheck
chmod +x /usr/src/utils/RasLink-build/common/version.sh
ln -fs /usr/src/utils/RasLink-build/common/version.sh /usr/bin/version
chmod +x /usr/src/utils/RasLink-build/common/end.sh
ln -fs /usr/src/utils/RasLink-build/common/end.sh /usr/bin/end
chmod +x /usr/src/utils/RasLink-build/common/nodenames.sh
ln -fs /usr/src/utils/RasLink-build/common/nodenames.sh /usr/bin/nodenames
chmod +x /usr/src/utils/RasLink-build/common/release-upgrade.sh
ln -fs /usr/src/utils/RasLink-build/common/release-upgrade.sh /usr/bin/release-upgrade
unlink /etc/cron.hourly/ntpdate &>/dev/null
exit 0
