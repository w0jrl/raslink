## RL-21.12
>Thu Dec 2 12:36:49 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(RasLink-build/common/timesync.service): Restart after 5 seconds if the service fails
>Thu Dec 2 12:26:59 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

If the system was booted without an internet connection, the service would time out,
causing the system time to be set incorrectly.
The 2 ways to fix this were to either run 'systemctl start timesync', or reboot the system.
Systemd will restart the Timesync service after 5 seconds if it fails.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## change(RasLink-build/common/asterisk.service): Restart after 5 seconds if the service fails
>Thu Dec 2 11:51:21 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

The restart time used to be 10 seconds.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.11
>Sun Oct 31 22:11:01 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Security updates from Debian/Raspbian

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.09
>Wed Sep 1 09:04:34 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Security updates from Debian/Raspbian

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.08
>Sun Aug 1 11:58:06 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## change(src/asterisk/apps/app_rpt.c)(RasLink-build/common/patch-asterisk.sh): Update revision format to use "YYYYMMDD"
>Sun Aug 1 11:53:22 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(RasLink-build/common/asterisk-config-check.sh): Keep the extnodes key from growing in rpt.conf
>Thu Jul 15 15:28:00 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Every time 'system-update' called this script,  the extnodes key was expanded by adding "ext" to the front of the line.
If you wanted to use the AllStarLink node list instead of the PTTLink version, you couldn't.
The line expansion caused the extnodes = key value pair to not match.
This forced your node to use the PTTLink node list.
The extnodes key has been replaced, and will work properly.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(RasLink-build/common/update-dahdi.sh): Make sure the tarball is removed if Dahdi install fails
>Fri Jul 2 15:13:35 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.07
>Wed Jun 30 09:47:15 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(LICENSE_RasLink.md): Use markdown syntax
>Mon Jun 21 02:20:58 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Rename LICENSE_RasLink to LICENSE_RasLink.md
*   Update file path in version.sh

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## style(rpi/zram.service): Update formatting
>Mon Jun 21 01:18:52 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.06
>Mon May 31 15:26:16 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## style: Fix formatting of service units
>Fri May 28 21:03:15 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(timesync.service): Make sure the service runs before pulseaudio.service
>Fri May 28 20:34:54 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

The Asterisk logs sometimes had inaccurate timestamps, because the clock wasn't always set correctly before Asterisk started.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(master-to-main.sh): Display script status
>Mon May 24 02:16:05 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

There was no information displayed when the script ran,
causing 'system-update' to look like it froze.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## style(uricheck.sh): Update spacing to match other scripts
>Mon May 24 01:43:07 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore: Replace changelog-generator with post-commit
>Mon May 24 01:03:27 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore(zram*): Convert from two scripts to one
>Tue May 11 00:03:55 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Remove zram.start
*   Remove zram.stop
*   Add zram
*   Modify zram.service to work with the new script

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.05
>Fri Apr 30 18:32:49 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.04.2
>Thu Apr 8 22:31:22 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(asterisk-menuselect): Update SOUNDS_URL to  <http://downloads.asterisk.org/pub/telephony/sounds/releases>
>Thu Apr 8 22:20:47 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

The old URL <http://downloads.digium.com/pub/telephony/sounds/releases> times out randomly, preventing Asterisk from finishing installation.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.04.1
>Thu Apr 8 15:03:48 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(node list support): Use "extnodes =" instead of "nodes ="
>Thu Apr 8 14:59:53 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Update rpt.conf template
*   Update active rpt.conf configuration

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.04
>Thu Apr 1 07:32:06 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## feature: Support both PTTLink and AllStarLink node lists
>Thu Apr 1 06:25:52 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

