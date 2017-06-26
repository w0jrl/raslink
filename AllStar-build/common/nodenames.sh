#!/bin/bash
# nodenames.sh - Enable/disable nodenames
disable()
{
    (service nodenames stop;systemctl disable nodenames) &>/dev/null
    cd /var/lib/asterisk/sounds/rpt/nodenames
    rm -r * .* &>/dev/null
    touch /root/.nonames
    echo "Nodenames disabled"
}
enable()
{
    rm -f /root/.nonames
    (systemctl enable nodenames.service;service nodenames start) &>/dev/null
    echo "Nodenames enabled"
}
help()
{
cat << _HELP_
Usage: nodenames [OPTION]
[on]      enable nodenames
[off]       disable nodenames
[status]        show status of nodenames
[help]      show this help message
_HELP_
}
status()
{
    if [[ -e /root/.nonames ]] || [[ "$(service nodenames status | grep -ic "disabled")" = "1" ]]; then
      echo "Nodenames disabled"
    else
      echo "Nodenames enabled"
    fi
}
if [[ "$1" = "" ]]; then
  help
  exit 2
  elif [[ "$1" = "-h" ]] || [[ "$1" = "--help" ]] || [[ "$1" = "help" ]]; then
    help
    exit 1
  elif [ "$1" = "on" ]; then
    enable
    exit 0
  elif [ "$1" = "off" ]; then
    disable
    exit 0
  elif [ "$1" = "status" ]; then
    status
fi
exit 0
