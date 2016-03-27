#! /bin/sh
# Rotate logs for Debian

#Script Start.
echo "/var/log/asterisk/messages /var/log/asterisk/*log {" >>/etc/logrotate.d/asterisk
echo "   monthly" >>/etc/logrotate.d/asterisk
echo "   rotate 12" >>/etc/logrotate.d/asterisk
echo "   missingok" >>/etc/logrotate.d/asterisk
echo "   notifempty" >>/etc/logrotate.d/asterisk
echo "   sharedscripts" >>/etc/logrotate.d/asterisk
echo "   postrotate" >>/etc/logrotate.d/asterisk
echo "   /usr/sbin/asterisk -rx 'logger reload' > /dev/null 2>&1" >>/etc/logrotate.d/asterisk
echo "   endscript" >>/etc/logrotate.d/asterisk
echo "}" >>/etc/logrotate.d/asterisk
