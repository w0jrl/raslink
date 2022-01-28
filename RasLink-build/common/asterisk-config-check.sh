#!/bin/bash
# asterisk-config-check.sh: Update Asterisk configuration files if required
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
cd /etc/asterisk/
echo "Updating Asterisk configuration files if required..."
# Fix EchoLink comment in rpt
sed -i 's/Say phonetic call sign/Say call sign/' ./rpt* | sed -i 's/say phonetic call sign/Say call sign/' ./rpt*
# Load res_crypto module
sed -i '/res_crypto.so/c\load \=> res_crypto.so ;   Cryptographic Digital Signatures                  ' ./modules*
# Load app_sendtext module
sed -i '/app_sendtext.so/c\load \=> app_sendtext.so ;   Send Text Applications                            ' ./modules*
# Add low pass and high pass filter configuration to usbradio
filters=$(grep -RiIl 'rxlpf' --include="usbradio*" | wc -w)
if [[ $filters = "0" ]]; then
    sed -i '/jblog \= no/a\
;\
rxlpf = 1    ; Receiver Audio Low Pass Filter 0,1,2\
        ; 0 - 3.0 kHz cutoff (Default) value for reduced noise and increased intelligibility. (default)\
        ; 1 - 3.3 kHz cutoff for increased high end, sibilance and brightness.\
        ; 2 - 3.5 kHz cutoff for even more high end, sibilance and brightness.\
;\
rxhpf = 1    ; Receiver Audio High Pass Filter 0,1\
        ; 0 - 300 Hz cutoff. (Default) value to reduce sub-audible signals for retransmission and in the receiver speaker. (default)\
        ; 1 - 250 Hz cutoff for additional received and retransmitted bass response.\
        ; recommend using this filter with a CTCSS tone no higher than 186.2 Hz.\
;\
txlpf = 1    ; Transmitter Audio Low Pass Filter 0,1\
        ; 0 - 3.0 kHz cutoff. (Default)\
        ; 1 - 3.3 kHz cutoff for increased high end, sibilance and brightness.\
;\
txhpf = 1    ; Transmitter Audio High Pass Filter 0,1,2\
        ; 0 - 300 Hz cutoff Reduce interference between voice and sub-audible signaling tones and codes. (default)\
        ; 1 - 250 Hz cutoff Increase bass response in transmitted audio.\
        ; 2 - 120 Hz cutoff for special applications requiring additional bass response in transmitted audio.\
        ; Not recommended due to the increased possibility of voice energy interfering with sub-audible signaling.' ./usbradio*
fi
# set jbmaxsize in usbradio
sed -i 's/jbmaxsize \= 500/jbmaxsize \= 250/' ./usbradio*
sed -i 's/jbmaxsize \= 200/jbmaxsize \= 250/' ./usbradio*
# Add g726aal2 codec to iax configuration
sed -i 's/ulaw, alaw, GSM and ADPCM should only be used,/ulaw, GSM, g726aal2 and ADPCM should only be used,/' ./iax*
sed -i 's/The ulaw and alaw codecs have/The ulaw codec has/' ./iax*
sed -i 's/followed by ADPCM, and lastly GSM,/followed by ADPCM\/g726aal2, and lastly GSM./' ./iax*
sed -i 's/alaw\/ulaw/ulaw/' ./iax*
adpcm=$(grep -RiIl 'allow = adpcm' --include="iax*" | wc -w)
g726aal2=$(grep -RiIl 'allow = g726aal2' --include="iax*" | wc -w)
if [[ $g726aal2 = "0" ]]; then
    # both layouts
    sed -i '/ULAW          best                    87 kbps/a\
; g726aal2         good                    55 kbps' ./iax*
    # new layout
    sed -i '/allow \= adpcm     ; good  55 kbps/i\
allow \= g726aal2     ; good  55 kbps' ./iax*
    # old layout
    sed -i 's/\t/ /g' ./iax*
    sed -i '/allow \= adpcm   ; good  55 kbps/i\
allow \= g726aal2   ; good  55 kbps' ./iax*
    # both layouts
    sed -i '/^allow \= ulaw$/ s:$:\nallow \= g726aal2:' ./iax*
fi
if [[ $adpcm = "0" ]]; then
    sed -i '/ULAW          best                    87 kbps/a\
; g726aal2         good                    55 kbps' ./iax*
    sed -i '/^allow \= g726aal2$/ s:$:\nallow \= adpcm:' ./iax*
