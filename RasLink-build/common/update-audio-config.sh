#!/bin/bash
# update-audio-config.sh: Update PulseAudio configuration if required
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
cd /usr/src/utils/RasLink-build/common/audio-config
echo "Updating Pulseaudio configuration files if required..."
# Make sure the pulse directory exists
mkdir -p /etc/pulse
# Add the PulseAudio configuration to the pulse directory
cp ./pulse/* /etc/pulse
# Add asound.conf to /etc
    # Needed for the ALSA hardware driver to relay instructions
    # to and from PulseAudio
cp ./asound.conf /etc
# Make sure the root user has access to PulseAudio
$(which adduser) root pulse-access &>/dev/null
$(which adduser) root audio &>/dev/null
# Override the default osspd.service
    # Its configuration won't work for us.
cp ./osspd.service /etc/systemd/system
# Add pulseaudio.service
cp ./pulseaudio.service /etc/systemd/system
$(which systemctl) daemon-reload &>/dev/null
echo "Done"
exit 0
