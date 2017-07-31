#! /bin/bash
# mk-logrotate-asterisk.sh - Rotate logs for AllStar
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

# Script Start.
echo "/var/log/asterisk/messages /var/log/asterisk/*log {" >>/etc/logrotate.d/asterisk
echo "   rotate 12" >>/etc/logrotate.d/asterisk
echo "   monthly" >>/etc/logrotate.d/asterisk
echo "   compress" >> /etc/logrotate.d/asterisk
echo "   missingok" >>/etc/logrotate.d/asterisk
echo "   notifempty" >>/etc/logrotate.d/asterisk
echo "   sharedscripts" >>/etc/logrotate.d/asterisk
echo "   postrotate" >>/etc/logrotate.d/asterisk
echo "   /usr/sbin/asterisk -rx 'logger reload' > /dev/null 2>&1" >>/etc/logrotate.d/asterisk
echo "   endscript" >>/etc/logrotate.d/asterisk
echo "}" >>/etc/logrotate.d/asterisk
exit 0
