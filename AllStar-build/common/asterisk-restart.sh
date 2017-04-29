#!/bin/bash
# asterisk-restart.sh - Used to restart Asterisk from a radio

# Script Start
sudo systemctl restart asterisk &>/dev/null
exit 0