[PTTLink](https://pttlink.org) remains the default network for RasLink.
If you would like to use the [AllStarLink Network](https://allstarlink.org) for IAX registration and node status posting, your files can easily be reconfigured.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(asterisk-config-check.sh): Don't overwrite user customizations
>Wed Mar 31 22:00:47 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Ignore registration lines in </etc/asterisk/iax.conf>
*   Ignore "statpost_url" (if defined) in </etc/asterisk/rpt.conf>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## docs(RasLink-build*): Update comment formatting
>Sun Mar 28 13:47:25 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(rc.updatenodelist): Replace which with command
>Sun Mar 28 13:46:56 2021 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## feature(chan_usrp.c): Add DTMF decoding
>Sun Mar 28 13:44:40 2021 -0600

>Author: Danny Lloyd <kb4mdd@arrl.net>

*   Clean up compiler warnings

Contributed-by: Danny Lloyd <kb4mdd@arrl.net>
Edited-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## style(timesync.hourly): Remove unneeded blank line
>Sat Mar 13 06:43:35 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.03.4
>Fri Mar 12 04:13:49 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore(timesync*): Move the date command to its own script
>Fri Mar 12 04:10:56 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

When the date command was running once an hour, it was causing Asterisk to restart. The date command will now only run once when the timesync service is started.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.03.3
>Thu Mar 11 17:58:59 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(timesync.hourly): Make sure we don't crash Asterisk while syncing to the time server
>Thu Mar 11 17:56:14 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.03.2
>Fri Mar 5 12:44:06 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(timesync.hourly): Make sure the date is set correctly
>Fri Mar 5 11:56:48 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.03.1
>Mon Mar 1 14:08:21 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(system-update*): Define the Tmux variables correctly
>Mon Mar 1 14:07:03 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.03
>Mon Mar 1 12:10:52 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## docs: Update license comments
>Thu Feb 25 16:52:38 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore: Replace Screen with Tmux
>Fri Feb 12 20:27:51 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.02.4
>Tue Feb 9 15:12:02 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(fail2ban*): Make sure the IPv6 interface is monitored
>Tue Feb 9 15:07:54 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.02.3
>Sat Feb 6 19:03:11 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(updatenodelist.service): Update description and configuration
>Sat Feb 6 18:57:23 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Change AllStar to RasLink
*   Change BindsTo to PartOf

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.02.2
>Sun Jan 31 00:57:05 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(system-update-stage1.sh): Update the source files" prompt
>Sun Jan 31 00:52:40 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   Remove "All Star"
*   Add "RasLink"

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.02.1
>Sun Jan 31 00:21:57 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore(version.sh): Prune deleted branches while fetching update information
>Sun Jan 31 00:18:15 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.02
>Sat Jan 30 22:49:38 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore(zram*): Remove block devices before system shutdown
>Sat Jan 30 21:02:35 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(zram.sh): Use Perl instead of bc to calculate the block device size
>Sat Jan 23 21:50:05 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

The Perl code is easier to read, and is shorter.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore(zram.sh): Remove sleep command
>Sat Jan 23 21:32:34 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Not needed.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore(zram.sh): Use zstd conpression algorithm if available
>Sat Jan 23 17:40:15 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

The zstd compression algorithm gives a much higher compression ratio than the default algorithm configured in the kernel on The Raspberry Pi.
If the zstd algorithm is available, it will be used.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore: Rename the master branch to main
>Thu Jan 14 08:54:04 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

To comply with software development inclusiveness best practices, the "master" branch has been renamed to "main."

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(app_rpt.c): Update statpost function to use forked wget
>Wed Jan 13 14:33:56 2021 -0700

>Author: Ben Cranston <thecranston@gmail.com>

This reverts statpost to using fork-n-exec of wget but retains the logic for individual node configurations.

Contributed-by: Ben Cranston <thecranston@gmail.com>
Edited-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore: Rename astsrc to src
>Thu Jan 7 05:14:51 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore: Add soft link  to point to RasLink-build
>Thu Jan 7 05:14:39 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

'system-update' stage 1 is not aware of changes to the RasLink repository. When it sets up for stage-2,
it expects to find the AllStar-build directory.
We don't want to break 'system-update', so a soft link is needed.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## chore: Rename AllStar-build to RasLink-build
>Thu Jan 7 05:14:39 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(README.md): Update text and links
>Thu Jan 7 05:14:14 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

*   replace the repository description<br>
    The RasLink repository is used to build RasLink, not AllStar (Reg).
    We should no longer use the AllStar (Reg) name, as we are not part of the<br>[AllStarLink Network](https://allstarlink.org).
*   fix the link markdown<br>
    It's easier to read in a plain text editor.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-21.01
>Fri Jan 1 01:09:28 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(*): Migrate RasLink servers to use PTTLink
>Fri Jan 1 00:30:57 2021 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Your server[s] will be migrated to PTTLink <https://pttlink.org/> for
registration and status posting.
The AllStarLink network may be added back in the future as a secondary network.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(tmpfs.sh): Set the mountpoint size to 100MB for /var/tmp
>Thu Dec 31 00:04:40 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(zram.sh): Recalculate zram swap size
>Wed Dec 30 23:57:41 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

The zram swap size will be 1.5 times the installed RAM.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(app_rpt.c): Set CURLOPT_TIMEOUT to 2 seconds
>Thu Dec 24 19:24:12 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

This is not a 100% fix for the audio dropsthat happen
when RasLink can't contact the stats server,
but it makes the dropouts more tolerable.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(app_rpt.c): Remove hangtime when sending beacon ID
>Tue Dec 8 10:45:05 2020 -0700

>Author: JimZAH <james@colderwood.com>

Beacon IDs are required in countries such as the UK.
See </etc/asterisk/rpt.conf> for more information.

Contributed-by: JimZAH <james@colderwood.com>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## license(*): Update copyright for scripts and service units
>Mon Dec 7 04:39:44 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(update-stage2.sh): Make sure Systemd doesn't hang the update
>Mon Dec 7 02:41:06 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

If Systemd needs to stop services, it may take a while for the services to completely stop.
While we wait for the services to stop, we will download the rest of the sources we need.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## docs(rpt.conf): Fix "statpost_override" documentation
>Mon Dec 7 00:54:17 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

"statpost_override" will turn on or off "statpost", regardless
if it is enabled in the globals section.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-20.12
>Mon Nov 30 16:06:48 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(release-upgrade.sh): Drop support for gcc-4.8 and gcc-4.9 in RasLink Stretch
>Mon Nov 30 15:40:26 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## bugfix(release-upgrade.sh): Fix the test condition for the prompt function
>Mon Nov 30 01:32:25 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## update(audio-configuration): Replace the OSS driver with PulseAudio
>Wed Nov 25 16:59:32 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Starting in Debian Stretch,
the OSS kernel module is no longer available.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## release(Stretch): Upgrade RasLink servers to Raspbian Stretch
>Wed Nov 25 16:48:19 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Your node[s] will be upgraded to Debian Stretch, which adds support for the
Raspberry Pi model 3B+.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(boot-config.txt): Support Raspbian Stretch
>Wed Nov 18 10:30:04 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## fix(rpt.conf): Change "net_if" to eth0
>Thu Nov 5 01:37:31 2020 -0700

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

The "ens192" interface is not valid for most users.
"eth0" is more commonly used.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## fix(app_rpt.c): Fix IP status telemetry settings
>Thu Nov 5 01:27:53 2020 -0700

>Author: Stacy Olivas <kg7qin@arrl.net>

Contributed-by: Stacy Olivas <kg7qin@arrl.net>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-20.11
>Sat Oct 31 19:44:12 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## feature(rpt.conf): Add Farnsworth Timing
>Fri Oct 23 21:14:38 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## feature(app_rpt): Allow users to adjust morse code letter spacing
>Fri Oct 23 14:55:57 2020 -0600

>Author: James Colderwood <jamesc@ip6net.net>

You can use "farnsworth = " in </etc/asterisk/rpt.conf
in the "[morse]" section.
To learn more about Farnsworth timing, visit <http://www.justlearnmorsecode.com/farnsworth.html>.
The minimum spacing value is 3, and the maximum value is 10.

Contributed-by: Jim <james@colderwood.com>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## style(changelog-generator): Change display of email addresses
>Thu Oct 22 18:37:06 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## docks: Fix rpt_flow.pdf file
>Thu Oct 22 14:33:21 2020 -0600

>Author: Stacy Olivas <kg7qin@arrl.net>

Replacement from https://wiki.allstarlink.org/w/images/f/fa/App_rpt_single_instance_audio_flow.pdf

Contributed-by: Stacy Olivas <kg7qin@arrl.net>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(app_rpt): Remove statpost_program
>Tue Oct 20 10:46:13 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

We use libcurl to send status info to the server,
so statpost_program is redundant.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(app_rpt): Cherry-pick libcurl updates from AllStarLink
>Mon Oct 19 23:21:57 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## feature(app_rpt): Add the rpt lookup function
>Mon Oct 19 19:55:57 2020 -0600

>Author: Adam Paul <adamjpaul@gmail.com>

Used to find information about a node in the nodelist
You can run "rpt lookup" from the 'asterisk' console.

Contributed-by: Adam Paul <adamjpaul@gmail.com>
edited-by: Stacy Olivas <kg7qin@arrl.net>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(app_rpt): Set "ASL_STATPOST_URL" to <http://stats.allstarlink.org/uhandler.php>
>Mon Oct 19 14:24:16 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

We shouldn't rely on stats-mco always being online.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## fix(app_rpt): GPIO command parsing
>Sun Oct 18 19:39:46 2020 -0600

>Author: Ben Cranston <thecranston@gmail.com>

Contributed-by: Ben Cranston <thecranston@gmail.com>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## feature(asterisk): Update inline_api from upstream asterisk for compatibility with modern GCC
>Sun Oct 18 14:41:19 2020 -0600

>Author: Adam Paul <adamjpaul@gmail.com>

Contributed-by: Adam Paul <adamjpaul@gmail.com>
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(changelog-generator): Use level two headings in markdown for log messages
>Thu Oct 8 06:00:45 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## refactor(changelog-generator): Fix markdown
>Thu Oct 8 05:45:27 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(version): Add the .md extension to the RasLinkCHANGELOG file name
>Thu Oct 8 05:34:51 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## update(changelog-generator): Add .md extension to the output file name
>Thu Oct 8 05:30:05 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## fix(changelog-generator): Define the "outputFile" variable properly in the confirmation echo statement
>Thu Oct 8 04:48:08 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## update(changelog-generator): Change the date format to be Month Day Year
>Thu Oct 8 04:37:33 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## fix(chk-packages): Prevent rewriting the logrotate script if the parameters are already up-to-date
>Thu Oct 8 04:10:10 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## rewrite(changelog-generator): Update output formatting
>Thu Oct 8 00:09:51 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

feature: Add author name variable

style: Use markdown
This makes the log look nice on GitLab and GitHub.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RasLinkCHANGELOG: RL-20.10
>Wed Sep 30 21:11:10 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RL-20.10
>Wed Sep 30 21:03:48 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## asterisk-usbradio: Add "txoffdelay" to usbradio configuration
>Wed Sep 30 02:30:10 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

If "txoffdelay" doesn't exist, it will be added by 'system-update'
Documentation for "txoffdelay" can be found in </etc/asterisk/usbradio.conf>

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## documentation-fix: asterisk-usbradio: Fix "rxondelay" comment
>Tue Sep 29 20:45:18 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

"rxondelay" doesn't follow the PTT.
"rxondelay" ignores the receiver, regardless if the PTT is keyed.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## asterisk-config-check: Add low-pass and high-pass filters (in enabled state) for older installs
>Tue Sep 29 20:45:18 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

If the configuration for the filters is not detected,
the filters will be added and enabled.
This doesn't affect installs where the
filters are detected, but set to "0".

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## asterisk-usbradio: Enable low-pass and high-pass filters in usbradio.conf template
>Tue Sep 29 20:44:29 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

* Will be used by new installs

Documentation is available in the file.

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## asterisk-usbradio: Update formatting in usbradio.conf template
>Tue Sep 29 13:43:17 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

* Will be used by new installs

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## changelog-generator: Remove author name from the front of log lines
>Sat Sep 19 20:19:56 2020 -0600

>Author: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>

Future commits/log-notes will be signed-off by contributors/maintainers of RasLink.
If the author name is different from the committer name, a trailer will be added to the log notes.

Unless noted in the ChangeLog messages below, all changes were committed to the RasLink repository by Jeremy Lincicome [W0JRL].

Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## asterisk-chan_usbradio: Add txoffdelay
>Sat Sep 19 20:19:56 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

"txoffdelay" ignores the reciever for a specified number of 20 millisecond
intervals after the transmitter unkeys.
This is useful when setting up a half-duplex link with an existing
repeater, where you need to ignore the repeater's hangtime.

Contributed-by: Skyler [W0SKY]
Signed-off-by: Jeremy Lincicome [W0JRL] <admin@jlappliedtechnologies.com>


## RasLinkCHANGELOG: RL-20.09
>Sun Aug 30 20:11:58 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## update-fail2ban: Check for "fail2ban-server" as the running process name
>Sun Aug 30 19:41:07 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## update-fail2ban: Make sure the default RasLink rules are loaded from'00raslink.local'
>Sun Aug 30 15:10:21 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

If Fail2ban isn't running after the new rules are installed, it will be started.


## asterisk-app_rpt: Bump module version to 20.09
>Sun Aug 30 11:39:16 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## bugfix: required-tools: Make sure "deb-src" sources are available
>Sat Aug 29 23:49:17 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

If a user tried to install RasLink on a Debian-based system (such as Ubuntu), the "deb-src" source repositories were not always loaded properly by 'required-tools'.


## fail2ban: Add 'jail.local'
>Sat Aug 29 23:43:11 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

'jail.local' is used to load user customizable rules for Fail2Ban.
Users can add jails to this file that are not already loaded by </etc/fail2ban/jail.d/00raslink.local>.
'jail.local' includes examples useful for users of Allmon.


## fail2ban: Add comment describing how '00raslink.local' is used
>Sat Aug 29 23:43:11 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## fail2ban: Disable '[mysqld-auth]' jail
>Sat Aug 29 23:43:11 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

By default, RasLink doesn't use any mysql databases.
The '[mysqld-auth]' jail is used to block attacks against the mysql server.
If a user installes Allmon, and wants to enable '[mysqld-auth]', the user should edit
</etc/fail2ban/jail.local>.


## fail2ban: Use 'banaction_allports' for the action to take when banning
>Sat Aug 29 23:43:11 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

When an attacker is banned, they should be banned on all ports,
not just the port they tried to access.
If the ssh port is changed from the default (22),
the RasLink Fail2Ban rules should not assume that the active ssh port will
always be the port defined in
</etc/services>.


## fail2ban: Rename 'jail.local' to '00raslink.local'
>Sat Aug 29 23:43:11 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

The default RasLink rules for Fail2Ban are loaded from this file.
Users should not edit this file.
Users can activate custom jails in
</etc/fail2ban/jail.local>.
'00raslink.local' is stored in and loaded from
</etc/fail2ban/jail.d>.


## asterisk-logrotate: Add 'delaycompress' to logrotate script
>Tue Aug 25 11:30:11 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

If 'delaycompress' doesn't exist, 'system-update' will add it.


## asterisk-logrotate: Fix messages and comments in install scripts
>Tue Aug 25 11:13:06 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Replace 'monthly' with 'weekly'


## chk-packages: Update the time between log rotations for Asterisk in the rotation message
>Mon Aug 10 01:04:57 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

The logs will be rotated once a week.


## chk-packages: Update existing logrotate parameters for Asterisk
>Mon Aug 10 00:39:01 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## mk-logrotate-asterisk: Don't keep rotated logs for more than two months
>Sun Aug 9 23:58:04 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## mk-logrotate-asterisk: Remove rotated logs every two weeks
>Sun Aug 9 23:43:54 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## version: Update the changelog file location
>Sun Aug 9 23:22:34 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add changelog generator
>Fri Aug 7 14:29:04 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

The changelog generator is used to create the changelog file for RasLink. Users should not run this program.
It is included for source code completeness.


## Rename RasLink_CHANGELOG to RasLinkCHANGELOG
>Fri Aug 7 14:02:44 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Wifi-setup: Fix connection detection
>Fri Aug 7 01:40:34 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Wifi-setup would sometimes report that a connection had failed, even though the connection was active.


## Version: Display the locations of the changelog and license
>Fri Aug 7 01:17:41 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## chk-packages: Fix sources list code for Debian
>Thu Aug 6 20:26:13 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## LICENSE_RasLink: Update text and formatting
>Thu Aug 6 20:10:42 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## RasLink_CHANGELOG: CHANGELOG RL-20.08
>Fri Jul 31 22:04:05 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Asterisk: Update app_rpt version to 20.08
>Fri Jul 31 21:56:56 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Security: Add Fail2Ban
>Fri Jul 31 21:54:29 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Fail2Ban is used to block brute force attackers from gaining access to RasLink.
Users have 6 (six) attempts to send the correct login information to RasLink.
If the user doesn't send the correct login information within 6 (six) attempts, the user will be banned for 2 (two) hours.
After 2 (two) hours, if repeated failed login attempts continue from the same IP address, the user/attacker will be banned for up to 1 (one) week.
To regain access to RasLink if the user gets banned, the user should fill in the 'Contact Us' form, located at:
https://jlappliedtechnologies.com/contact-us/
When filling in the form, the user should request to have their IP address unbanned.


## Required-tools: Add build tools for Fail2Ban
>Thu Jul 30 11:59:59 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Required-tools: Add preload
>Sun Jul 26 15:27:17 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Preload is used to load applications that are used the most into ram faster.


## Asterisk-modules: Use pthread instead of lpthread
>Mon Jul 20 23:05:48 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Update status posting functions
>Mon Jul 20 22:51:04 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Update wget switches in STATPOST_PROGRAM

* Adjust thread priorities

* Update url in ASL_STATPOST_URL


## RasLink_CHANGELOG: CHANGELOG RL-20.07
>Wed Jul 1 09:55:36 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Patch-asterisk: Make sure pthread.h is included in future updates to app_rpt.c
>Mon Jun 29 21:43:09 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Patch-asterisk: Include pthread.h in sources where needed
>Mon Jun 29 20:47:56 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Add tries and timeout to STATPOST_PROGRAM
>Mon Jun 29 15:06:18 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Irqbalance: Reload daily to prevent memory leaks
>Mon Jun 29 10:31:13 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## LICENSE_RasLink: Update text
>Mon Jun 29 10:14:01 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Asterisk-Makefile: Remove pthread patch
>Mon Jun 29 10:09:09 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Adjust pthreads
>Mon Jun 29 10:01:05 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Change priority values

* Include pthread.h


## App_rpt: Revert tries in STATPOST_PROGRAM
>Sun Jun 21 15:07:12 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Causes  the transmitter to hang if unable to connect to the stats server


## App_rpt: Set tries to 5 in STATPOST_PROGRAM
>Sun Jun 21 14:35:42 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Add tries to STATPOST_PROGRAM
>Sun Jun 21 13:57:01 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Usbradio: Readd RasLink patches
>Sat Jun 20 12:57:13 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Usbradio: Revert changes to CM119B code made by AllStar Link
>Sat Jun 20 12:25:49 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Caused issues withCM119A chips


## App_rpt: Update version number to 20.07
>Sat Jun 20 10:47:56 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## RasLink_CHANGELOG: CHANGELOG RL-20.06.1
>Mon Jun 1 09:34:59 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## System-update: Make sure stage 2 prompts the user on error
>Mon Jun 1 09:31:59 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## RasLink_CHANGELOG: CHANGELOG RL-20.06
>Thu May 28 19:13:01 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## System-update: Users need to acknowledge the reboot message in stage-1
>Thu May 28 18:05:21 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## System-update: Don't exit on errors
>Thu May 28 17:02:48 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Error messages will be displayed until the user presses Enter.


## System-update: Users must acknowledge the session reconnect procedure in both stages
>Thu May 28 15:29:01 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Chk-packages: Don't remove ntp-date and control scripts needed for timesync
>Thu May 28 12:59:36 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Timesync: Synchronize time from ntp server using Ntp-date
>Thu May 28 12:23:16 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* ntp-date is controlled by timesync.hourly
* timesync.hourly is started by timesync.service on boot


## Timesync: Stop using timedatectl for updates from time servers
>Thu May 28 12:23:15 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

The random time intervals between synchronization seems to cause USB IO timing issues.


## Version: Use release tags instead of commit hash to display version number
>Thu May 28 12:10:41 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Bump version to 20.06
>Thu May 28 12:06:40 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## RasLink_CHANGELOG: Remove merge commit messages
>Wed May 27 21:03:46 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## RasLink_CHANGELOG: Reformat the log so that hashes are stript
>Wed May 27 20:29:34 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update timers in scripts
>Wed May 27 16:09:17 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Some users have reported that system-update is too slow.


## Asterisk-config-check: Set linkclip to 1 in active config and rpt template
>Wed May 27 13:13:54 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Prevents the first word received by the local radio from getting cut off


## Add CHANGELOG
>Wed May 27 02:08:01 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add RasLink license
>Wed May 27 02:01:56 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Use PLAYBACK in ci.confno variable
>Tue May 26 23:08:53 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Use PLAYBACK function from RasLink master
>Tue May 26 23:08:53 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Swap playback and localplay in tlist->mode
>Tue May 26 23:08:53 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

If LOCALPLAY is used, audio files definedin custom user scripts that should play only on the local node,
end up playing on all connected nodes as well.
Not Cool!


## Required-tools: Add libcurlpp0 and libcurlpp-dev
>Sun May 24 12:30:20 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "App_rpt: Add timeout to statpost"
>Sat May 23 16:51:35 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Useless! Made no difference.
This reverts commit 8d1315eb36fe191b38b1a48fcceb9b43599fc319


## App_rpt: Add timeout to statpost
>Sat May 23 16:19:08 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Asterisk: Test for false DTMF decode"
>Sat May 23 14:43:53 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit 21cffd9f002ddf1b928e53331d2acae14e03950c.


## Asterisk: Test for false DTMF decode
>Sat May 23 14:16:49 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Usbradio: Remove custom includes
>Sat May 23 12:43:05 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update-asterisk: Remove ARM CPU check
>Sat May 23 01:05:40 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Asterisk: Remove auto generated code"
>Fri May 22 23:04:32 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Causes Asterisk to not build
Oops... Maybe some of that stuff wasn't so random after all!
This reverts commit bfdd2c6684548b65c73e9993db9773e144f4b89f.


## Asterisk: Remove auto generated code
>Fri May 22 22:36:22 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Will be rebuilt at compile time


## Remove duplicate *.md files
>Fri May 22 21:22:41 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Uridiag: Update from DMK Engineering 20200522
>Fri May 22 16:16:58 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## System-update/install: Adjust object cleanup
>Fri May 22 15:10:15 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Checkout before cleanup


## Asterisk-menuselect: Make sure radio_relax is used
>Fri May 22 14:39:53 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Needed for some simplex nodes to decode dtmf properly


## App_rpt: Update from AllStarLink repository 20200522
>Fri May 22 00:34:04 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update-asterisk: Update active configuration file checking and rpt template
>Thu May 21 23:27:57 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Move config file checking to its own script (Called from update-asterisk.sh)
* Add [globals] section to existing user config and rpt template
* Modify Status Reporting section in existing user config and rpt template


## Usbradio: Fix QUEUE_SIZE
>Wed May 20 12:24:44 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Asterisk-bootstrap: Fix find command
>Wed May 20 11:15:42 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* 'third-party' is required, even though it's not found.


## Sounds: Copy Asterisk sounds from src during build
>Wed May 20 09:55:55 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update-asterisk: Run bootstrap before configure
>Wed May 20 09:15:47 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Required-tools: Add libblkid-dev
>Wed May 20 09:04:46 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Asterisk: Update modules from AllStarLink
>Wed May 20 02:49:18 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version to RasLink-20.05
>Mon Apr 27 13:06:25 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Fix bad patch
>Sun Apr 26 21:16:48 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Telemetry: Add telemetry ducking
>Sun Apr 26 20:29:29 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Will be added to user's rpt.conf automaticly
* 'telemnomdb' should be left at 0.
* 'telemduckdb' is adjustable.
A negative value makes telemetry quieter, and a positive value  makes it louder.


## Install: Make sure timedatectl runs after RasLink is installed on new systems
>Mon Apr 20 12:44:12 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Install: Remove timesync from install scripts
>Mon Apr 20 12:38:16 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Timesync: Make sure timedatectl runs on boot
>Mon Apr 20 12:30:09 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Timesync: Make sure timesync.hourly is removed from the user's node
>Mon Apr 20 02:24:42 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Required-tools: remove the ntpdate package...The right way!
>Mon Apr 20 02:05:17 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Required-tools: Make sure ntpdate package is removed"
>Mon Apr 20 01:55:41 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Apt-get treats the dash before the N in nptdate as an invalid command.
We don't want this behavior.
This reverts commit 88f08c0934bac81771b6a209caefb5381d3b6d69.


## Required-tools: Make sure ntpdate package is removed
>Mon Apr 20 01:30:25 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Timesync: Use timedatectl to sync with NTP servers
>Mon Apr 20 01:13:28 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Removes timesync.hourly and timesync.service


## Update version to RasLink-20.04
>Fri Mar 27 14:37:35 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Template: Update comments in rpt.conf
>Fri Mar 27 14:31:02 2020 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Bump version to RasLink- 20.03
>Mon Feb 24 15:32:47 2020 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update-dahdi: Make Dahdi compile faster
>Mon Feb 24 14:53:19 2020 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

Use all available cores to build the Dahdi driver.
By default, the compiler only uses one core, which is slow.


## Update version to 20.02
>Sat Feb 1 01:45:14 2020 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## App_rpt: Set on demand telemetry timer to 15 seconds
>Fri Jan 24 12:38:34 2020 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 When a user sets telemetry to on demand (cop,35), AllStar will stop reporting
all telemetry after 15 seconds. This timer used to be set at 2 minutes.
This helps to reduce unnecessary background chatter for the user.


## Update version number to 20.01
>Tue Dec 31 14:36:12 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update copyright notices
>Tue Dec 31 14:30:14 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version number to 19.12
>Wed Nov 27 17:18:53 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## system-update: Fix startup scripts comment
>Wed Nov 27 17:11:38 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## system-update: Fix string comparisons for gsmcount and error handling
>Wed Nov 27 16:35:30 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version to 19.11
>Wed Oct 30 15:43:39 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## configure.ac: Update the test condition for sox
>Wed Oct 30 15:35:54 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * The program is no longer called soxmix.


## Revert "Debian-install: Rename raslink-pc-install"
>Thu Oct 17 15:14:32 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit 857cafcf0df9f9fdfda66a9e0cc3212bab2429ea.


## Revert "Debian-install: Fix chmod command in stage1"
>Thu Oct 17 15:13:14 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit 0dc0d75aeb624eec145f7b4098571ff04cdee192.


## Debian-install: Fix chmod command in stage1
>Thu Oct 17 13:09:32 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Debian-install: Rename raslink-pc-install
>Thu Oct 17 04:27:21 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Users must be running Debian Jessie for the script to work.


## Required-tools: Make sure libneon27-gnutls is installed
>Thu Oct 17 01:05:32 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Install: Update sounds directory in installer files
>Wed Oct 16 16:48:31 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update-asterisk: Don't assume that gcc-4.8 is installed
>Mon Oct 14 20:56:58 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Required-tools: Update the list of packages
>Mon Oct 14 20:53:16 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Asterisk: Look for sox instead of soxmix
>Mon Oct 14 20:49:52 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## motd: Make sure all motd related files are removed
>Wed Oct 9 11:29:57 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Timesync: Don't assume the system is online when it isn't
>Wed Oct 9 10:08:25 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix statements that caused the junk file to be created
>Mon Sep 30 21:41:02 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Update object cleanup routine"
>Mon Sep 30 20:18:38 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Not the cause of the junk file
This reverts commit 21013d9f6c7618a5827be57b8f940c748dabd43c.


## Update object cleanup routine
>Mon Sep 30 19:22:44 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Stop-gap mesure to clean up junk files, until the cause can be found.
* Doesn't affect the build process


## Fix broken packages if detected
>Mon Sep 30 18:32:41 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version number to 19.10
>Mon Sep 30 16:01:09 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure timesync is loaded at boot time
>Sat Sep 28 21:49:03 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Use dahdi_dummy as timing source for asterisk
>Sat Sep 28 19:26:41 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add zram to rpi installer
>Sat Sep 28 18:41:05 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Use ntpdate to sync time
>Sat Sep 28 17:57:59 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Adjust Asterisk startup timer to 60 seconds"
>Sat Sep 28 00:55:47 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit 6d560b59a8733e8948411589882980a4dfeebb94.


## Remove ntpd
>Sat Sep 28 00:52:28 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Causes timing issues with USB sound cards


## Reformat nodenames scripts
>Thu Sep 26 14:02:46 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make the version command more screen reader friendly
>Sun Sep 22 09:24:17 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove useless motd file
>Sun Sep 22 09:07:18 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Screen reader users were getting spammed on log in by a message
that sighted users couldn't see.


## Update date format used by version command
>Sun Sep 22 06:40:27 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Reformat chk-packages
>Thu Sep 19 18:12:40 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Comment blank lines in service units and scripts
>Thu Sep 19 13:46:59 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update package database if sources list is changed
>Sat Sep 14 13:05:28 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Dahdi template files
>Sat Sep 14 12:36:29 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Auto assign spans during Dahdi configuration
>Sat Sep 14 10:41:43 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Stop update-dahdi from creating junk files, and remove any junk files already on the system
>Fri Sep 13 02:57:27 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Dahdi build process
>Fri Sep 13 01:28:55 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Make sure we remove any junk files that the build process leaves behind.


## Update license comments in service units
>Fri Sep 13 01:06:12 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Adjust Asterisk startup timer to 60 seconds
>Thu Sep 12 22:35:54 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Gives time for all other services to start properly


## Increase prompt timers for system update to 5 seconds
>Thu Sep 12 22:32:51 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix prompts
>Thu Sep 12 21:32:38 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 A space is required in echo statements after
the -e expression, but before the quote.


## Update status messages, echo statements, and comments in build scripts
>Thu Sep 12 20:48:45 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix loaded variable for zram
>Thu Sep 12 18:34:41 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make update and version scripts more user-friendly
>Thu Sep 12 17:37:10 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add zram to Raspberry Pi
>Thu Sep 12 14:39:41 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 If the Pi needs to swap,
zram will be used,
instead of touching the SD card.


## Change dynamic memory split on Raspberry Pi
>Thu Sep 12 13:39:33 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version to 19.09
>Sat Aug 31 12:39:11 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update timers for node list retrieval
>Sat Aug 31 12:29:08 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update required-tools
>Wed Jul 24 21:19:34 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Skyler's callsign in comments for wifi-setup
>Wed Jul 24 18:10:27 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version to 19.08
>Mon Jul 22 11:54:21 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Stop removing libpt-dev
>Mon Jul 8 13:08:23 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Needed for Asterisk to build properly


## Add exports required for libpt to update-asterisk
>Mon Jul 8 12:49:10 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add libpt-dev to required-tools
>Mon Jul 8 12:38:13 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 *Used with gcc


## Update version number to 19.07
>Sun Jun 30 14:33:42 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make public IP address playable using autopatch
>Sun Jun 30 13:59:07 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Users can use the autopatch up command + 00 to hear their public IP
spoken by AllStar.


## Update version number to 19.06
>Sat Jun 1 09:42:30 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Set permissions for dsp device at system boot
>Thu May 23 13:30:48 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Bump version to 19.05
>Tue Apr 30 14:39:17 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update help text in nodenames control script
>Fri Apr 26 10:11:46 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update retry count in rc.updatenodelist
>Fri Apr 26 09:55:47 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Reduce Asterisk startup timer by 15 seconds
>Fri Apr 26 09:23:08 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update RasLink to 19.04
>Sun Mar 31 19:11:22 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove pciradio patch
>Wed Mar 20 13:30:55 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 No longer supported in dahdi.


## Fix directory paths
>Wed Mar 20 10:50:34 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add error checking to patch commands
>Wed Mar 20 10:30:40 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add Dahdi patches from AllStar Link
>Wed Mar 20 10:16:16 2019 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Needed for autopatch service.


## Update usbradio queue_size to 4
>Sat Mar 9 00:28:50 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update to version 19.03
>Wed Feb 27 23:27:14 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version to 19.02
>Mon Jan 28 21:47:18 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Use semicolons instead of blank lines if audio filters need to be added to usbradio configuration while running update-asterisk
>Mon Jan 28 21:42:27 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add pointers to the AllStar Wiki for most config templates
>Mon Jan 28 20:53:28 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Should make setting up a new node easier.

Remove blank lines from templates

 Looks cleaner, and follows the format of non AllStar Asterisk
configuration files.


## Remove blank line from end of rpt template
>Mon Jan 28 15:01:09 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove sudo from raslink-pc-install
>Mon Jan 28 14:50:44 2019 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Not needed.


## Remove sudo from reboot command in rpi update-stage2
>Tue Dec 25 18:10:53 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 No need, the script already runs as root.


## Fix bugs in update-stage1
>Tue Dec 25 17:34:29 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Remove sudo from the reboot command
 * Make sure that allAllStar related services are disabled before rebooting
Services such as updatenodelist and nodenames will force Asterisk to start,
even if the Asterisk service is disabled.
Although there is a check in update-stage2 that will force all AllStar
related services to stop if needed, this should be done in stage1,
before reboot.


## Update wifi-setup to include Skyler's new callsign
>Tue Dec 25 15:46:32 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version number to 19.01
>Tue Dec 25 15:38:43 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update rpt.conf template
>Tue Dec 25 15:24:45 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Add comments to explain how to set up a half-duplex RF link
to a repeater.
 * Make comments more readable.


## Update copyright in scripts and service units
>Tue Dec 25 12:21:28 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update license file from upstream
>Tue Dec 25 12:06:45 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove fixed-dahdi-patches directory
>Fri Nov 30 20:55:53 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 No longer needed.


## Clear output before running system update
>Mon Nov 19 22:03:13 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure Dahdi configuration is generated
>Mon Nov 19 21:14:11 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 If the Dahdi configuration files are not available,
the Asterisk service will not start.


## Remove Dahdi patches
>Mon Nov 19 19:59:22 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 No longer needed


## Update RasLink software version to 18.12
>Mon Nov 19 19:06:34 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version.sh
>Mon Nov 19 18:45:07 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Clear output before displaying information
* Add welcome message


## Update Asterisk components from upstream
>Mon Nov 19 18:11:38 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update 18.11
>Thu Oct 25 11:27:54 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update RasLink to version 18.10
>Mon Sep 17 15:30:06 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update RasLink to 18.09
>Fri Aug 31 20:40:09 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Filter Wi-Fi card listso only the card name is output
>Fri Aug 24 15:25:03 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 The card name is used in the wificard variable.
The extra information output was causing active connections to be treated
as failed.


## Use ip command to setup connection
>Fri Aug 24 14:26:07 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Ifconfig is deprecated.


## Stop downloading ASL-sounds
>Wed Aug 8 14:37:54 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Causes build errors on some hosts.
The required files already exist in the repository.


## Update app_rpt to version 18.08
>Mon Jul 16 14:12:56 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Check for failed download during stage 1 of system-update
>Fri Jul 13 19:09:54 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Abort update if download fails.


## Update nodenames from Gitlab
>Wed Jun 13 16:18:21 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Move RasLink to Gitlab
>Wed Jun 13 16:18:11 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update RasLink to version 18.07
>Tue Jun 12 12:50:16 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add snd_mixer_oss as required kernel module
>Sun Jun 10 00:57:16 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update libpri using apt-get,  and add alsa-oss as required package
>Sun Jun 10 00:57:05 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove Libpri directory structure and hooks
>Sun Jun 10 00:56:50 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure Perl is configured correctly on Raspberry Pi
>Wed May 30 21:44:58 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 The tmpfs script prevented Perl from updating correctly.


## Remove noexec option from tmpfs  script on Raspberry Pi
>Wed May 30 21:44:45 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Noexec prevented some packages from updating properly.


## Cleanup directory structure
>Sat May 19 19:12:21 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update statpost comment in rpt template
>Sat May 19 16:45:52 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update RasLink to version 18.06
>Sat May 19 15:39:24 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Disable statpost lines in rpt template
>Sat May 19 15:23:11 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Users who are setting up a public node will need to uncomment them.


## Uninstall Libpri
>Thu Apr 19 11:12:10 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk from AllStarLink
>Thu Apr 19 11:11:55 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove update from required-tools
>Fri Mar 30 12:57:41 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update package requirements
>Fri Mar 30 12:27:37 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert Stretch upgrade
>Fri Mar 30 02:45:45 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 As of the latest update for stretch on the Raspberry Pi,
Dahdi isn't building properly.


## Update purge command in package cleanup
>Tue Mar 27 10:59:21 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update error messages
>Tue Mar 27 05:40:51 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Sleep 5 seconds if errors detected during system update
>Tue Mar 27 02:37:21 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Increase sleep time to 5 seconds after instructions in release-upgrade
>Tue Mar 27 02:17:30 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Helpful for screen readers.


## Sleep 3 seconds after instructions in release-upgrade
>Mon Mar 26 22:48:53 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update error checking in build and update scripts
>Mon Mar 26 20:05:28 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove force switch from reboot command in release-upgrade
>Mon Mar 26 15:21:20 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 The force switch keeps ssh terminals from closing properly.


## Move sync command, and update reboot command in release-upgrade
>Mon Mar 26 15:11:04 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update release-upgrade purge command for old packages
>Mon Mar 26 00:45:55 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sleep times  for release-upgrade
>Mon Mar 26 00:17:58 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix exit status in release-upgrade
>Sun Mar 25 20:07:49 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure release-upgrade and wifi-setup exit properly
>Sun Mar 25 17:20:29 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove ptlib export from Asterisk build
>Fri Mar 23 18:07:18 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove libpt-dev
>Fri Mar 23 17:32:52 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add ssh to required-tools
>Fri Mar 23 14:51:45 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Copy AllStar configurations before building Asterisk during RasLink install
>Fri Mar 23 05:41:29 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Generate Dahdi system configuration after building
>Fri Mar 23 03:00:34 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update EchoLink template
>Fri Mar 23 00:47:14 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add Radio_relax to Asterisk menuselect
>Thu Mar 22 18:14:40 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure Asterisk doesn't start during raslink-rpi-install
>Thu Mar 22 14:17:31 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update version check in Debian and RPi install scripts
>Thu Mar 22 14:09:23 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update license comments in RPi install scripts
>Thu Mar 22 10:12:50 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Clean up trash from uninstalled packages
>Wed Mar 21 23:15:30 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Remove unused configurations for old packages"
>Wed Mar 21 21:07:25 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit b4c14f3e05a897cd5bfd4a59f890db760837aed1.


## Remove unused configurations for old packages
>Wed Mar 21 20:58:47 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add package clean up to required-tools
>Tue Mar 20 22:33:16 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove apt-listchanges  during release-upgrade
>Wed Mar 14 05:42:13 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure the Asterisk service is disabled before running stage2 of system-update
>Wed Mar 14 05:42:13 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Sleep 3 seconds after telling user they're already running Stretch
>Wed Mar 14 05:41:20 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 If a user tries to run release-upgrade manually,
screen will terminate too quickly,
and no output will be displayed.


## Remove sleep command from release-upgrade
>Wed Mar 14 05:10:50 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Not needed.


## Readd tmpfs, and set permissions
>Wed Mar 14 05:10:31 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make release-upgrade sleep after sync
>Wed Mar 14 05:10:16 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix libneon27-gnutls-dev package name  I had [ in the name. It should be -.
>Wed Mar 14 05:09:59 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update libneon27 requirement for stretch
>Wed Mar 14 05:09:45 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove tmpfs from RPi, and update libssl requirement
>Wed Mar 14 05:09:29 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure /tmp is unmounted during system-update on RPi
>Wed Mar 14 05:09:16 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Package signatures won't verify if tmp is mounted as a RAM file system.


## Fix dist-upgrade command in release-upgrade script
>Wed Mar 14 05:09:04 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Split release-upgrade update function into two sections
>Wed Mar 14 05:08:49 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Upgrade nodes to Stretch
>Tue Mar 13 01:45:08 2018 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix libpri build script
>Tue Feb 20 04:05:58 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure g726aal2 and ADPCM work in both old and new iax layouts
>Tue Feb 20 02:20:37 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## add g726aal2 to iax configuration
>Mon Feb 19 20:47:29 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Needed for compatibility with older ACID based systems.


## Re-add low pass and high pass filter configuration to usbradio
>Wed Feb 14 15:04:54 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update error checking
>Wed Feb 14 11:34:38 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update channel drivers from upstream
>Wed Feb 14 11:34:38 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update codecs from upstream
>Wed Feb 14 11:34:38 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk core from upstream
>Wed Feb 14 11:33:53 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Suppress useless messages from system-update and uricheck
>Tue Feb 13 15:10:13 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update service unit settings
>Tue Feb 13 15:00:24 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Create ram filesystems for temporary files at boot on RPi
>Tue Feb 13 14:29:35 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make raslink-pc-install-stage2 clean objects quietly
>Thu Feb 1 00:16:59 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure lsb-release is installed before running lsb checks
>Wed Jan 31 22:56:23 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update package requirements to work with Debian stretch
>Wed Jan 31 20:30:38 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Require lsb-release to get release info
>Sun Jan 21 22:01:13 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update jitter buffer in usbradio template
>Tue Jan 16 05:50:24 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Disable on board sound card on rpi
>Tue Jan 16 05:50:13 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure old modules are removed before building Asterisk,   and fix comment in Wifi-setup
>Tue Jan 16 03:05:51 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure latest version of g++ is installed for Dahdi
>Tue Jan 16 01:06:40 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure build scripts exit on error
>Tue Jan 9 19:49:03 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix usbradio member from upstream
>Tue Jan 9 01:43:43 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 I forgot to revert t_pmr_chan.


## Revert to xpmr from upstream master
>Tue Jan 9 01:08:06 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove RADIO_RELAX from Asterisk build options
>Tue Jan 9 01:07:55 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update usbradio patches
>Sun Jan 7 16:58:37 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk channel drivers from upstream
>Sat Jan 6 06:59:05 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Set each core individually to performance on rpi
>Thu Jan 4 04:14:45 2018 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Disable Asterisk timer during system-update
>Sat Dec 23 13:44:24 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure Avahi-daemon starts on boot
>Sat Dec 23 12:36:22 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Avahi-daemon makes it easier to log into a node by its hostname on a
local network.
It was disabled because I thought it was causing Asterisk to have audio issues,
due to network latency.
It turns out the issues were in the usbradio driver.


## Make DSP work better on Raspberry Pi
>Tue Dec 19 17:56:59 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix Wi-Fi connection detection
>Mon Dec 18 07:49:21 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add sleep after Asterisk .version file creation
>Mon Dec 18 07:29:25 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update copyright info
>Mon Dec 18 07:25:08 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure Fake-hwclock is installed on Raspberry Pi
>Mon Dec 11 23:12:17 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 If the Pi loses network connection,
Fake-hwclock will try to prevent the time from drifting.


## Make sure that /etc/init.d/asterisk is removed
>Mon Dec 11 22:28:47 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Systemd has taken over the functions of that script.


## Set Asterisk startup timer to 30 seconds
>Thu Nov 30 12:38:25 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Makes sure that the network stack has a chance to start properly.
If it doesn't, the Asterisk service will start it.


## Update app_rpt.c to 17.12
>Thu Nov 30 12:24:44 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk Copyright from upstream
>Fri Nov 17 12:58:09 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Prevent the version command from printing period on a new line
>Fri Nov 17 12:58:09 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Clean up code, and fix spelling errors
>Sat Oct 28 22:38:43 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix sleep command after country setup
>Tue Oct 17 18:41:22 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Fix ptlib configuration"
>Mon Oct 2 14:46:46 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit 1c467373bc52053d63b951d5cd43f566d7b4644c.


## Fix ptlib configuration
>Mon Oct 2 14:23:44 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Require g++-4.8 and gcc-4.8
>Thu Sep 28 14:27:10 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Require libpt-dev for Asterisk build
>Thu Sep 28 01:09:41 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Stop checking for GUI packages on RPi
>Mon Sep 25 06:14:16 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update queue size in usbradio driver
>Mon Sep 25 05:54:12 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

A queue size of 10 was causing audio to stutter


## Fix statement bugs in update scripts
>Fri Sep 8 18:45:52 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk core libraries
>Thu Aug 31 21:55:55 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update editline and ssl packages
>Thu Aug 31 18:41:52 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove unneeded comments from rpi-allstar-install
>Thu Aug 31 15:00:43 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

The script is for developers, so reminders to edit config files aren't needed.


## Remove "git pull" from rpi-allstar-install
>Wed Aug 30 16:17:11 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Not needed.


## Update package requirements
>Wed Aug 30 16:17:01 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make the update process a little quieter
>Sat Aug 26 13:28:13 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix "Connection Failed" detection
>Mon Aug 7 23:12:30 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Wifi-setup reported that new connections failed, even though they were really active.


## Remove low pass and high pass filters from usbradio
>Sat Aug 5 01:29:16 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 They weren't being used.


## Update channel drivers from upstream
>Fri Aug 4 16:55:32 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk build options
>Fri Aug 4 16:25:56 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update xpmr from upstream
>Fri Aug 4 16:04:27 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Accept ssids with spaces in wifi-setup
>Wed Aug 2 23:03:20 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add license information
>Mon Jul 31 13:47:28 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add irqbalance as required package
>Fri Jul 28 00:53:46 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Balance cpu interrupts to make timing better.


## Update Dahdi and Asterisk package and module requirements
>Fri Jul 28 00:36:26 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update dynamic gpu memory split on Raspberry Pi
>Sun Jul 23 12:00:33 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Re-enable res_crypto module
>Wed Jul 19 14:58:25 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 It's needed for the phone portal.


## Correct pthread comment in patch-asterisk
>Sat Jul 15 18:54:59 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Pthread is used on both the RPi and Debian,
not just the RPi.


## Make sure Asterisk uses pthread
>Sat Jul 15 18:16:27 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Increase queue size in usbradio driver
>Tue Jul 11 00:40:01 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix bug in Asterisk module loader patch
>Fri Jul 7 00:05:31 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 The syntax was wrong, so the patch wasn't
being loaded properly.
There's really no need for res_crypto.


## Add support for nodenames
>Sun Jun 25 22:06:59 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Nodenames are custom files that play whenever a node connects or disconnects.
Users can use 'nodenames on' to enable,
and 'nodenames off' to disable.
As of this commit, nodenames are enabled by default.


## Increase jbmaxsize in usbradio
>Wed Jun 21 15:36:36 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Stop loading res_crypto module
>Sun Jun 18 21:48:53 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update title header and download link in master README
>Sun Jun 18 06:16:27 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian installer to clone from "raslink" URL
>Sun Jun 18 03:06:05 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Migrate nodes to the RasLink URL for updates
>Sun Jun 18 00:50:26 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

"AllStar-raspbian" is now "raslink".


## Fix i2c package name
>Sat Jun 17 18:46:28 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Require i2c on RPi
>Sat Jun 17 18:34:44 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove avahi-daemon from startup
>Sat Jun 17 17:55:04 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove unneeded filter comments in usbradio
>Tue Jun 6 10:23:36 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

The "Audio from internet" and "Audio to internet" comments are wrong!


## Optimize Asterisk for RPi CPU if Raspbian is detected
>Mon Jun 5 01:36:53 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change display of version command to make output more readable
>Sat Jun 3 14:29:28 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update GPU menory split on RPi
>Sat Jun 3 14:29:28 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Restore .bashrc before completing stage two of Debian install
>Tue May 30 15:52:07 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

If the Debian install fails,
Stage two shouldn't try again after reboot.


## Make GPU memory split on RPI dynamic
>Sun May 28 13:35:05 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update app_rpt version to 17.05
>Sat May 27 15:10:52 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update app-rpt to version 5.27.17
>Sat May 27 14:52:19 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update app_rpt to version 0.527
>Sat May 27 13:59:19 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Set CPU governor to performance on RPI
>Mon May 1 16:05:48 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Adjust Asterisk startup timer
>Mon May 1 13:17:24 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Clean Libpri before building
>Mon May 1 02:20:26 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Clean Asterisk before building
>Mon May 1 02:11:25 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make RPi reboot after update stage2
>Sun Apr 30 23:05:03 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 CPU config file doesn't get reloaded unless Pi is rebooted


## Fix audio issues with Raspberry Pi
>Sun Apr 30 21:55:22 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add end command
>Sat Apr 29 18:19:31 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Used to disconnect links from AllStar
For help, run 'end.sh -h'.


## Update comments in scripts
>Sat Apr 29 17:00:10 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Send messages to standard error if connection fails during wifi-setup
>Tue Apr 25 04:30:46 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## reallocate gpu memory split on rpi
>Mon Apr 24 16:39:32 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Set gpu_mem to 16
Gives more memory to the Asterisk process


## Speak normal characters when EchoLink station connects
>Fri Apr 21 03:02:54 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove Limey Linux makefile
>Thu Apr 20 00:01:28 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 I'm not sure why this didn't get removed during the recent code clean up.
It's no longer needed.


## Modernize rc.updatenodelist
>Tue Apr 18 13:05:42 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Speed up execution of scripts
>Tue Apr 18 11:52:38 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Move Uridiag build process to its own script
>Tue Apr 18 11:52:38 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add dahdi_cfg to Asterisk startup
>Sat Apr 15 00:22:01 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove Dahdi startup routine
>Fri Apr 14 21:49:09 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Asterisk PBX can start Dahdi when needed


## Clean up startup routines
>Fri Apr 14 04:19:37 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix math expressions in variables
>Sun Apr 9 03:28:29 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Standardize functions in build and update scripts
>Sun Apr 9 01:43:10 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Add quotes around variables


## Update EchoLink and app_rpt from upstream
>Thu Mar 30 12:56:23 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update rpi install script
>Wed Mar 29 22:36:20 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update statpost lines in rpt template
>Wed Mar 29 20:35:39 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove unneeded .bashrc code from debian-install-stage2
>Tue Mar 28 14:57:34 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 It could've caused an annoying loop!


## Update Allstar requirements and logrotate instructions
>Tue Mar 28 14:08:08 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix comment in rpt.conf template
>Mon Mar 20 07:46:50 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure common/version.sh is executable
>Mon Mar 20 06:56:01 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix typos in version command commit
>Mon Mar 20 06:42:01 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add version command
>Mon Mar 20 06:07:54 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Used to display software version, and check
for available updates
Does not perform system update
Will run automaticly at root login


## Stop patching asterisk on the fly
>Mon Mar 20 03:21:05 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Asterisk sources will be patched before upload to repository.


## Decrease DTMF energy requirement
>Fri Mar 17 21:03:29 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix DTMF handling in dsp.c
>Fri Mar 17 19:16:02 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Stop cleaning Asterisk objects
>Fri Mar 17 16:29:12 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Cleaning the objects blew-away the Asterisk
customizations needed for usbradio
and app_rpt.c.


## Fix typo in update-stage2; both platforms
>Fri Mar 17 15:49:35 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove ability to keep Asterisk running during stage two
>Fri Mar 17 15:27:38 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Allowing Asterisk to run during stage two of system-update
causes Asterisk not to build properly.
Users will just have to
wait for the build to complete.


## Create Asterisk version number after cleaning objects
>Fri Mar 17 08:09:57 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure asterisk objects are cleaned before build
>Fri Mar 17 08:09:57 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make code style uniform in all scripts
>Fri Mar 17 08:09:57 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 The different ways of controlling file access in each program
  was driving me nuts!


## Prevent reading false DTMF digits
>Fri Mar 17 08:09:57 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix code error in usbradio template
>Mon Mar 13 05:58:55 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Keep stage 2 from running after reboot if failure detected during update
>Mon Mar 13 05:28:42 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make messages more readable during system update
>Mon Mar 13 04:29:29 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove the need for a second reboot during update
>Sun Mar 12 21:26:10 2017 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update menuselect
>Fri Mar 10 20:38:34 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sound files
>Fri Mar 10 20:38:34 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk build instructions
>Fri Mar 10 20:38:34 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Set correct timer in rc.updatenodelist
>Wed Mar 1 09:10:34 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 I changed the wrong timer in the last 2commits.


## Set correct timer in rc.updatenodelist
>Wed Mar 1 08:27:58 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 I changed the wrong timer in the last commit.


## Adjust timers in rc.updatenodelist to make sure node list gets loaded at boot
>Wed Mar 1 07:47:31 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Adjust timers in rc.updatenodelist to keep from hitting the server too hard
>Wed Mar 1 06:55:15 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure dahdi is used as timing source for asterisk
>Wed Mar 1 04:58:45 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update timers in rc.updatenodelist
>Tue Feb 28 18:53:45 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Node list will be updated more frequently.


## Update required-tools.sh and rc.updatenodelist
>Sat Feb 25 02:59:09 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert changes to updatenodelist
>Fri Feb 24 17:10:28 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix typeos
>Fri Feb 24 15:59:17 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make sure that updatenodelist.service is removed
>Fri Feb 24 15:59:17 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Clean up build scripts
>Fri Feb 24 15:59:17 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update EchoLink and uridiag from upstream
>Mon Feb 20 17:01:14 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Disable notch filter by defalt in rpt template
>Sun Feb 19 00:07:07 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Users can re-enable the filter by uncommenting the line, and adjusting to meet
their needs.


## Add adaptive jitterbuffer to usbradio
>Sat Feb 18 19:33:02 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add ability to use tail messages to rpt template
>Sat Feb 18 18:58:54 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove references to Acid and Limey Linux
>Thu Feb 16 20:14:30 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update configuration templates
>Thu Feb 16 19:38:21 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 Add notes to usbradio.conf for filters


## Update usbradio and echolink templates
>Thu Feb 16 17:04:44 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add low-pass and high-pass filters to usbradio from upstream
>Thu Feb 16 17:04:44 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix alsa audio driver configuration on RPI
>Sun Feb 12 17:24:37 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add bug fix from upstream to make uridiag set levles properly on RPI
>Sun Feb 12 16:49:50 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove Subversion dependency
>Sun Feb 5 00:13:20 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix remote connectivity issues
>Sat Feb 4 14:28:39 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add alsa[utils, bc, and dnsutils as required dependencies
>Wed Feb 1 10:11:05 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix code error made during refactor
>Fri Jan 27 17:09:09 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Refactor scripts, and fix EchoLink startup on Debian
>Fri Jan 27 17:09:09 2017 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make Asterisk use short commit hash when compiling
>Fri Dec 30 02:10:37 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 After the source switched from Subversion to Github,
  Asterisk no longer displayed a version number on the console.


## Update rpi/chk-packages.sh to fix syntax error
>Thu Dec 29 17:56:08 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fork AllStar Link Github repository
>Thu Dec 29 16:56:22 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Enhance logrotate check on RPI
>Fri Dec 9 21:04:32 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>

 People who are running the older image of Jessie for the RPI
  are not getting their logs rotated properly
  because of a setting that Raspbian
  put in /etc/fstab.
 Chk-packages checks for this setting and removes it.


## Update AllStar sources to revision 1538M
>Thu Dec 1 12:13:34 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Increase GPU memory split on Raspberry Pis
>Thu Dec 1 12:13:34 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make the node list load faster
>Sun Nov 20 14:22:18 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make AllStar load faster on boot
>Sun Nov 20 14:22:18 2016 -0700

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make the Dahdi driver restart when restarting Asterisk from the radio
>Fri Oct 28 01:46:22 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update usbradio.conf and sip.conf templates
>Fri Oct 28 01:08:48 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix issue causing repeaters to hang after disconnect
>Wed Oct 12 08:57:14 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update AllStar sources to version-1537M
>Sat Aug 27 19:28:32 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update AllStar sources to version-1537M
>Sat Aug 27 19:20:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian install script to use git clone instead of wget
>Sat Aug 27 19:20:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Set permissions for wifi-setup
>Tue Aug 16 16:53:06 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Set permissions for wifi-setup
>Tue Aug 16 16:45:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add additional dependency requirements
>Tue Aug 16 16:45:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian install script to use git clone instead of wget
>Tue Aug 16 16:45:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Add additional dependency requirements
>Tue Aug 16 16:03:56 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian install script to use git clone instead of wget
>Tue Aug 16 16:01:44 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Add additional dependency requirements
>Tue Aug 16 15:15:21 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian install script to use git clone instead of wget
>Tue Aug 16 15:14:00 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Add additional dependency requirements
>Tue Aug 16 14:57:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Handle multiple  Wi-Fi cards on Raspberry Pi
>Tue Aug 16 14:57:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Handle multiple  Wi-Fi cards on Raspberry Pi
>Tue Aug 16 14:57:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert changes to README
>Tue Aug 16 14:57:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add additional dependency requirements
>Tue Aug 16 14:57:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add additional dependency requirements
>Tue Aug 16 14:57:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian install script to use git clone instead of wget
>Tue Aug 16 14:57:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Fix README markup
>Wed Jul 27 16:05:52 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add additional dependency requirements
>Wed Jul 27 15:55:35 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update master-README.md
>Wed Jul 27 15:37:16 2016 -0600

>Author: Jeremy Lincicome <w0jrl@users.noreply.github.com>



## Add additional dependency requirements
>Wed Jul 27 15:24:49 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian install script to use git clone instead of wget
>Wed Jul 27 15:22:38 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Update Debian install script to use git clone instead of wget
>Wed Jul 27 15:04:49 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Update Debian install script to use git clone instead of wget
>Wed Jul 27 14:40:57 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Update Debian install script to use git clone instead of wget
>Wed Jul 27 10:09:00 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Update Debian install script to use git clone instead of wget
>Wed Jul 27 10:05:42 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Remove requirement for Zip and Unzip
  The script will use the Git repository directly.


## Update Debian install script to use git clone instead of wget
>Wed Jul 27 09:28:38 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update required dependencies
>Wed Jul 27 02:49:24 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Debian install script
>Wed Jul 27 02:49:24 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Make sure that zip and unzip are installed
  Debian no longer includes them in their basic tool kit.
* Fix unzip file path
  Unzip tried to unpack 'AllStar-master.zip' and failed because the file is
  actually called 'master.zip'.


## Keep Dahdi from printing useless error messages to the Asterisk log file
>Tue Jul 26 03:39:55 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update README
>Tue Jul 26 03:09:32 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Keep Dahdi from printing useless error messages to the Asterisk log file
>Tue Jul 26 03:09:32 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sources
>Tue Jul 26 03:09:32 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update README
>Tue Jul 26 02:51:59 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update README
>Tue Jul 26 00:28:05 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Keep Dahdi from printing useless error messages to the Asterisk log file
>Tue Jul 26 00:22:01 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sources
>Mon Jul 25 17:50:57 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sources
>Mon Jul 25 14:59:45 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove rpi-kernel-update script; no longer needed
>Fri Jun 17 01:48:17 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

The script was used to recovernodes when Raspbian forced a kernel update.


## Add exit codes to all scripts
>Fri Jun 17 01:48:17 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove redundant adding of Asterisk version durring stage-1 of system-update
>Thu Jun 16 21:21:44 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

	* Affects both platforms
This is handled by the update-asterisk script already.


## Ask user for node country durring wifi-setup
>Thu Jun 16 20:12:31 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Provide max power to usb ports on boot; RPI
>Wed Jun 15 19:27:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update AllStar sources to 1535M
>Wed Jun 15 19:27:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## modify .gitattributes
>Mon Jun 6 15:58:48 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix audio module loading on rpi
>Mon Jun 6 13:28:21 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

*Turn off internal Broadcom hardware audio module
		May interfere with USB sound fobs.
*Prevent snd_pcm_oss from loading more them once on boot
		When building the image, due to problems with the kernel, I ran
		the build script several times.
		This caused multiple copies of 'snd_pcm_oss' to be added to
		/etc/modules.


## Clean up the update scripts
>Wed Jun 1 11:51:11 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Clean up the update scripts
>Wed Jun 1 11:07:48 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Clean up the update scripts
>Wed Jun 1 10:58:27 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Change compiler flag reset to use 'git checkout -f' only"
>Wed Jun 1 10:58:27 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit b4ad3cc39fba8fde785a426ad977b7c824ca48be.


## Change compiler flag reset to use 'git checkout -f' only
>Sun May 29 03:59:05 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## modify scripts to add AllStar build version number
>Fri May 27 15:01:25 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sources and scripts
>Fri May 27 14:50:19 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change update scripts to use git clean
>Thu May 19 18:08:55 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk and Libpri
>Thu May 19 16:44:25 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update source version number
>Thu May 19 16:39:17 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk source
>Thu May 19 16:20:33 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Modify 'common/update-asterisk.sh' to update the svn version
>Thu May 19 14:47:34 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk and Libpri
>Thu May 19 14:37:30 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add make-links script for Debian, and modify 'debian/update-stage2.sh'
>Thu May 19 14:32:50 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk and Libpri
>Thu May 19 14:09:49 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add make-links script for Debian, and modify 'debian/update-stage2.sh'
>Thu May 19 13:51:42 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add make-links script for Debian, and modify 'debian/update-stage2.sh'
>Thu May 19 13:42:07 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Make kernel update run in 2 stages
>Mon May 16 00:13:35 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Make kernel update run in 2 stages"
>Sun May 15 23:58:51 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit d68286dfe7aa53ffd6b5c17b78a05e8275ac40b1.


## Make kernel update run in 2 stages
>Sun May 15 23:39:53 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Make kernel update run in 2 stages"
>Sun May 15 23:23:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit 81b8aab2f88cf2ce25346566fd0bdf59ddd6bc8d.


## Make kernel update run in 2 stages
>Sun May 15 23:18:35 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove old kernel headers while 'rpi-update-kernel.sh' is running
>Sun May 15 23:18:35 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove old kernel headers while 'rpi-update-kernel.sh' is running
>Sun May 15 15:56:26 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add wildcard to 'update-dahdi.sh'
>Sun May 15 06:30:38 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change 'update-dahdi.sh'
>Sun May 15 06:12:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Replace README.md after merge
>Sun May 15 04:01:16 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add fxload as required package in build scripts
>Sun May 15 03:26:48 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Create new build process
>Sun May 15 02:46:08 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change dahdi path in build scripts
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update build scripts
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove dahdi and update build scripts
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Dahdi will be downloaded during update-stage-2.


## Update dahdi again
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update dahdi
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Recreate .gitignore
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove .gitignore
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sources and scripts
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix README
>Sun May 15 00:55:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change dahdi path in build scripts
>Sun May 15 00:32:34 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update build scripts
>Sun May 15 00:04:34 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove dahdi and update build scripts
>Sat May 14 23:33:35 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Dahdi will be downloaded during update-stage-2.


## Update dahdi again
>Sat May 14 22:33:30 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update dahdi
>Sat May 14 22:11:06 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Recreate .gitignore
>Sat May 14 21:40:49 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove .gitignore
>Sat May 14 21:35:58 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sources and scripts
>Sat May 14 20:53:17 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix README
>Sat May 14 01:41:46 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update .gitattributes
>Sat May 14 01:39:41 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Replace README.md
>Sat May 14 01:39:41 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change compiler flag reset method
>Sat May 14 01:34:53 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change compiler flag reset method
>Sat May 14 01:32:44 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update uricheck to use the correct path for uridiag
>Sat May 14 01:04:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update uricheck to use the correct path for uridiag
>Sat May 14 00:52:24 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update .gitattributes
>Fri May 13 21:46:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Replace README.md
>Fri May 13 21:43:50 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update gpu_mem to 16 in boot-config.txt
>Fri May 13 21:37:19 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update build scripts and add dahdi init script
>Fri May 13 21:13:51 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update sourcecode
>Fri May 13 19:48:11 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update README for the testing branch
>Fri May 13 18:04:57 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add script to remove pi user; Not needed for AllStar
>Fri May 13 11:11:13 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove trailing whitespace after website name
>Fri May 13 08:17:46 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update README.md
>Fri May 13 08:14:00 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update boot-config.txt to support the new kernel package
>Fri May 13 07:56:16 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix rpi-kernel-update.sh
>Fri May 13 07:24:31 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Missing 'pi' in raspberrypi-kernel-headers


## Update boot configuration for RPI
>Fri May 13 07:14:45 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

* Removed kernel lines from boot-config.txt
* added script that should be run before system-update
 to update kernel headers


## Update README.md
>Fri May 13 04:24:50 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update README.md
>Fri May 13 04:21:49 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update scripts to install automake1.9
>Fri May 13 04:17:27 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update README.md
>Mon May 9 14:18:03 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update Asterisk sources
>Mon May 9 13:58:31 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix broken 'update-stage2.sh'; all platforms
>Fri May 6 21:42:29 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update 'debian/update-stage2.sh' and add compiled files
>Fri May 6 20:25:29 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update .gitignore
>Fri May 6 20:03:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update .gitignore
>Fri May 6 20:00:44 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Modify 'update-stage2.sh' to disable timers; all platforms
>Fri May 6 19:41:59 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add startup timer for dahdi
>Fri May 6 18:03:53 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update chk-packages.sh; all platforms
>Fri May 6 04:51:21 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update required packages
>Fri May 6 04:46:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update startup and build scripts to use systemd exclusively
>Fri May 6 01:35:31 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Using rc.local keeps users from being able to
start their own custom applications.


## Add updatenodelist.service and asterisk.timer
>Thu May 5 22:37:56 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Replace README for the 'testing' branch
>Tue May 3 06:55:01 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

I broke it when I did a rebase!


## Modify 'rpi/build-asterisk.sh' to change RPTENA message in app_rpt.c
>Tue May 3 06:47:37 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Modify 'rpi/build-asterisk.sh' to change RPTENA message in app_rpt.c
>Tue May 3 06:44:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Move restore bashrc lines to end of file 'update-stage2.sh'; both platforms
>Tue May 3 06:44:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Modify 'rpi/build-asterisk.sh' to change RPTENA message in app_rpt.c
>Tue May 3 06:23:03 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Move restore bashrc lines to end of file 'update-stage2.sh'; both platforms
>Tue May 3 05:41:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update app_rpt.c to say "TXENA when TX enabled
>Tue May 3 04:59:26 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Merge build scripts
>Tue May 3 04:39:37 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove build scripts for refactoring
>Tue May 3 04:34:15 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

A bad merge caused the wrong scripts to get added.


## Revert "Merge branch 'testing' of https://github.com/w0jrl/AllStar-raspbian"
>Tue May 3 03:49:31 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit e7f1770d2d7387b8a689035a7578ffee06bcd7b5, reversing
changes made to 5f4c3259e0f06be344f82367ead9e0b3eff9c369.


## Revert "Revert "Merge branch 'testing' of https://github.com/w0jrl/AllStar-raspbian""
>Tue May 3 03:31:12 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit e8152e63421136fa9a86bf4c2564abe8759e5000.


## Revert "Merge branch 'testing' of https://github.com/w0jrl/AllStar-raspbian"
>Tue May 3 03:31:11 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit e7f1770d2d7387b8a689035a7578ffee06bcd7b5, reversing
changes made to 5f4c3259e0f06be344f82367ead9e0b3eff9c369.


## Update Dahdi and build scripts
>Tue May 3 03:31:11 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change uridiag install method
>Tue May 3 03:31:11 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update interpreter line in all scripts
>Tue May 3 03:31:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix scripts and uricheck execution
>Tue May 3 03:31:10 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Update scripts to prevent printing not found messages
 * Fix uricheck soft links


## Revert "Merge branch 'testing' of https://github.com/w0jrl/AllStar-raspbian"
>Tue May 3 03:12:07 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit e7f1770d2d7387b8a689035a7578ffee06bcd7b5, reversing
changes made to 5f4c3259e0f06be344f82367ead9e0b3eff9c369.


## Update Dahdi and build scripts
>Tue May 3 02:29:36 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Change uridiag install method
>Mon May 2 22:28:34 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update interpreter line in all scripts
>Mon May 2 22:10:22 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix scripts and uricheck execution
>Mon May 2 21:17:39 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Update scripts to prevent printing not found messages
 * Fix uricheck soft links


## Update uricheck install scripts; all platforms
>Mon May 2 20:03:56 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Makes sure that uricheck installs properly on a clean build of Debian and
Raspbian.


## Set uricheck execution bit
>Mon May 2 19:39:17 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update rpi/chk-packages.sh
>Mon May 2 18:47:32 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add logrotate parameter check to AllStar-build/rpi/chkpackages.sh
>Mon May 2 18:02:14 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Revert "Remove README.md; Not needed"
>Mon May 2 17:10:23 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

This reverts commit 906fe4d0fb00af265a9dc97456a4132024bb4628.


## Remove README.md; Not needed
>Mon May 2 17:00:36 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update 'rpi/rpi-image-prep.sh'
>Mon May 2 16:17:05 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Change log rotate method


## Move 'mk-logrotate-asterisk.sh' from debian to common, and update 'debian-allstar-install.sh' to reflect the change
>Mon May 2 15:47:29 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix alignment in sip.conf template
>Sun May 1 22:22:12 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Update iax.conf template
>Sun May 1 22:14:50 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Fixed alignment, and enhanced comment in registration section.


## Fix alignment in features.conf template
>Sun May 1 21:49:37 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix alignment and spelling in extensions.conf template
>Sun May 1 21:45:35 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix alignment in echolink.conf template
>Sun May 1 18:43:26 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix alignment in dnsmgr.conf template
>Sun May 1 15:13:54 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix alignment in usbradio.conf and rpt.conf templates
>Sun May 1 14:45:37 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Add patches to install Screen
>Sun Apr 24 11:17:49 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Remove 'killall rc.updatenodelist' from update-stage2
>Wed Apr 20 22:38:22 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>



## Fix the kludgy patch to 'update-stage2.sh'
>Wed Apr 20 22:00:39 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

Making the script wait for 30 seconds would make the update take too long.
Running killall rc.local before the build process fixes the problem.


## Fix 'update-stage2.sh' on both platforms
>Wed Apr 20 21:07:14 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

I forgot to factor in the startup timer!


## Update system boot configuration
>Wed Apr 20 20:23:17 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Add timer to rc.local to keep asterisk from starting
before other system services are up fully
 * Remove 'systemctl stop dahdi' from update-stage2.sh
on both platforms
No longer needed


## Update system boot configuration
>Wed Apr 20 19:53:38 2016 -0600

>Author: w0jrl <admin@jlappliedtechnologies.com>

 * Add timer to rc.local to keep asterisk from starting
before other system services are up fully
 * Remove 'systemctl stop dahdi' from update-stage2.sh
on both platforms
No longer needed


## Update README
>Tue Apr 19 18:45:07 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>

Add information about how to switch back to master.


## Update 'AllStar-build/rpi/build-asterisk.sh'
>Tue Apr 19 16:30:45 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## add rapper script for uridiag
>Tue Apr 19 16:21:00 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Update 'rpi/update-stage2.sh'
>Tue Apr 19 11:26:01 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Update README
>Tue Apr 19 10:56:01 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Update README
>Tue Apr 19 10:15:58 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Add URI diagnostic tool
>Tue Apr 19 10:06:35 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Fix spelling errors in configuration templates
>Sat Apr 9 21:09:06 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Update 'AllStar-build/common/required-tools.sh' and 'AllStar-build/debian/update-stage2.sh'
>Sat Apr 9 07:30:49 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>

Removed some link creation code from required-tools.
Added link creation code to update-stage2.sh.


## Update 'AllStar-build/rpi/make-links.sh'
>Sat Apr 9 07:03:16 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Update 'AllStar-build/rpi/make-links.sh'
>Sat Apr 9 06:15:50 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>

Permissions were typed wrong.


## Update source files and templates
>Sat Apr 9 05:39:55 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>

Add notch filter
Add mdc1200 support
Fix configuration files that will live in /etc/asterisk; They were a mess!


## Create new directory structure
>Fri Apr 8 20:28:40 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>

This will make it easier to tell what platform each file is part of.
Scripts for Raspbian and Debian have been changed to work with the new directories.
A scripts link that points to the rpi directory was added to keep nodes from breaking during update.


## Create new directory structure
>Fri Apr 8 20:14:26 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>

This will make it easier to tell what platform each file is part of.
Scripts for Raspbian and Debian have been changed to work with the new directories.


## Update 'system/rc.allstar' and 'system/asterisk.service'
>Thu Apr 7 06:04:42 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated 'system/rc.allstar' and 'system/asterisk.service'
>Thu Apr 7 05:13:08 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated 'debian/update-stage2.sh'
>Thu Apr 7 05:09:28 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Tweaked boot configuration files
>Thu Apr 7 04:21:21 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Update-stage2 script changed
>Sat Apr 2 22:28:30 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated system-update
>Sat Apr 2 20:06:35 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Changed attributes file
>Sat Apr 2 19:37:27 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated Asterisk sources
>Sat Apr 2 19:31:42 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Removed attributes file; Not needed
>Sat Apr 2 18:28:23 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated build scripts
>Sat Apr 2 18:19:02 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated build scripts
>Sat Apr 2 17:37:18 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated build scripts
>Sat Apr 2 17:27:40 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Removed rpi-update
>Sat Apr 2 16:15:30 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated boot parameters
>Sat Apr 2 14:40:47 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Code clean up
>Sat Apr 2 08:01:45 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Modified attributes file
>Sat Apr 2 07:39:11 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Removed unneeded attributes file
>Sat Apr 2 07:35:47 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Merged changes from testing to master
>Sat Apr 2 07:04:10 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Added gitignore file
>Sat Apr 2 06:55:47 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated merge settings
>Tue Mar 29 17:16:00 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated merge settings
>Tue Mar 29 17:10:27 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Fixed broken README
>Tue Mar 29 16:56:02 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Removed unneeded file
>Tue Mar 29 16:41:31 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Tweaked usbradio template
>Tue Mar 29 16:34:31 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated merge settings
>Tue Mar 29 16:31:21 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## updated merge settings
>Tue Mar 29 16:28:40 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Tweaked usbradio template
>Mon Mar 28 09:00:33 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Fixed README file
>Sun Mar 27 11:15:24 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated AllStar sources
>Sun Mar 27 10:19:52 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Removed unneeded code from update scripts
>Sun Mar 27 09:59:22 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Fixed README so it looks nicer on Github
>Sun Mar 27 09:11:06 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated README. Rebuilt system-update scripts to use testing if the branch is turned on.
>Sun Mar 27 09:02:40 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Created README. It will be updated soon.
>Sun Mar 27 08:10:34 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated README
>Sun Mar 27 04:57:08 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated the usbradio.conf template
>Sun Mar 27 01:52:50 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## usbradio Updated the usbradio.conf template.
>Sun Mar 27 01:34:50 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Updated scripts to work with the git repository
>Sat Mar 26 19:20:20 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## added update files to Github
>Sat Mar 26 18:08:43 2016 -0600

>Author: w0jrl <w0jrl1@gmail.com>



## Initial commit
>Sat Mar 26 17:31:32 2016 -0600

>Author: Jeremy Lincicome <w0jrl@users.noreply.github.com>


