#!/bin/bash
# nodenames.sh - Enable/disable nodenames
#    Copyright (C) 2021  Jeremy Lincicome (W0JRL)
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
[on]    Enable nodenames
[off]   Disable nodenames
[status]    Show status of nodenames
[help]  Show this help message
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