fi
# Update URL for playing public IP using autopatch
sed -i 's/http:\/\/myip.vg/https:\/\/ipinfo.io\/ip/' ./extensions*
# Add telemetry ducking to rpt
telemnomdb=$(grep -RiIl 'telemnomdb' --include="rpt*" | wc -w)
telemduckdb=$(grep -RiIl 'telemduckdb' --include="rpt*" | wc -w)
if [[ $telemnomdb = "0" ]] && [[ $telemduckdb = "0" ]]; then
    sed -i '/beaconing/i\
telemnomdb = 0     ; Set the default telemetry level. (default = 0)\
telemduckdb = -9     ; Reduce telemetry level when ducking (default = -9)\
;' ./rpt*
fi
# Add globals section to rpt
globals=$(grep -RiIl 'globals' --include="rpt*" | wc -w)
if [[ $globals = "0" ]]; then
    sed -i '/Your\ Repeater/a\
;\
[globals]\
;\
;conslock = 0     ; 0=off \/ 1=limited\/ 2=locked - Locks rpt global set - Use on console\
;maxlinks = 128     ; Can be 32 to 256\
notchfilter = 1     ; 0=off\/ 1=on\
mdcencode = 1     ; 0=off\/ 1=on\
mdcdecode = 1     ; 0=off\/ 1=on\
;localchannels = 0     ; 0=off\/ 1=on\
;fakeserial = 0     ; 0=off\/ 1=on\
;noremotemdc = 0     ; 0=off\/ 1=on\
;nocdrpost = 0     ; 0=off\/ 1=on\
;mdcsay = 0     ; 0=off\/ 1=on\
linkclip = 1     ; 0=off\/ 1=on\
;zoption = 0     ; 0=off\/ 1=on\
;alttune = 0     ; 0=off\/ 1=on\
;linkdtmf = 0     ; 0=off\/ 1=on\
;setic706ctcss = 0     ; 0=off\/ 1=on\
;dtmftimeout = 3     ; Change dtmf timeout interval (3- )\
statpost = 1     ; 0 = off \/ 1= AllStar \/ 2= custom (set statpost_url line)\
;statpost_url =     ; Set to custom statpost_url if statpost=2 (above)\
net_if = eth0     ; Set to name of ethernet interface for sayip to use' ./rpt*
fi
# Remove statpost comment from rpt
sed -i -e "/If you're setting up a public node/,+1d" ./rpt*
# Update statpost program in rpt
stpost=$(grep -RiIl 'statpost_custom' --include="rpt*" | wc -w)
if [[ $stpost = "0" ]]; then
    sed -i -e "/\*\*\* Status Reporting \*\*\*/{n;N;N;N;d}" ./rpt*
    sed -i '/\*\*\*\ Status\ Reporting\ \*\*\*/a\
;\
; Set the following to override the global statpost behavior.\
     ; 0=off; 1=on\
;statpost_override = 1\
     ; 0 = Information about this node will not be sent to the configured stats server.\
     ; 1 = Information about this node will be sent to the configured stats server.\
;\
; Set to 1 to use custom statpost url for this node only.\
;statpost_custom = 0     ; If left blank\, it will default to using the global statpost.\
;statpost_url = \<url to custom handler\>     ; Set to a valid URL if overriding.\
     ; Example\: http\:\/\/stats.pttlink.org\/uhandler.php' ./rpt*
    sed -i -e "/For ACID and Debian/{N;N;N;d}" ./rpt*
    sed -i -e "/For Limey Linux/{N;N;N;d}" ./rpt*
fi
# Fix "rxondelay" comment in usbradio
sed -i '/rxondelay/,/!/ {
    s/following the/regardless if the/
    s/release of PTT/PTT is keyed/
    }' ./usbradio*
# Add txoffdelay to usbradio
txoffdelay=$(grep -RiIl 'txoffdelay' --include="usbradio*" | wc -w)
if [[ $txoffdelay = "0" ]]; then
    sed -i -e '/interfering\ with\ sub-audible\ signaling/a\
;\
;txoffdelay = 0    ; Ignore the reciever for a specified number of 20 millisecond\
        ; intervals after the transmitter unkeys.\
        ; This is useful when setting up a half-duplex link with an existing\
        ; repeater\, where you need to ignore the repeater'\''s hangtime.' ./usbradio*
fi
# Add farnsworth timing to rpt
farnsworth=$(grep -RiIl 'farnsworth' --include="rpt*" | wc -w)
if [[ $farnsworth = "0" ]]; then
    sed -i -e '/idamplitude/a\
farnsworth = 3     ; Adjust morse code letter spacing\
     ; Values are from 3 to 10\
     ; 3 = the least amount of space between letters\,\
     ; and 10 = the most.\
     ; For more information about Farnsworth Timing\, visit\:\
     ; http://www.justlearnmorsecode.com/farnsworth.html' ./rpt*
fi
# Set "net_if" to eth0 in rpt
sed -i 's/ens192/eth0/' /etc/asterisk/rpt*
# Fix statpost_override docs in rpt
statpostdoc=$(grep -RiIl 'when globals are off' --include="rpt*" | wc -w)
if [[ $statpostdoc != "0" ]]; then
    sed -i 's/when globals are off//g; /statpost_override/a\     \; 0 \= Information about this node will not be sent to the configured stats server\.\n     \; 1 \= Information about this node will be sent to the configured stats server\.' ./rpt*
fi
# Update config files to use pttlink.org
aslurl=$(grep -RiIl 'allstarlink.org' --include="*.conf" | wc -w)
if [[ $aslurl != "0" ]]; then
    find . -type f -exec sed -i -r '/statpost_url|register/!s/allstarlink\.org/pttlink.org/g' {} +
fi
# Support both AllStarLink and PTTLink node lists
aslnodes=$(grep -RiIl 'rpt_allstarlink' --include="rpt*" | wc -w)
if [[ $aslnodes = "0" ]]; then
    find . -type f -exec sed -i -e '/controlstates \=/a\
;\
;extnodes = /var/lib/asterisk/rpt_allstarlink\
     ; Uncomment this to use the AllStarLink node list for this node.' {} +
fi
sed -i 's/nodes \=/extnodes =/; s/extext.* \=/extnodes =/' ./rpt*
echo -e "Done.\n"
exit 0
