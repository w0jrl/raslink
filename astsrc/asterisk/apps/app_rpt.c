/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2002-2014, Jim Dixon, WB6NIL
 *
 * Jim Dixon, WB6NIL <jim@lambdatel.com>
 * Serious contributions by Steve RoDgers, WA6ZFT <hwstar@rodgers.sdcoxmail.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 *
 * -------------------------------------
 * Notes on app_rpt.c
 * -------------------------------------
 * By: Stacy Olivas, KG7QIN <kg7qin@arrl.net> - 20 March 2017
 * This application, the heart of the AllStar network and using asterisk as a repeater,
 * is largely undocumented code.  It uses a multi-threaded approach to fulfilling its functions
 * and can be quite a chore to follow for debugging.
 *
 * The entry point in the code , rpt_exec, is called by the main pbx call handing routine.
 * The code handles the initial setup and then passes the call/connection off to
 * the threaded routines, which do the actual work <behind the scenes> of keeping multiple
 * connections open, passing telemetry, etc.  rpt_master handles the management of the threaded
 * routines used (rpt_master_thread is the p_thread structure).
 *
 * Having gone through this code during an attempt at porting to this Asterisk 1.8, I recommend
 * that anyone who is serious about trying to understand this code, to liberally sprinkle
 * debugging statements throughout it and run it.  The program flow may surprise you.
 *
 * Note that due changes in later versions of asterisk, you cannot simply drop this module into
 * the build tree and expect it to work.  There has been some significant renaming of
 * key variables and structures between 1.4 and later versions of Asterisk.  Additionally,
 * the changes to how the pbx module passes calls off to applications has changed as well,
 * which causes app_rpt to fail without a modification of the base Asterisk code in these
 * later versions.
 * --------------------------------------
 */
/*! \file
 *
 * \brief Radio Repeater / Remote Base program
 *  version 20.04 04/01/2020
 *
 * \author Jim Dixon, WB6NIL <jim@lambdatel.com>
 *
 * \note Serious contributions by Steve RoDgers, WA6ZFT <hwstar@rodgers.sdcoxmail.com>
 * \note contributions by Steven Henke, W9SH, <w9sh@arrl.net>
 * \note contributions by Mike Zingman, N4IRR
 * \note contributions by Steve Zingman, N4IRS
 *
 * \note Allison ducking code by W9SH
 * \ported by Adam KC1KCC
 * \ported by Mike N4IRR
 * \modified by Jeremy W0JRL
 *
 * See http://www.zapatatelephony.org/app_rpt.html
 *
 *
 * Repeater / Remote Functions:
 * "Simple" Mode:  * - autopatch access, # - autopatch hangup
 * Normal mode:
 * See the function list in rpt.conf (autopatchup, autopatchdn)
 * autopatchup can optionally take comma delimited setting=value pairs:
 *
 *
 * context=string		:	Override default context with "string"
 * dialtime=ms			:	Specify the max number of milliseconds between phone number digits (1000 milliseconds = 1 second)
 * farenddisconnect=1		:	Automatically disconnect when called party hangs up
 * noct=1			:	Don't send repeater courtesy tone during autopatch calls
 * quiet=1			:	Don't send dial tone, or connect messages. Do not send patch down message when called party hangs up
 *
 *
 * Example: 123=autopatchup,dialtime=20000,noct=1,farenddisconnect=1
 *
 *  To send an asterisk (*) while dialing or talking on phone,
 *  use the autopatch acess code.
 *
 *
 * status cmds:
 *
 *  1 - Force ID (global)
 *  2 - Give Time of Day (global)
 *  3 - Give software Version (global)
 *  4 - Give GPS location info
 *  5 - Last (dtmf) user
 *  11 - Force ID (local only)
 *  12 - Give Time of Day (local only)
 *
 * cop (control operator) cmds:
 *
 *  1 - System warm boot
 *  2 - System enable
 *  3 - System disable
 *  4 - Test Tone On/Off
 *  5 - Dump System Variables on Console (debug)
 *  6 - PTT (phone mode only)
 *  7 - Time out timer enable
 *  8 - Time out timer disable
 *  9 - Autopatch enable
 *  10 - Autopatch disable
 *  11 - Link enable
 *  12 - Link disable
 *  13 - Query System State
 *  14 - Change System State
 *  15 - Scheduler Enable
 *  16 - Scheduler Disable
 *  17 - User functions (time, id, etc) enable
 *  18 - User functions (time, id, etc) disable
 *  19 - Select alternate hang timer
 *  20 - Select standard hang timer 
 *  21 - Enable Parrot Mode
 *  22 - Disable Parrot Mode
 *  23 - Birdbath (Current Parrot Cleanup/Flush)
 *  24 - Flush all telemetry
 *  25 - Query last node un-keyed
 *  26 - Query all nodes keyed/unkeyed
 *  27 - Reset DAQ minimum on a pin
 *  28 - Reset DAQ maximum on a pin
 *  30 - Recall Memory Setting in Attached Xcvr
 *  31 - Channel Selector for Parallel Programmed Xcvr
 *  32 - Touchtone pad test: command + Digit string + # to playback all digits pressed
 *  33 - Local Telemetry Output Enable
 *  34 - Local Telemetry Output Disable
 *  35 - Local Telemetry Output on Demand
 *  36 - Foreign Link Local Output Path Enable
 *  37 - Foreign Link Local Output Path Disable
 *  38 - Foreign Link Local Output Path Follows Local Telemetry
 *  39 - Foreign Link Local Output Path on Demand
 *  42 - Echolink announce node # only
 *  43 - Echolink announce node Callsign only
 *  44 - Echolink announce node # & Callsign
 *  45 - Link Activity timer enable
 *  46 - Link Activity timer disable
 *  47 - Reset "Link Config Changed" Flag 
 *  48 - Send Page Tone (Tone specs separated by parenthesis)
 *  49 - Disable incoming connections (control state noice)
 *  50 - Enable incoming connections (control state noicd)
 *  51 - Enable sleep mode
 *  52 - Disable sleep mode
 *  53 - Wake up from sleep
 *  54 - Go to sleep
 *  55 - Parrot Once if parrot mode is disabled
 *  56 - Rx CTCSS Enable
 *  57 - Rx CTCSS Disable
 *  58 - Tx CTCSS On Input only Enable
 *  59 - Tx CTCSS On Input only Disable
 *  60 - Send MDC-1200 Burst (cop,60,type,UnitID[,DestID,SubCode])
 *     Type is 'I' for PttID, 'E' for Emergency, and 'C' for Call 
 *     (SelCall or Alert), or 'SX' for STS (ststus), where X is 0-F.
 *     DestID and subcode are only specified for  the 'C' type message.
 *     UnitID is the local systems UnitID. DestID is the MDC1200 ID of
 *     the radio being called, and the subcodes are as follows: 
 *          Subcode '8205' is Voice Selective Call for Spectra ('Call')
 *          Subcode '8015' is Voice Selective Call for Maxtrac ('SC') or
 *             Astro-Saber('Call')
 *          Subcode '810D' is Call Alert (like Maxtrac 'CA')
 *  61 - Send Message to USB to control GPIO pins (cop,61,GPIO1=0[,GPIO4=1].....)
 *  62 - Send Message to USB to control GPIO pins, quietly (cop,62,GPIO1=0[,GPIO4=1].....)
 *  63 - Send pre-configred APRSTT notification (cop,63,CALL[,OVERLAYCHR])
 *  64 - Send pre-configred APRSTT notification, quietly (cop,64,CALL[,OVERLAYCHR]) 
 *  65 - Send POCSAG page (equipped channel types only)
 *
 * ilink cmds:
 *
 *  1 - Disconnect specified link
 *  2 - Connect specified link -- monitor only
 *  3 - Connect specified link -- tranceive
 *  4 - Enter command mode on specified link
 *  5 - System status
 *  6 - Disconnect all links
 *  7 - Last Node to Key Up
 *  8 - Connect specified link -- local monitor only
 *  9 - Send Text Message (9,<destnodeno or 0 (for all)>,Message Text, etc.
 *  10 - Disconnect all RANGER links (except permalinks)
 *  11 - Disconnect a previously permanently connected link
 *  12 - Permanently connect specified link -- monitor only
 *  13 - Permanently connect specified link -- tranceive
 *  15 - Full system status (all nodes)
 *  16 - Reconnect links disconnected with "disconnect all links"
 *  17 - MDC test (for diag purposes)
 *  18 - Permanently Connect specified link -- local monitor only

 *  200 thru 215 - (Send DTMF 0-9,*,#,A-D) (200=0, 201=1, 210=*, etc)
 *
 * remote cmds:
 *
 *  1 - Recall Memory MM  (*000-*099) (Gets memory from rpt.conf)
 *  2 - Set VFO MMMMM*KKK*O   (Mhz digits, Khz digits, Offset)
 *  3 - Set Rx PL Tone HHH*D*
 *  4 - Set Tx PL Tone HHH*D* (Not currently implemented with DHE RBI-1)
 *  5 - Link Status (long)
 *  6 - Set operating mode M (FM, USB, LSB, AM, etc)
 *  100 - RX PL off (Default)
 *  101 - RX PL On
 *  102 - TX PL Off (Default)
 *  103 - TX PL On
 *  104 - Low Power
 *  105 - Med Power
 *  106 - Hi Power
 *  107 - Bump Down 20 Hz
 *  108 - Bump Down 100 Hz
 *  109 - Bump Down 500 Hz
 *  110 - Bump Up 20 Hz
 *  111 - Bump Up 100 Hz
 *  112 - Bump Up 500 Hz
 *  113 - Scan Down Slow
 *  114 - Scan Down Medium
 *  115 - Scan Down Fast
 *  116 - Scan Up Slow
 *  117 - Scan Up Medium
 *  118 - Scan Up Fast
 *  119 - Transmit allowing auto-tune
 *  140 - Link Status (brief)
 *  200 thru 215 - (Send DTMF 0-9,*,#,A-D) (200=0, 201=1, 210=*, etc)
 *
 * playback cmds:
 *  specify the name of the file to be played globally (for example, 25=rpt/foo)
 *
 * localplay cmds:
 * specify the name of the file to be played locally (for example, 25=rpt/foo)
 *
 * 'duplex' modes:  (defaults to duplex=2)
 *
 * 0 - Only remote links key Tx and no main repeat audio.
 * 1 - Everything other then main Rx keys Tx, no main repeat audio.
 * 2 - Normal mode
 * 3 - Normal except no main repeat audio.
 * 4 - Normal except no main repeat audio during autopatch only
 *
 *
 * "events" subsystem:
 *
 * in the "events" section of the rpt.conf file (if any), the user may 
 * specify actions to take place when ceratin events occur. 
 *
 * It is implemented as acripting, based heavily upon expression evaluation built
 * into Asterisk. Each line of the section contains an action, a type, and variable info.
 * Each line either sets a variable, or executes an action based on a transitional state
 * of a specified (already defined) variable (such as going true, going false, no change, 
 * or getting set initially).
 *
 * The syntax for each line is as follows:
 *
 * action-spec = action|type|var-spec
 *
 * if action is 'V' (for "setting variable"), then action-spec is the variable being set.
 * if action is 'G' (for "setting global variable"), then action-spec is the global variable being set.
 * if action is 'F' (for "function"), then action-spec is a DTMF function to be executed (if result is 1).
 * if action is 'C' (for "rpt command"), then action-spec is a raw rpt command to be executed (if result is 1).
 * if action is 'S' (for "shell command"), then action-spec is a shell command to be executed (if result is 1).
 *
 * if type is 'E' (for "evaluate statement" (or perhaps "equals") ) then the var-spec is a full statement containing
 *    expressions, variables and operators per the expression evaluation built into Asterisk.
 * if type is 'T' (for "going True"), var-spec is a single (already-defined) variable name, and the result will be 1
 *    if the varible has just gone from 0 to 1.
 * if type is 'F' (for "going False"), var-spec is a single (already-defined) variable name, and the result will be 1
 *    if the varible has just gone from 1 to 0.
 * if type is 'N' (for "no change"), var-spec is a single (already-defined) variable name, and the result will be 1
 *    if the varible has not changed.
 *
 * "RANGER" mode configuration:
 * in the node stanza in rpt.conf ONLY the following need be specified for a RANGER node:
 *
 * 
 *
 * [90101]
 *
 * rxchannel=Radio/usb90101
 * functions=rangerfunctions
 * litzcmd=*32008
 *
 * This example given would be for node "90101" (note ALL RANGER nodes MUST begin with '9'.
 * litzcmd specifes the function that LiTZ inititiates to cause a connection
 * "rangerfunctions" in this example, is a function stanza that AT LEAST has the *3 command
 * to connect to another node
 *
 *
*/

/*** MODULEINFO
	<depend>tonezone</depend>
	<defaultenabled>no</defaultenabled>
 ***/

/* maximum digits in DTMF buffer, and seconds after * for DTMF command timeout */

#define	MAXDTMF 32
#define	MAXMACRO 2048
#define	MAXLINKLIST 5120
#define	LINKLISTTIME 10000
#define	LINKLISTSHORTTIME 200
#define	LINKPOSTTIME 30000
#define	LINKPOSTSHORTTIME 200
#define	KEYPOSTTIME 30000
#define	KEYPOSTSHORTTIME 200
#define	KEYTIMERTIME 250
#define	MACROTIME 100
#define	MACROPTIME 500
#define	DTMF_TIMEOUT 3
#define	KENWOOD_RETRIES 5
#define	TOPKEYN 32
#define	TOPKEYWAIT 3
#define	TOPKEYMAXSTR 30
#define	NEWKEYTIME 2000

#define	AUTHTELLTIME 7000
#define	AUTHTXTIME 1000
#define	AUTHLOGOUTTIME 25000

#define	DISC_TIME 10000  /* report disc after 10 seconds of no connect */
#define	MAX_RETRIES 5
#define	MAX_RETRIES_PERM 1000000000

#define	APRSTT_PIPE "/tmp/aprs_ttfifo"
#define	APRSTT_SUB_PIPE "/tmp/aprs_ttfifo_%s"

#define	REDUNDANT_TX_TIME 2000

#define	RETRY_TIMER_MS 5000

#define	PATCH_DIALPLAN_TIMEOUT 1500

#define	RPT_LOCKOUT_SECS 10

#define MAXPEERSTR 31
#define	MAXREMSTR 15

#define	DELIMCHR ','
#define	QUOTECHR 34

#define	MONITOR_DISK_BLOCKS_PER_MINUTE 38

#define	DEFAULT_MONITOR_MIN_DISK_BLOCKS 10000
#define	DEFAULT_REMOTE_INACT_TIMEOUT (15 * 60)
#define	DEFAULT_REMOTE_TIMEOUT (60 * 60)
#define	DEFAULT_REMOTE_TIMEOUT_WARNING (3 * 60)
#define	DEFAULT_REMOTE_TIMEOUT_WARNING_FREQ 30

#define	DEFAULT_ERXGAIN "-3.0"
#define	DEFAULT_ETXGAIN "3.0"
#define	DEFAULT_TRXGAIN "-3.0"
#define	DEFAULT_TTXGAIN "3.0"
#define	DEFAULT_LINKMONGAIN "0.0"

#define	DEFAULT_EANNMODE 1
#define	DEFAULT_TANNMODE 1

#define	DEFAULT_RXBURST_TIME 250 
#define	DEFAULT_RXBURST_THRESHOLD 16

#define	DEFAULT_SPLIT_2M 600
#define	DEFAULT_SPLIT_70CM 5000

#define TONE_SAMPLE_RATE 8000
#define TONE_SAMPLES_IN_FRAME 160

#define	MAX_TEXTMSG_SIZE 160

#define	MAX_EXTNODEFILES 50
#define	MAX_LOCALLINKNODES 50
#define	MAX_LSTUFF 20

#define ISRANGER(name) (name[0] == '9')

#define	NODES "nodes"
#define	EXTNODES "extnodes"
#define MEMORY "memory"
#define MACRO "macro"
#define	FUNCTIONS "functions"
#define TELEMETRY "telemetry"
#define MORSE "morse"
#define	TONEMACRO "tonemacro"
#define	MDCMACRO "mdcmacro"
#define	DTMFKEYS "dtmfkeys"
#define	FUNCCHAR '*'
#define	ENDCHAR '#'
#define	EXTNODEFILE "/var/lib/asterisk/rpt_extnodes"
#define	NODENAMES "rpt/nodenames"
#define	PARROTFILE "/tmp/parrot_%s_%u"
#define	GPSFILE "/tmp/gps.dat"

#define	GPS_VALID_SECS 60
#define	GPS_UPDATE_SECS 30

#define	PARROTTIME 1000

#define	TELEM_HANG_TIME 15000
#define	LINK_HANG_TIME 120000

#define	DEFAULT_IOBASE 0x378

#define	DEFAULT_CIV_ADDR 0x58

#define	MAXCONNECTTIME 5000

#define MAXNODESTR 300

#define MAXNODELEN 16

#define MAXIDENTLEN 32

#define MAXPATCHCONTEXT 100

#define ACTIONSIZE 32

#define TELEPARAMSIZE 400

#define REM_SCANTIME 100

#define	DTMF_LOCAL_TIME 250
#define	DTMF_LOCAL_STARTTIME 500

#define	IC706_PL_MEMORY_OFFSET 50

#define	VOX_ON_DEBOUNCE_COUNT 3
#define	VOX_OFF_DEBOUNCE_COUNT 20
#define	VOX_MAX_THRESHOLD 10000.0
#define	VOX_MIN_THRESHOLD 3000.0
#define	VOX_TIMEOUT_MS 10000
#define	VOX_RECOVER_MS 2000
#define	SIMPLEX_PATCH_DELAY 25
#define	SIMPLEX_PHONE_DELAY 25

#define	RX_LINGER_TIME 50
#define	RX_LINGER_TIME_IAXKEY 150

#define	REQUIRED_ZAPTEL_VERSION 'A'

#define	STATPOST_PROGRAM "/usr/bin/wget,-q,--output-document=/dev/null,--no-check-certificate"

#define	ALLOW_LOCAL_CHANNELS

#define EL_DB_ROOT "echolink"

#define	DEFAULT_LITZ_TIME 3000
#define	DEFAULT_LITZ_CHAR "0"

/*
 * DAQ subsystem
 */

#define MAX_DAQ_RANGES 16  /* Max number of entries for range() */
#define MAX_DAQ_ENTRIES 10 /* Max number of DAQ devices */
#define MAX_DAQ_NAME 32 /* Max length of a device name */
#define MAX_DAQ_DEV 64 /* Max length of a daq device path */
#define MAX_METER_FILES 10 /* Max number of sound files in a meter def. */
#define DAQ_RX_TIMEOUT 50 /* Receive time out for DAQ subsystem */ 
#define DAQ_ADC_ACQINT 10 /* Acquire interval in sec. for ADC channels */
#define ADC_HIST_TIME 300 /* Time  in sec. to calculate short term avg, high and low peaks from. */
#define ADC_HISTORY_DEPTH ADC_HIST_TIME/DAQ_ADC_ACQINT
 

enum {REM_OFF,REM_MONITOR,REM_TX};

enum {LINKMODE_OFF,LINKMODE_ON,LINKMODE_FOLLOW,LINKMODE_DEMAND,
	LINKMODE_GUI,LINKMODE_PHONE,LINKMODE_ECHOLINK,LINKMODE_TLB};

enum{ID,PROC,TERM,COMPLETE,UNKEY,REMDISC,REMALREADY,REMNOTFOUND,REMGO,
	CONNECTED,CONNFAIL,STATUS,TIMEOUT,ID1, STATS_TIME, PLAYBACK,
	LOCALPLAY, STATS_VERSION, IDTALKOVER, ARB_ALPHA, TEST_TONE, REV_PATCH,
	TAILMSG, MACRO_NOTFOUND, MACRO_BUSY, LASTNODEKEY, FULLSTATUS,
	MEMNOTFOUND, INVFREQ, REMMODE, REMLOGIN, REMXXX, REMSHORTSTATUS,
	REMLONGSTATUS, LOGINREQ, SCAN, SCANSTAT, TUNE, SETREMOTE, TOPKEY,
	TIMEOUT_WARNING, ACT_TIMEOUT_WARNING, LINKUNKEY, UNAUTHTX, PARROT,
	STATS_TIME_LOCAL, VARCMD, LOCUNKEY, METER, USEROUT, PAGE,
	STATS_GPS,STATS_GPS_LEGACY, MDC1200, LASTUSER, REMCOMPLETE, PFXTONE};


enum {REM_SIMPLEX,REM_MINUS,REM_PLUS};

enum {REM_LOWPWR,REM_MEDPWR,REM_HIPWR};

enum {DC_INDETERMINATE, DC_REQ_FLUSH, DC_ERROR, DC_COMPLETE, DC_COMPLETEQUIET, DC_DOKEY};

enum {SOURCE_RPT, SOURCE_LNK, SOURCE_RMT, SOURCE_PHONE, SOURCE_DPHONE, SOURCE_ALT};

enum {DLY_TELEM, DLY_ID, DLY_UNKEY, DLY_CALLTERM, DLY_COMP, DLY_LINKUNKEY, DLY_PARROT, DLY_MDC1200};

enum {REM_MODE_FM,REM_MODE_USB,REM_MODE_LSB,REM_MODE_AM};

enum {HF_SCAN_OFF,HF_SCAN_DOWN_SLOW,HF_SCAN_DOWN_QUICK,
      HF_SCAN_DOWN_FAST,HF_SCAN_UP_SLOW,HF_SCAN_UP_QUICK,HF_SCAN_UP_FAST};

/*
 * DAQ Subsystem
 */

enum{DAQ_PS_IDLE = 0, DAQ_PS_START, DAQ_PS_BUSY, DAQ_PS_IN_MONITOR};
enum{DAQ_CMD_IN, DAQ_CMD_ADC, DAQ_CMD_OUT, DAQ_CMD_PINSET, DAQ_CMD_MONITOR};
enum{DAQ_SUB_CUR = 0, DAQ_SUB_MIN, DAQ_SUB_MAX, DAQ_SUB_STMIN, DAQ_SUB_STMAX, DAQ_SUB_STAVG};
enum{DAQ_PT_INADC = 1, DAQ_PT_INP, DAQ_PT_IN, DAQ_PT_OUT};
enum{DAQ_TYPE_UCHAMELEON};


#define DEFAULT_TELEMDUCKDB "-9"


#define	DEFAULT_RPT_TELEMDEFAULT 1
#define	DEFAULT_RPT_TELEMDYNAMIC 1
#define	DEFAULT_GUI_LINK_MODE LINKMODE_ON
#define	DEFAULT_GUI_LINK_MODE_DYNAMIC 1
#define	DEFAULT_PHONE_LINK_MODE LINKMODE_ON
#define	DEFAULT_PHONE_LINK_MODE_DYNAMIC 1
#define	DEFAULT_ECHOLINK_LINK_MODE LINKMODE_DEMAND
#define	DEFAULT_ECHOLINK_LINK_MODE_DYNAMIC 1
#define	DEFAULT_TLB_LINK_MODE LINKMODE_DEMAND
#define	DEFAULT_TLB_LINK_MODE_DYNAMIC 1

#include "asterisk.h"
#include "../astver.h"

/*
 * Defines for the "old" way to manage module hooks into Asterisk
*/

#ifdef OLD_ASTERISK
#define ast_free free
#define ast_malloc malloc
#define ast_strdup strdup
#endif

#ifdef OLD_ASTERISK
#define	START_DELAY 10
#else
#define	START_DELAY 2
#endif

/*
 * Please change this revision number when you make a edit
 * use the simple format YYMMDD (better for sort)
*/

ASTERISK_FILE_VERSION(__FILE__, "$Revision: 191130 $")
// ASTERISK_FILE_VERSION(__FILE__, "$"ASTERISK_VERSION" $")

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <sys/vfs.h>
#include <math.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fnmatch.h>

#include "asterisk/utils.h"
#include "asterisk/lock.h"
#include "asterisk/file.h"
#include "asterisk/logger.h"
#include "asterisk/channel.h"
#include "asterisk/callerid.h"
#include "asterisk/pbx.h"
#include "asterisk/module.h"
#include "asterisk/translate.h"
#include "asterisk/features.h"
#include "asterisk/options.h"
#include "asterisk/cli.h"
#include "asterisk/config.h"
#include "asterisk/say.h"
#include "asterisk/localtime.h"
#include "asterisk/cdr.h"
#include "asterisk/options.h"
#include "asterisk/manager.h"
#include "asterisk/astdb.h"
#include "asterisk/app.h"
#include "asterisk/indications.h"
#include <termios.h>

#ifdef	NEW_ASTERISK
struct ast_flags config_flags = { CONFIG_FLAG_WITHCOMMENTS };
#endif

/* Un-comment the following to include support decoding of MDC-1200 digital tone
   signalling protocol (using KA6SQG's GPL'ed implementation) */
#include "../allstar/mdc_decode.c"

/* Un-comment the following to include support encoding of MDC-1200 digital tone
   signalling protocol (using KA6SQG's GPL'ed implementation) */
#include "../allstar/mdc_encode.c"

/* Un-comment the following to include support for notch filters in the
   rx audio stream (using Tony Fisher's mknotch (mkfilter) implementation) */
#include "rpt_notch.c"


#ifdef	__RPT_NOTCH
#define	MAXFILTERS 10
#endif

#ifdef	_MDC_ENCODE_H_

#define	MDCGEN_BUFSIZE 2000

struct mdcgen_pvt
{
	mdc_encoder_t *mdc;
	int origwfmt;
	struct ast_frame f;
	char buf[(MDCGEN_BUFSIZE * 2) + AST_FRIENDLY_OFFSET];
	unsigned char cbuf[MDCGEN_BUFSIZE];
} ;

struct mdcparams
{
	char	type[10];
	short	UnitID;
	short	DestID;
	short	subcode;
} ;

static int mdc1200gen(struct ast_channel *chan, char *type, short UnitID, short destID, short subcode);
static int mdc1200gen_start(struct ast_channel *chan, char *type, short UnitID, short destID, short subcode);

#endif


#ifdef	OLD_ASTERISK
int reload();
#else
static int reload(void);
#endif

AST_MUTEX_DEFINE_STATIC(rpt_master_lock);

/* Start a tone-list going */
int ast_playtones_start(struct ast_channel *chan, int vol, const char* tonelist, int interruptible);
/*! Stop the tones from playing */
void ast_playtones_stop(struct ast_channel *chan);

static  char *tdesc = "Radio Repeater / Remote Base  version 20.04 04/01/2020";

static char *app = "Rpt";

static char *synopsis = "Radio Repeater/Remote Base Control System";

static char *descrip = 
"  Rpt(nodename[|options][|M][|*]):  \n"
"    Radio Remote Link or Remote Base Link Endpoint Process.\n"
"\n"
"    Not specifying an option puts it in normal endpoint mode (where source\n"
"    IP and nodename are verified).\n"
"\n"
"    Options are as follows:\n"
"\n"
"        X - Normal endpoint mode WITHOUT security check. Only specify\n"
"            this if you have checked security already (like with an IAX2\n"
"            user/password or something).\n"
"\n"
"        Rannounce-string[|timeout[|timeout-destination]] - Amateur Radio\n"
"            Reverse Autopatch. Caller is put on hold, and announcement (as\n"
"            specified by the 'announce-string') is played on radio system.\n"
"            Users of radio system can access autopatch, dial specified\n"
"            code, and pick up call. Announce-string is list of names of\n"
"            recordings, or \"PARKED\" to substitute code for un-parking,\n"
"            or \"NODE\" to substitute node number.\n"
"\n"
"        P - Phone Control mode. This allows a regular phone user to have\n"
"            full control and audio access to the radio system. For the\n"
"            user to have DTMF control, the 'phone_functions' parameter\n"
"            must be specified for the node in 'rpt.conf'. An additional\n"
"            function (cop,6) must be listed so that PTT control is available.\n"
"\n"
"        D - Dumb Phone Control mode. This allows a regular phone user to\n"
"            have full control and audio access to the radio system. In this\n"
"            mode, the PTT is activated for the entire length of the call.\n"
"            For the user to have DTMF control (not generally recomended in\n"
"            this mode), the 'dphone_functions' parameter must be specified\n"
"            for the node in 'rpt.conf'. Otherwise no DTMF control will be\n"
"            available to the phone user.\n"
"\n"
"        S - Simplex Dumb Phone Control mode. This allows a regular phone user\n"
"            audio-only access to the radio system. In this mode, the\n"
"            transmitter is toggled on and off when the phone user presses the\n"
"            funcchar (*) key on the telephone set. In addition, the transmitter\n"
"            will turn off if the endchar (#) key is pressed. When a user first\n"
"            calls in, the transmitter will be off, and the user can listen for\n"
"            radio traffic. When the user wants to transmit, they press the *\n" 
"            key, start talking, then press the * key again or the # key to turn\n"
"            the transmitter off.  No other functions can be executed by the\n"
"            user on the phone when this mode is selected. Note: If your\n"
"            radio system is full-duplex, we recommend using either P or D\n"
"            modes as they provide more flexibility.\n"
"\n"
"        V - Set Asterisk channel variable for specified node ( e.g. rpt(2000|V|foo=bar) ).\n"
"\n"
"        q - Query Status. Sets channel variables and returns + 101 in plan.\n"
"\n"
"        M - Memory Channel Steer as MXX where XX is the memory channel number.\n"
"\n"
"        * - Alt Macro to execute (e.g. *7 for status)\n"
"\n";
;

static int debug = 0;  /* Set this >0 for extra debug output */
static int nrpts = 0;

static char remdtmfstr[] = "0123456789*#ABCD";

enum {TOP_TOP,TOP_WON,WON_BEFREAD,BEFREAD_AFTERREAD};

int max_chan_stat [] = {22000,1000,22000,100,22000,2000,22000};

int nullfd = -1;

#define NRPTSTAT 7

struct rpt_chan_stat
{
	struct timeval last;
	long long total;
	unsigned long count;
	unsigned long largest;
	struct timeval largest_time;
};

char *discstr = "!!DISCONNECT!!";
char *newkeystr = "!NEWKEY!";
char *newkey1str = "!NEWKEY1!";
char *iaxkeystr = "!IAXKEY!";
static char *remote_rig_ft950="ft950";
static char *remote_rig_ft897="ft897";
static char *remote_rig_ft100="ft100";
static char *remote_rig_rbi="rbi";
static char *remote_rig_kenwood="kenwood";
static char *remote_rig_tm271="tm271";
static char *remote_rig_tmd700="tmd700";
static char *remote_rig_ic706="ic706";
static char *remote_rig_xcat="xcat";
static char *remote_rig_rtx150="rtx150";
static char *remote_rig_rtx450="rtx450";
static char *remote_rig_ppp16="ppp16";	  		// parallel port programmable 16 channels

/*
 * DTMF Tones - frequency pairs used to generate them along with the required timings
 */
 
static char* dtmf_tones[] = {
	"!941+1336/200,!0/200",	/* 0 */
	"!697+1209/200,!0/200",	/* 1 */
	"!697+1336/200,!0/200",	/* 2 */
	"!697+1477/200,!0/200",	/* 3 */
	"!770+1209/200,!0/200",	/* 4 */
	"!770+1336/200,!0/200",	/* 5 */
	"!770+1477/200,!0/200",	/* 6 */
	"!852+1209/200,!0/200",	/* 7 */
	"!852+1336/200,!0/200",	/* 8 */
	"!852+1477/200,!0/200",	/* 9 */
	"!697+1633/200,!0/200",	/* A */
	"!770+1633/200,!0/200",	/* B */
	"!852+1633/200,!0/200",	/* C */
	"!941+1633/200,!0/200",	/* D */
	"!941+1209/200,!0/200",	/* * */
	"!941+1477/200,!0/200" };	/* # */

#define ISRIG_RTX(x) ((!strcmp(x,remote_rig_rtx150)) || (!strcmp(x,remote_rig_rtx450)))
#define	IS_XPMR(x) (!strncasecmp(x->rxchanname,"rad",3))


#ifdef	OLD_ASTERISK
STANDARD_LOCAL_USER;
LOCAL_USER_DECL;
#endif

#define	MSWAIT 20
#define	HANGTIME 5000
#define SLEEPTIME 900		/* default # of seconds for of no activity before entering sleep mode */
#define	TOTIME 180000
#define	IDTIME 300000
#define	MAXRPTS 500
#define MAX_STAT_LINKS 32
#define POLITEID 30000
#define FUNCTDELAY 1500

#define	MAXXLAT 20
#define	MAXXLATTIME 3

#define MAX_SYSSTATES 10

#define FT897_SERIAL_DELAY 75000		/* # of usec to wait between some serial commands on FT-897 */
#define FT100_SERIAL_DELAY 75000		/* # of usec to wait between some serial commands on FT-897 */

struct vox {
	float	speech_energy;
	float	noise_energy;
	int	enacount;
	char	voxena;
	char	lastvox;
	int	offdebcnt;
	int	ondebcnt;
} ;

#define	mymax(x,y) ((x > y) ? x : y)
#define	mymin(x,y) ((x < y) ? x : y)

struct rpt_topkey
{
char	node[TOPKEYMAXSTR];
int	timesince;
int	keyed;
} ;

struct rpt_xlat
{
char	funccharseq[MAXXLAT];
char	endcharseq[MAXXLAT];
char	passchars[MAXXLAT];
int	funcindex;
int	endindex;
time_t	lastone;
} ;

static time_t	starttime = 0;

static  pthread_t rpt_master_thread;

/*
 * Structure that holds information regarding app_rpt operation
*/ 
struct rpt;

/*
 * Structure used to manage links 
*/
struct rpt_link
{
	struct rpt_link *next;
	struct rpt_link *prev;
	char	mode;			/* 1 if in tx mode */
	char	isremote;
	char	phonemode;
	char	phonevox;		/* vox the phone */
	char	phonemonitor;		/* no tx or funs for the phone */
	char	name[MAXNODESTR];	/* identifier (routing) string */
	char	lasttx;
	char	lasttx1;
	char	lastrx;
	char	lastrealrx;
	char	lastrx1;
	char	wouldtx;
	char	connected;
	char	hasconnected;
	char	perma;
	char	thisconnected;
	char	outbound;
	char	disced;
	char	killme;
	long	elaptime;
	long	disctime;
	long 	retrytimer;
	long	retxtimer;
	long	rerxtimer;
	long	rxlingertimer;
	int	retries;
	int	max_retries;
	int	reconnects;
	long long connecttime;
	struct ast_channel *chan;	
	struct ast_channel *pchan;	
	char	linklist[MAXLINKLIST];
	time_t	linklistreceived;
	long	linklisttimer;
	int	dtmfed;
	int linkunkeytocttimer;
	struct timeval lastlinktv;
	struct	ast_frame *lastf1,*lastf2;
	struct	rpt_chan_stat chan_stat[NRPTSTAT];
	struct vox vox;
	char wasvox;
	int voxtotimer;
	char voxtostate;
	char newkey;
	char iaxkey;
	int linkmode;
	int newkeytimer;
	char gott;
	time_t	lastkeytime;
	time_t	lastunkeytime;
#ifdef OLD_ASTERISK
        AST_LIST_HEAD(, ast_frame) rxq;
#else
	AST_LIST_HEAD_NOLOCK(, ast_frame) rxq;
#endif
#ifdef OLD_ASTERISK
        AST_LIST_HEAD(, ast_frame) textq;
#else
	AST_LIST_HEAD_NOLOCK(, ast_frame) textq;
#endif
} ;

/*
 * Structure used to manage link status
*/
struct rpt_lstat
{
	struct	rpt_lstat *next;
	struct	rpt_lstat *prev;
	char	peer[MAXPEERSTR];
	char	name[MAXNODESTR];
	char	mode;
	char	outbound;
	int	reconnects;
	char	thisconnected;
	long long	connecttime;
	struct	rpt_chan_stat chan_stat[NRPTSTAT];
} ;

struct rpt_tele
{
	struct rpt_tele *next;
	struct rpt_tele *prev;
	struct rpt *rpt;
	struct ast_channel *chan;
	int	mode;
	struct rpt_link mylink;
	char param[TELEPARAMSIZE];
	union {
		int i;
		void *p;
		char _filler[8];
	} submode;
	unsigned int parrot;
	char killed;
	pthread_t threadid;
} ;

struct function_table_tag
{
	char action[ACTIONSIZE];
	int (*function)(struct rpt *myrpt, char *param, char *digitbuf, 
		int command_source, struct rpt_link *mylink);
} ;

/*
 * Structs used in the DAQ code
 */


struct daq_tx_entry_tag{
	char txbuff[32];
	struct daq_tx_entry_tag *prev;
	struct daq_tx_entry_tag *next;
};

struct daq_pin_entry_tag{
	int num;
	int pintype;
	int command;
	int state;
	int value;
	int valuemax;
	int valuemin;
	int ignorefirstalarm;
	int alarmmask;
	int adcnextupdate;
	int adchistory[ADC_HISTORY_DEPTH];
	char alarmargs[64];
	void (*monexec)(struct daq_pin_entry_tag *);
	struct daq_pin_entry_tag *next;
};


struct daq_entry_tag{
	char name[MAX_DAQ_NAME];
	char dev[MAX_DAQ_DEV];
	int type;
	int fd;
	int active;
	time_t adcacqtime;
	pthread_t threadid;
	ast_mutex_t lock;
	struct daq_tx_entry_tag *txhead;
	struct daq_tx_entry_tag *txtail;
	struct daq_pin_entry_tag *pinhead;
	struct daq_entry_tag *next;
};

struct daq_tag{
	int ndaqs;
	struct daq_entry_tag *hw;
};


/* Used to store the morse code patterns */

struct morse_bits
{		  
	int len;
	int ddcomb;
} ;

struct telem_defaults
{
	char name[20];
	char value[200];
} ;


struct sysstate
{
	char txdisable;
	char totdisable;
	char linkfundisable;
	char autopatchdisable;
	char schedulerdisable;
	char userfundisable;
	char alternatetail;
	char noincomingconns;
	char sleepena;
};

/* rpt cmd support */
#define CMD_DEPTH 1
#define CMD_STATE_IDLE 0
#define CMD_STATE_BUSY 1
#define CMD_STATE_READY 2
#define CMD_STATE_EXECUTING 3

struct rpt_cmd_struct
{
    int state;
    int functionNumber;
    char param[MAXDTMF];
    char digits[MAXDTMF];
    int command_source;
};

typedef struct {
	int v2;
	int v3;
	int chunky;
	int fac;
	int samples;
} goertzel_state_t;

typedef struct {
	int value;
	int power;
} goertzel_result_t;

typedef struct
{
	int freq;
	int block_size;
	int squelch;		/* Remove (squelch) tone */
	goertzel_state_t tone;
	float energy;		/* Accumulated energy of the current block */
	int samples_pending;	/* Samples remain to complete the current block */
	int mute_samples;	/* How many additional samples needs to be muted to suppress already detected tone */

	int hits_required;	/* How many successive blocks with tone we are looking for */
	float threshold;	/* Energy of the tone relative to energy from all other signals to consider a hit */

	int hit_count;		/* How many successive blocks we consider tone present */
	int last_hit;		/* Indicates if the last processed block was a hit */

} tone_detect_state_t;

/*
 * Populate rpt structure with data
*/ 
static struct rpt
{
	ast_mutex_t lock;
	ast_mutex_t remlock;
	ast_mutex_t statpost_lock;
	struct ast_config *cfg;
	char reload;
	char reload1;
	char deleted;
	char xlink;		 							// cross link state of a share repeater/remote radio
	unsigned int statpost_seqno;

	char *name;
	char *rxchanname;
	char *txchanname;
	char remote;
	char *remoterig;
	struct	rpt_chan_stat chan_stat[NRPTSTAT];
	unsigned int scram;
#ifdef	_MDC_DECODE_H_
	mdc_decoder_t *mdc;
#endif

	struct {
		char *ourcontext;
		char *ourcallerid;
		char *acctcode;
		char *ident;
		char *tonezone;
		char simple;
		char *functions;
		char *link_functions;
		char *phone_functions;
		char *dphone_functions;
		char *alt_functions;
		char *nodes;
		char *extnodes;
		char *extnodefiles[MAX_EXTNODEFILES];
		int  extnodefilesn;
		char *patchconnect;
		char *lnkactmacro;
		char *lnkacttimerwarn;
		char *rptinactmacro;
		char *dtmfkeys;
		int hangtime;
		int althangtime;
		int totime;
		int idtime;
		int tailmessagetime;
		int tailsquashedtime;
		int sleeptime;
		int lnkacttime;
		int rptinacttime;
		int duplex;
		int politeid;
		char *tailmessages[500];
		int tailmessagemax;
		char	*memory;
		char	*macro;
		char	*tonemacro;
		char	*mdcmacro;
		char	*startupmacro;
		char	*morse;
		char	*telemetry;
		int iobase;
		char *ioport;
		int iospeed;
		char funcchar;
		char endchar;
		char nobusyout;
		char notelemtx;
		char propagate_dtmf;
		char propagate_phonedtmf;
		char linktolink;
		unsigned char civaddr;
		struct rpt_xlat inxlat;
		struct rpt_xlat outxlat;
		char *archivedir;
		int authlevel;
		char *csstanzaname;
		char *skedstanzaname;
		char *txlimitsstanzaname;
		long monminblocks;
		int remoteinacttimeout;
		int remotetimeout;
		int remotetimeoutwarning;
		int remotetimeoutwarningfreq;
		int sysstate_cur;
		struct sysstate s[MAX_SYSSTATES];
		char parrotmode;
		int parrottime;
		char *rptnode;
		char remote_mars;
		int voxtimeout_ms;
		int voxrecover_ms;
		int simplexpatchdelay;
		int simplexphonedelay;
		char telemdefault;
		char telemdynamic;		
		char lnkactenable;
		char *statpost_program;
		char *statpost_url;
		char linkmode[10];
		char linkmodedynamic[10];
		char *locallist[16];
		int nlocallist;
		char ctgroup[16];
		float telemnomgain;		/*!< \brief nominal gain adjust for telemetry */
		float telemduckgain;	/*!< \brief duck on busy gain adjust for telemetry */
		float erxgain;
		float etxgain;
		float linkmongain;
		char eannmode; /* {NONE,NODE,CALL,BOTH} */
		float trxgain;
		float ttxgain;
		char tannmode; /* {NONE,NODE,CALL,BOTH} */
		char *discpgm;
		char *connpgm;
		char *mdclog;
		char nolocallinkct;
		char nounkeyct;
		char holdofftelem;
		char beaconing;
		int rxburstfreq;
		int rxbursttime;
		int rxburstthreshold;
		int litztime;
		char *litzchar;
		char *litzcmd;
		int itxctcss;		
		int gpsfeet;
		int default_split_2m;
		int default_split_70cm;
		int dtmfkey;
		char dias;
		char dusbabek;
		char *outstreamcmd;
		char dopfxtone;
		char *events;
		char *locallinknodes[MAX_LOCALLINKNODES];
		int locallinknodesn;
		char *eloutbound;
		int elke;
		char *aprstt;
		char *lconn[MAX_LSTUFF];
		int nlconn;
		char *ldisc[MAX_LSTUFF];
		int nldisc;
		char *timezone;
	} p;
	struct rpt_link links;
	int unkeytocttimer;
	time_t lastkeyedtime;
	time_t lasttxkeyedtime;
	char keyed;
	char txkeyed;
	char exttx;
	char localtx;
	char remrx;	
	char remoterx;
	char remotetx;
	char remoteon;
	char remtxfreqok;
	char tounkeyed;
	char tonotify;
	char dtmfbuf[MAXDTMF];
	char macrobuf[MAXMACRO];
	char rem_dtmfbuf[MAXDTMF];
	char lastdtmfcommand[MAXDTMF];
	char cmdnode[50];
	char nowchan;						// channel now
	char waschan;						// channel selected initially or by command
	char bargechan;						// barge in channel
	char macropatch;					// autopatch via tonemacro state
	char parrotstate;
	char parrotonce;
	char linkactivityflag;
	char rptinactwaskeyedflag;
	char lastitx;
	char remsetting;
	char tunetx;
	int  parrottimer;
	unsigned int parrotcnt;
	int telemmode;
	struct ast_channel *rxchannel,*txchannel, *monchannel, *parrotchannel;
	struct ast_channel *pchannel,*txpchannel, *zaprxchannel, *zaptxchannel;
	struct ast_channel *telechannel;  	/*!< \brief pseudo channel between telemetry conference and txconf */
	struct ast_channel *btelechannel;  	/*!< \brief pseudo channel buffer between telemetry conference and txconf */
	struct ast_channel *voxchannel;
	struct ast_frame *lastf1,*lastf2;
	struct rpt_tele tele;
	struct timeval lasttv,curtv;
	pthread_t rpt_call_thread,rpt_thread;
	time_t dtmf_time,rem_dtmf_time,dtmf_time_rem;
	int calldigittimer;
	int tailtimer,totimer,idtimer,txconf,conf,callmode,cidx,scantimer,tmsgtimer,skedtimer,linkactivitytimer,elketimer;
	int mustid,tailid;
	int rptinacttimer;
	int tailevent;
	int teleconf;								/*!< \brief telemetry conference id */
	int telemrefcount;
	int dtmfidx,rem_dtmfidx;
	int dailytxtime,dailykerchunks,totalkerchunks,dailykeyups,totalkeyups,timeouts;
	int totalexecdcommands, dailyexecdcommands;
	long	retxtimer;
	long	rerxtimer;
	long long totaltxtime;
	char mydtmf;
	char exten[AST_MAX_EXTENSION];
	char freq[MAXREMSTR],rxpl[MAXREMSTR],txpl[MAXREMSTR];
	int  splitkhz;
	char offset;
	char powerlevel;
	char txplon;
	char rxplon;
	char remmode;
	char tunerequest;
	char hfscanmode;
	int hfscanstatus;
	char hfscanstop;
	char lastlinknode[MAXNODESTR];
	char savednodes[MAXNODESTR];
	int stopgen;
	int remstopgen;
	char patchfarenddisconnect;
	char patchnoct;
	char patchquiet;
	char patchvoxalways;
	char patchcontext[MAXPATCHCONTEXT];
	char patchexten[AST_MAX_EXTENSION];
	int patchdialtime;
	int macro_longest;
	int phone_longestfunc;
	int alt_longestfunc;
	int dphone_longestfunc;
	int link_longestfunc;
	int longestfunc;
	int longestnode;
	int threadrestarts;		
	int tailmessagen;
	time_t disgorgetime;
	time_t lastthreadrestarttime;
	long	macrotimer;
	char	lastnodewhichkeyedusup[MAXNODESTR];
	int	dtmf_local_timer;
	char	dtmf_local_str[100];
	struct ast_filestream *monstream,*parrotstream;
	char	loginuser[50];
	char	loginlevel[10];
	long	authtelltimer;
	long	authtimer;
	int iofd;
	time_t start_time,last_activity_time;
	char	lasttone[32];
	struct rpt_tele *active_telem;
	struct 	rpt_topkey topkey[TOPKEYN];
	int topkeystate;
	time_t topkeytime;
	int topkeylong;
	struct vox vox;
	char wasvox;
	int voxtotimer;
	char voxtostate;
	int linkposttimer;			
	int keyposttimer;			
	int lastkeytimer;			
	char newkey;
	char iaxkey;
	char inpadtest;
	long rxlingertimer;
	char localoverride;
	char ready;
	char lastrxburst;
	char reallykeyed;
	char dtmfkeyed;
	char dtmfkeybuf[MAXDTMF];
	char localteleminhibit;		/*!< \brief local telemetry inhibit */
	char noduck;				/*!< \brief no ducking of telemetry  */
	char sleepreq;
	char sleep;
	char lastdtmfuser[MAXNODESTR];
	char curdtmfuser[MAXNODESTR];
	int  sleeptimer;
	time_t lastgpstime;
	int outstreampipe[2];
	int outstreampid;
	struct ast_channel *remote_webtransceiver;
#ifndef	OLD_ASTERISK
	struct timeval lastdtmftime;
#endif
	tone_detect_state_t burst_tone_state;
#ifdef OLD_ASTERISK
	AST_LIST_HEAD(, ast_frame) txq;
#else
	AST_LIST_HEAD_NOLOCK(, ast_frame) txq;
#endif
#ifdef OLD_ASTERISK
	AST_LIST_HEAD(, ast_frame) rxq;
#else
	AST_LIST_HEAD_NOLOCK(, ast_frame) rxq;
#endif
	char txrealkeyed;
#ifdef	__RPT_NOTCH
	struct rptfilter
	{
		char	desc[100];
		float	x0;
		float	x1;
		float	x2;
		float	y0;
		float	y1;
		float	y2;
		float	gain;
		float	const0;
		float	const1;
		float	const2;
	} filters[MAXFILTERS];
#endif
#ifdef	_MDC_DECODE_H_
	unsigned short lastunit;
	char lastmdc[32];
#endif
	struct rpt_cmd_struct cmdAction;
	struct timeval paging;
	char deferid;
	struct timeval lastlinktime;
} rpt_vars[MAXRPTS];

struct nodelog {
struct nodelog *next;
struct nodelog *prev;
time_t	timestamp;
char archivedir[MAXNODESTR];
char str[MAXNODESTR * 2];
} nodelog;

static int service_scan(struct rpt *myrpt);
static int set_mode_ft897(struct rpt *myrpt, char newmode);
static int set_mode_ft100(struct rpt *myrpt, char newmode);
static int set_mode_ic706(struct rpt *myrpt, char newmode);
static int simple_command_ft897(struct rpt *myrpt, char command);
static int simple_command_ft100(struct rpt *myrpt, unsigned char command, unsigned char p1);
static int setrem(struct rpt *myrpt);
static int setrtx_check(struct rpt *myrpt);
static int channel_revert(struct rpt *myrpt);
static int channel_steer(struct rpt *myrpt, char *data);
static void rpt_telemetry(struct rpt *myrpt,int mode, void *data);
static void rpt_manager_trigger(struct rpt *myrpt, char *event, char *value);

AST_MUTEX_DEFINE_STATIC(nodeloglock);

AST_MUTEX_DEFINE_STATIC(nodelookuplock);

#ifdef	APP_RPT_LOCK_DEBUG

#warning COMPILING WITH LOCK-DEBUGGING ENABLED!!

#define	MAXLOCKTHREAD 100

#define rpt_mutex_lock(x) _rpt_mutex_lock(x,myrpt,__LINE__)
#define rpt_mutex_unlock(x) _rpt_mutex_unlock(x,myrpt,__LINE__)

struct lockthread
{
	pthread_t id;
	int lockcount;
	int lastlock;
	int lastunlock;
} lockthreads[MAXLOCKTHREAD];


struct by_lightning
{
	int line;
	struct timeval tv;
	struct rpt *rpt;
	struct lockthread lockthread;
} lock_ring[32];

int lock_ring_index = 0;

AST_MUTEX_DEFINE_STATIC(locklock);

static struct lockthread *get_lockthread(pthread_t id)
{
int	i;

	for(i = 0; i < MAXLOCKTHREAD; i++)
	{
		if (lockthreads[i].id == id) return(&lockthreads[i]);
	}
	return(NULL);
}

static struct lockthread *put_lockthread(pthread_t id)
{
int	i;

	for(i = 0; i < MAXLOCKTHREAD; i++)
	{
		if (lockthreads[i].id == id)
			return(&lockthreads[i]);
	}
	for(i = 0; i < MAXLOCKTHREAD; i++)
	{
		if (!lockthreads[i].id)
		{
			lockthreads[i].lockcount = 0;
			lockthreads[i].lastlock = 0;
			lockthreads[i].lastunlock = 0;
			lockthreads[i].id = id;
			return(&lockthreads[i]);
		}
	}
	return(NULL);
}

/*
 * Functions related to the threading used in app_rpt dealing with locking
*/

static void rpt_mutex_spew(void)
{
	struct by_lightning lock_ring_copy[32];
	int lock_ring_index_copy;
	int i,j;
	long long diff;
	char a[100];
	struct timeval lasttv;

	ast_mutex_lock(&locklock);
	memcpy(&lock_ring_copy, &lock_ring, sizeof(lock_ring_copy));
	lock_ring_index_copy = lock_ring_index;
	ast_mutex_unlock(&locklock);

	lasttv.tv_sec = lasttv.tv_usec = 0;
	for(i = 0 ; i < 32 ; i++)
	{
		j = (i + lock_ring_index_copy) % 32;
		strftime(a,sizeof(a) - 1,"%m/%d/%Y %H:%M:%S",
			localtime(&lock_ring_copy[j].tv.tv_sec));
		diff = 0;
		if(lasttv.tv_sec)
		{
			diff = (lock_ring_copy[j].tv.tv_sec - lasttv.tv_sec)
				* 1000000;
			diff += (lock_ring_copy[j].tv.tv_usec - lasttv.tv_usec);
		}
		lasttv.tv_sec = lock_ring_copy[j].tv.tv_sec;
		lasttv.tv_usec = lock_ring_copy[j].tv.tv_usec;
		if (!lock_ring_copy[j].tv.tv_sec) continue;
		if (lock_ring_copy[j].line < 0)
		{
			ast_log(LOG_NOTICE,"LOCKDEBUG [#%d] UNLOCK app_rpt.c:%d node %s pid %x diff %lld us at %s.%06d\n",
				i - 31,-lock_ring_copy[j].line,lock_ring_copy[j].rpt->name,(int) lock_ring_copy[j].lockthread.id,diff,a,(int)lock_ring_copy[j].tv.tv_usec);
		}
		else
		{
			ast_log(LOG_NOTICE,"LOCKDEBUG [#%d] LOCK app_rpt.c:%d node %s pid %x diff %lld us at %s.%06d\n",
				i - 31,lock_ring_copy[j].line,lock_ring_copy[j].rpt->name,(int) lock_ring_copy[j].lockthread.id,diff,a,(int)lock_ring_copy[j].tv.tv_usec);
		}
	}
}


static void _rpt_mutex_lock(ast_mutex_t *lockp, struct rpt *myrpt, int line)
{
struct lockthread *t;
pthread_t id;

	id = pthread_self();
	ast_mutex_lock(&locklock);
	t = put_lockthread(id);
	if (!t)
	{
		ast_mutex_unlock(&locklock);
		return;
	}
	if (t->lockcount)
	{
		int lastline = t->lastlock;
		ast_mutex_unlock(&locklock);
		ast_log(LOG_NOTICE,"rpt_mutex_lock: Double lock request line %d node %s pid %x, last lock was line %d\n",line,myrpt->name,(int) t->id,lastline);
		rpt_mutex_spew();
		return;
	}
	t->lastlock = line;
	t->lockcount = 1;
	gettimeofday(&lock_ring[lock_ring_index].tv, NULL);
	lock_ring[lock_ring_index].rpt = myrpt;
	memcpy(&lock_ring[lock_ring_index].lockthread,t,sizeof(struct lockthread));
	lock_ring[lock_ring_index++].line = line;
	if(lock_ring_index == 32)
		lock_ring_index = 0;
	ast_mutex_unlock(&locklock);
	ast_mutex_lock(lockp);
}


static void _rpt_mutex_unlock(ast_mutex_t *lockp, struct rpt *myrpt, int line)
{
struct lockthread *t;
pthread_t id;

	id = pthread_self();
	ast_mutex_lock(&locklock);
	t = put_lockthread(id);
	if (!t)
	{
		ast_mutex_unlock(&locklock);
		return;
	}
	if (!t->lockcount)
	{
		int lastline = t->lastunlock;
		ast_mutex_unlock(&locklock);
		ast_log(LOG_NOTICE,"rpt_mutex_lock: Double un-lock request line %d node %s pid %x, last un-lock was line %d\n",line,myrpt->name,(int) t->id,lastline);
		rpt_mutex_spew();
		return;
	}
	t->lastunlock = line;
	t->lockcount = 0;
	gettimeofday(&lock_ring[lock_ring_index].tv, NULL);
	lock_ring[lock_ring_index].rpt = myrpt;
	memcpy(&lock_ring[lock_ring_index].lockthread,t,sizeof(struct lockthread));
	lock_ring[lock_ring_index++].line = -line;
	if(lock_ring_index == 32)
		lock_ring_index = 0;
	ast_mutex_unlock(&locklock);
	ast_mutex_unlock(lockp);
}

#else  /* APP_RPT_LOCK_DEBUG */

#define rpt_mutex_lock(x) ast_mutex_lock(x)
#define rpt_mutex_unlock(x) ast_mutex_unlock(x)

#endif  /* APP_RPT_LOCK_DEBUG */


#ifdef	_MDC_DECODE_H_
static const char *my_variable_match(const struct ast_config *config, const char *category, const char *variable)
{
	struct ast_variable *v;

	if (category)
	{
		for (v = ast_variable_browse(config, category); v; v = v->next)
		{
			if (!fnmatch(v->name,variable,FNM_CASEFOLD | FNM_NOESCAPE))
				return v->value;
		}

	}
	return NULL;
}
#endif

/* Return 1 if a web transceiver node */
static int iswebtransceiver(struct  rpt_link *l)
{
int	i;

	if (!l) return 0;
	for(i = 0; l->name[i]; i++)
	{
		if (!isdigit(l->name[i])) return 1;
	}
	return 0;
}

/*
* Return 1 if rig is multimode capable
*/

static int multimode_capable(struct rpt *myrpt)
{
	if(!strcmp(myrpt->remoterig, remote_rig_ft897))
		return 1;
	if(!strcmp(myrpt->remoterig, remote_rig_ft100))
		return 1;
	if(!strcmp(myrpt->remoterig, remote_rig_ft950))
		return 1;
	if(!strcmp(myrpt->remoterig, remote_rig_ic706))
		return 1;
	return 0;
}	
/*
* Return 1 if rig is narrow capable
*/

static int narrow_capable(struct rpt *myrpt)
{
	if(!strcmp(myrpt->remoterig, remote_rig_kenwood))
		return 1;
	if(!strcmp(myrpt->remoterig, remote_rig_tmd700))
		return 1;
	if(!strcmp(myrpt->remoterig, remote_rig_tm271))
		return 1;
	return 0;
}	

static char is_paging(struct rpt *myrpt)
{
char	rv = 0;

	if ((!ast_tvzero(myrpt->paging)) &&
		(ast_tvdiff_ms(ast_tvnow(),myrpt->paging) <= 300000)) rv = 1;
	return(rv);
}


static void voxinit_rpt(struct rpt *myrpt,char enable)
{

	myrpt->vox.speech_energy = 0.0;
	myrpt->vox.noise_energy = 0.0;
	myrpt->vox.enacount = 0;
	myrpt->vox.voxena = 0;
	if (!enable) myrpt->vox.voxena = -1;
	myrpt->vox.lastvox = 0;
	myrpt->vox.ondebcnt = VOX_ON_DEBOUNCE_COUNT;
	myrpt->vox.offdebcnt = VOX_OFF_DEBOUNCE_COUNT;
	myrpt->wasvox = 0;
	myrpt->voxtotimer = 0;
	myrpt->voxtostate = 0;
}

static void voxinit_link(struct rpt_link *mylink,char enable)
{

	mylink->vox.speech_energy = 0.0;
	mylink->vox.noise_energy = 0.0;
	mylink->vox.enacount = 0;
	mylink->vox.voxena = 0;
	if (!enable) mylink->vox.voxena = -1;
	mylink->vox.lastvox = 0;
	mylink->vox.ondebcnt = VOX_ON_DEBOUNCE_COUNT;
	mylink->vox.offdebcnt = VOX_OFF_DEBOUNCE_COUNT;
	mylink->wasvox = 0;
	mylink->voxtotimer = 0;
	mylink->voxtostate = 0;
}

static int dovox(struct vox *v,short *buf,int bs)
{

	int i;
	float	esquare = 0.0;
	float	energy = 0.0;
	float	threshold = 0.0;
	
	if (v->voxena < 0) return(v->lastvox);
	for(i = 0; i < bs; i++)
	{
		esquare += (float) buf[i] * (float) buf[i];
	}
	energy = sqrt(esquare);

	if (energy >= v->speech_energy)
		v->speech_energy += (energy - v->speech_energy) / 4;
	else
		v->speech_energy += (energy - v->speech_energy) / 64;

	if (energy >= v->noise_energy)
		v->noise_energy += (energy - v->noise_energy) / 64;
	else
		v->noise_energy += (energy - v->noise_energy) / 4;
	
	if (v->voxena) threshold = v->speech_energy / 8;
	else
	{
		threshold = mymax(v->speech_energy / 16,v->noise_energy * 2);
		threshold = mymin(threshold,VOX_MAX_THRESHOLD);
	}
	threshold = mymax(threshold,VOX_MIN_THRESHOLD);
	if (energy > threshold)
	{
		if (v->voxena) v->noise_energy *= 0.75;
		v->voxena = 1;
	} else 	v->voxena = 0;
	if (v->lastvox != v->voxena)
	{
		if (v->enacount++ >= ((v->lastvox) ? v->offdebcnt : v->ondebcnt))
		{
			v->lastvox = v->voxena;
			v->enacount = 0;
		}
	} else v->enacount = 0;
	return(v->lastvox);



}

/*
 * Multi-thread safe sleep routine
*/
static void rpt_safe_sleep(struct rpt *rpt,struct ast_channel *chan, int ms)
{
	struct ast_frame *f;
	struct ast_channel *cs[2],*w;

	cs[0] = rpt->rxchannel;
	cs[1] = chan;
	while (ms > 0) {
		w = ast_waitfor_n(cs,2,&ms);
		if (!w) break;
		f = ast_read(w);
		if (!f) break;
		if ((w == cs[0]) && (f->frametype != AST_FRAME_VOICE) && (f->frametype != AST_FRAME_NULL))
		{
			ast_queue_frame(rpt->rxchannel,f);
			ast_frfree(f);
			break;
		}
		ast_frfree(f);
	}
	return;
}

/*
 * Routine to forward a "call" from one channel to another
*/

static void rpt_forward(struct ast_channel *chan, char *dialstr, char *nodefrom)
{

struct ast_channel *dest,*w,*cs[2];
struct ast_frame *f;
int	ms;

	dest = ast_request("IAX2", AST_FORMAT_SLINEAR, dialstr ,NULL);
	if (!dest)
	{
		if (ast_safe_sleep(chan,150) == -1) return;
		dest = ast_request("IAX2", AST_FORMAT_SLINEAR, dialstr ,NULL);
		if (!dest)
		{
			ast_log(LOG_ERROR,"Can not create channel for rpt_forward to IAX2/%s\n",dialstr);
			return;
		}
	}
	ast_set_read_format(chan, AST_FORMAT_SLINEAR);
	ast_set_write_format(chan, AST_FORMAT_SLINEAR);
	ast_set_read_format(dest, AST_FORMAT_SLINEAR);
	ast_set_write_format(dest, AST_FORMAT_SLINEAR);
	if (option_verbose > 2)
		ast_verbose(VERBOSE_PREFIX_3 "rpt forwarding call from %s to %s on %s\n",
			nodefrom,dialstr,dest->name);
	ast_set_callerid(dest,nodefrom,chan->cid.cid_name,nodefrom);
        ast_call(dest,dialstr,999); 
	cs[0] = chan;
	cs[1] = dest;
	for(;;)
	{
		if (ast_check_hangup(chan)) break;
		if (ast_check_hangup(dest)) break;
		ms = 100;
		w = cs[0];
		cs[0] = cs[1];
		cs[1] = w;
		w = ast_waitfor_n(cs,2,&ms);
		if (!w) continue;
		if (w == chan)
		{
			f = ast_read(chan);
			if (!f) break;
			if ((f->frametype == AST_FRAME_CONTROL) &&
			    (f->subclass == AST_CONTROL_HANGUP))
			{
				ast_frfree(f);
				break;
			}
			ast_write(dest,f);
			ast_frfree(f);
		}
		if (w == dest)
		{
			f = ast_read(dest);
			if (!f) break;
			if ((f->frametype == AST_FRAME_CONTROL) &&
			    (f->subclass == AST_CONTROL_HANGUP))
			{
				ast_frfree(f);
				break;
			}
			ast_write(chan,f);
			ast_frfree(f);
		}

	}
	ast_hangup(dest);
	return;
}


/*
* CLI extensions
*/

/* Debug mode */
static int rpt_do_debug(int fd, int argc, char *argv[]);
static int rpt_do_dump(int fd, int argc, char *argv[]);
static int rpt_do_stats(int fd, int argc, char *argv[]);
static int rpt_do_lstats(int fd, int argc, char *argv[]);
static int rpt_do_nodes(int fd, int argc, char *argv[]);
static int rpt_do_xnode(int fd, int argc, char *argv[]);
static int rpt_do_local_nodes(int fd, int argc, char *argv[]);
static int rpt_do_reload(int fd, int argc, char *argv[]);
static int rpt_do_restart(int fd, int argc, char *argv[]);
static int rpt_do_playback(int fd, int argc, char *argv[]);
static int rpt_do_localplay(int fd, int argc, char *argv[]);
static int rpt_do_sendall(int fd, int argc, char *argv[]);
static int rpt_do_sendtext(int fd, int argc, char *argv[]);
static int rpt_do_fun(int fd, int argc, char *argv[]);
static int rpt_do_fun1(int fd, int argc, char *argv[]);
static int rpt_do_cmd(int fd, int argc, char *argv[]);
static int rpt_do_setvar(int fd, int argc, char *argv[]);
static int rpt_do_showvars(int fd, int argc, char *argv[]);
static int rpt_do_asl(int fd, int argc, char *argv[]);
static int rpt_do_page(int fd, int argc, char *argv[]);

static char debug_usage[] =
"Usage: rpt debug level {0-7}\n"
"       Enables debug messages in app_rpt\n";

static char dump_usage[] =
"Usage: rpt dump <nodename>\n"
"       Dumps struct debug info to log\n";

static char dump_stats[] =
"Usage: rpt stats <nodename>\n"
"       Dumps node statistics to console\n";

static char dump_lstats[] =
"Usage: rpt lstats <nodename>\n"
"       Dumps link statistics to console\n";

static char dump_nodes[] =
"Usage: rpt nodes <nodename>\n"
"       Dumps a list of directly and indirectly connected nodes to the console\n";

static char dump_xnode[] =
"Usage: rpt xnode <nodename>\n"
"       Dumps extended node info to the console\n";

static char usage_local_nodes[] =
"Usage: rpt localnodes\n"
"       Dumps a list of the locally configured node numbers to the console.\n";

static char reload_usage[] =
"Usage: rpt reload\n"
"       Reloads app_rpt running config parameters\n";

static char restart_usage[] =
"Usage: rpt restart\n"
"       Restarts app_rpt\n";

static char playback_usage[] =
"Usage: rpt playback <nodename> <sound_file_base_name>\n"
"       Send an Audio File to a node, send to all other connected nodes (global)\n";

static char localplay_usage[] =
"Usage: rpt localplay <nodename> <sound_file_base_name>\n"
"       Send an Audio File to a node, do not send to other connected nodes (local)\n";


static char sendtext_usage[] =
"Usage: rpt sendtext <nodename> <destnodename> <Text Message>\n"
"       Send a Text message to a specified node\n";

static char sendall_usage[] =
"Usage: rpt sendall <nodename> <Text Message>\n"
"       Send a Text message to all connected nodes\n";


static char fun_usage[] =
"Usage: rpt fun <nodename> <command>\n"
"       Send a DTMF function to a node\n";

static char cmd_usage[] =
"Usage: rpt cmd <nodename> <cmd-name> <cmd-index> <cmd-args>\n"
"       Send a command to a node.\n        i.e. rpt cmd 2000 ilink 3 2001\n";

static char setvar_usage[] =
"Usage: rpt setvar <nodename> <name=value> [<name=value>...]\n"
"       Set an Asterisk channel variable for a node.\nNote: variable names are case-sensitive.\n";

static char showvars_usage[] =
"Usage: rpt showvars <nodename>\n"
"       Display all the Asterisk channel variables for a node.\n";

static char asl_usage[] =
"Usage: asl [Unused value]\n"
"       Do ASL Stuff\n";

static char page_usage[] =
"Usage: rpt page <nodename> <baud> <capcode> <[ANT]Text....>\n"
"       Send an page to a user on a node, specifying capcode and type/text\n";


#ifndef	NEW_ASTERISK

static struct ast_cli_entry  cli_debug =
        { { "rpt", "debug", "level" }, rpt_do_debug, 
		"Enable app_rpt debugging", debug_usage };

static struct ast_cli_entry  cli_dump =
        { { "rpt", "dump" }, rpt_do_dump,
		"Dump app_rpt structs for debugging", dump_usage };

static struct ast_cli_entry  cli_stats =
        { { "rpt", "stats" }, rpt_do_stats,
		"Dump node statistics", dump_stats };

static struct ast_cli_entry  cli_nodes =
        { { "rpt", "nodes" }, rpt_do_nodes,
		"Dump node list", dump_nodes };

static struct ast_cli_entry  cli_xnode =
        { { "rpt", "xnode" }, rpt_do_xnode,
		"Dump extended info", dump_xnode };

static struct ast_cli_entry  cli_local_nodes =
        { { "rpt", "localnodes" }, rpt_do_local_nodes,
		"Dump list of local node numbers", usage_local_nodes };

static struct ast_cli_entry  cli_lstats =
        { { "rpt", "lstats" }, rpt_do_lstats,
		"Dump link statistics", dump_lstats };

static struct ast_cli_entry  cli_reload =
        { { "rpt", "reload" }, rpt_do_reload,
		"Reload app_rpt config", reload_usage };

static struct ast_cli_entry  cli_restart =
        { { "rpt", "restart" }, rpt_do_restart,
		"Restart app_rpt", restart_usage };

static struct ast_cli_entry  cli_playback =
        { { "rpt", "playback" }, rpt_do_playback,
		"Play Back an Audio File Globally", playback_usage };

static struct ast_cli_entry  cli_localplay =
        { { "rpt", "localplay" }, rpt_do_localplay,
                "Play Back an Audio File Locally", localplay_usage };

static struct ast_cli_entry  cli_sendtext =
        { { "rpt", "sendtext" }, rpt_do_sendtext,
                "Send a Text message to specific node", sendtext_usage };


static struct ast_cli_entry  cli_sendall =
        { { "rpt", "sendall" }, rpt_do_sendall,
                "Send a Text message to all", sendall_usage };


static struct ast_cli_entry  cli_fun =
        { { "rpt", "fun" }, rpt_do_fun,
		"Execute a DTMF function", fun_usage };

static struct ast_cli_entry  cli_fun1 =
        { { "rpt", "fun1" }, rpt_do_fun1,
		"Execute a DTMF function", fun_usage };

static struct ast_cli_entry  cli_cmd =
        { { "rpt", "cmd" }, rpt_do_cmd,
		"Execute a DTMF function", cmd_usage };

static struct ast_cli_entry  cli_setvar =
        { { "rpt", "setvar" }, rpt_do_setvar,
		"Set an Asterisk channel variable", setvar_usage };

static struct ast_cli_entry  cli_showvars =
        { { "rpt", "showvars" }, rpt_do_showvars,
		"Display Asterisk channel variables", showvars_usage };

static struct ast_cli_entry  cli_asl =
        { { "asl" }, rpt_do_asl,
               "Do ASL stuff", asl_usage };

static struct ast_cli_entry  cli_page =
        { { "rpt", "page" }, rpt_do_page,
		"Page a user on a node", page_usage };

#endif

/*
* Telemetry defaults
*/


static struct telem_defaults tele_defs[] = {
	{"ct1","|t(350,0,100,3072)(500,0,100,3072)(660,0,100,3072)"},
	{"ct2","|t(660,880,150,3072)"},
	{"ct3","|t(440,0,150,3072)"},
	{"ct4","|t(550,0,150,3072)"},
	{"ct5","|t(660,0,150,3072)"},
	{"ct6","|t(880,0,150,3072)"},
	{"ct7","|t(660,440,150,3072)"},
	{"ct8","|t(700,1100,150,3072)"},
	{"ranger","|t(1800,0,60,3072)(0,0,50,0)(1800,0,60,3072)(0,0,50,0)(1800,0,60,3072)(0,0,50,0)(1800,0,60,3072)(0,0,50,0)(1800,0,60,3072)(0,0,50,0)(1800,0,60,3072)(0,0,150,0)"},
	{"remotemon","|t(1600,0,75,2048)"},
	{"remotetx","|t(2000,0,75,2048)(0,0,75,0)(1600,0,75,2048)"},
	{"cmdmode","|t(900,904,200,2048)"},
	{"functcomplete","|t(1000,0,100,2048)(0,0,100,0)(1000,0,100,2048)"},
	{"remcomplete","|t(650,0,100,2048)(0,0,100,0)(650,0,100,2048)(0,0,100,0)(650,0,100,2048)"},
	{"pfxtone","|t(350,440,30000,3072)"}
} ;

static inline void goertzel_sample(goertzel_state_t *s, short sample)
{
	int v1;
	
	v1 = s->v2;
	s->v2 = s->v3;
	
	s->v3 = (s->fac * s->v2) >> 15;
	s->v3 = s->v3 - v1 + (sample >> s->chunky);
	if (abs(s->v3) > 32768) {
		s->chunky++;
		s->v3 = s->v3 >> 1;
		s->v2 = s->v2 >> 1;
		v1 = v1 >> 1;
	}
}

static inline void goertzel_update(goertzel_state_t *s, short *samps, int count)
{
	int i;
	
	for (i = 0; i < count; i++) {
		goertzel_sample(s, samps[i]);
	}
}


static inline float goertzel_result(goertzel_state_t *s)
{
	goertzel_result_t r;
	r.value = (s->v3 * s->v3) + (s->v2 * s->v2);
	r.value -= ((s->v2 * s->v3) >> 15) * s->fac;
	r.power = s->chunky * 2;
	return (float)r.value * (float)(1 << r.power);
}

static inline void goertzel_init(goertzel_state_t *s, float freq, int samples)
{
	s->v2 = s->v3 = s->chunky = 0.0;
	s->fac = (int)(32768.0 * 2.0 * cos(2.0 * M_PI * freq / TONE_SAMPLE_RATE));
	s->samples = samples;
}

static inline void goertzel_reset(goertzel_state_t *s)
{
	s->v2 = s->v3 = s->chunky = 0.0;
}

/*
 * Code used to detect tones
*/


static void tone_detect_init(tone_detect_state_t *s, int freq, int duration, int amp)
{
	int duration_samples;
	float x;
	int periods_in_block;

	s->freq = freq;

	/* Desired tone duration in samples */
	duration_samples = duration * TONE_SAMPLE_RATE / 1000;
	/* We want to allow 10% deviation of tone duration */
	duration_samples = duration_samples * 9 / 10;

	/* If we want to remove tone, it is important to have block size not
	   to exceed frame size. Otherwise by the moment tone is detected it is too late
 	   to squelch it from previous frames */
	s->block_size = TONE_SAMPLES_IN_FRAME;

	periods_in_block = s->block_size * freq / TONE_SAMPLE_RATE;

	/* Make sure we will have at least 5 periods at target frequency for analisys.
	   This may make block larger than expected packet and will make squelching impossible
	   but at least we will be detecting the tone */
	if (periods_in_block < 5)
		periods_in_block = 5;

	/* Now calculate final block size. It will contain integer number of periods */
	s->block_size = periods_in_block * TONE_SAMPLE_RATE / freq;

	/* tone_detect is currently only used to detect fax tones and we
	   do not need suqlching the fax tones */
	s->squelch = 0;

	/* Account for the first and the last block to be incomplete
	   and thus no tone will be detected in them */
	s->hits_required = (duration_samples - (s->block_size - 1)) / s->block_size;

	goertzel_init(&s->tone, freq, s->block_size);

	s->samples_pending = s->block_size;
	s->hit_count = 0;
	s->last_hit = 0;
	s->energy = 0.0;

	/* We want tone energy to be amp decibels above the rest of the signal (the noise).
	   According to Parseval's theorem the energy computed in time domain equals to energy
	   computed in frequency domain. So subtracting energy in the frequency domain (Goertzel result)
	   from the energy in the time domain we will get energy of the remaining signal (without the tone
	   we are detecting). We will be checking that
		10*log(Ew / (Et - Ew)) > amp
	   Calculate threshold so that we will be actually checking
		Ew > Et * threshold
	*/

	x = pow(10.0, amp / 10.0);
	s->threshold = x / (x + 1);

	ast_log(LOG_DEBUG,"Setup tone %d Hz, %d ms, block_size=%d, hits_required=%d\n", freq, duration, s->block_size, s->hits_required);
}


static int tone_detect(tone_detect_state_t *s, int16_t *amp, int samples)
{
	float tone_energy;
	int i;
	int hit = 0;
	int limit;
	int res = 0;
	int16_t *ptr;
	int start, end;

	for (start = 0;  start < samples;  start = end) {
		/* Process in blocks. */
		limit = samples - start;
		if (limit > s->samples_pending) {
			limit = s->samples_pending;
		}
		end = start + limit;

		for (i = limit, ptr = amp ; i > 0; i--, ptr++) {
			/* signed 32 bit int should be enough to suqare any possible signed 16 bit value */
			s->energy += (int32_t) *ptr * (int32_t) *ptr;

			goertzel_sample(&s->tone, *ptr);
		}

		s->samples_pending -= limit;

		if (s->samples_pending) {
			/* Finished incomplete (last) block */
			break;
		}

		tone_energy = goertzel_result(&s->tone);

		/* Scale to make comparable */
		tone_energy *= 2.0;
		s->energy *= s->block_size;

		hit = 0;
		ast_log(LOG_DEBUG,"tone %d, Ew=%.2E, Et=%.2E, s/n=%10.2f\n", s->freq, tone_energy, s->energy, tone_energy / (s->energy - tone_energy));
		if (tone_energy > s->energy * s->threshold) {
			ast_log(LOG_DEBUG,"Hit! count=%d\n", s->hit_count);
			hit = 1;
		}

		if (s->hit_count) {
			s->hit_count++;
		}

		if (hit == s->last_hit) {
			if (!hit) {
				/* Two successive misses. Tone ended */
				s->hit_count = 0;
			} else if (!s->hit_count) {
				s->hit_count++;
			}

		}

		if (s->hit_count >= s->hits_required) {
			ast_log(LOG_DEBUG,"%d Hz tone detected\n", s->freq);
			res = 1;
		}

		s->last_hit = hit;

		/* Reinitialise the detector for the next block */
		/* Reset for the next block */
		goertzel_reset(&s->tone);

		/* Advance to the next block */
		s->energy = 0.0;
		s->samples_pending = s->block_size;

		amp += limit;
	}

	return res;
}


/*
 * DAQ variables
 */

static struct daq_tag daq;


/*
* Forward decl's - these suppress compiler warnings when funcs coded further down the file than thier invokation
*/

static int setrbi(struct rpt *myrpt);
static int set_ft897(struct rpt *myrpt);
static int set_ft100(struct rpt *myrpt);
static int set_ft950(struct rpt *myrpt);
static int set_ic706(struct rpt *myrpt);
static int set_xcat(struct rpt *myrpt);
static int setkenwood(struct rpt *myrpt);
static int set_tm271(struct rpt *myrpt);
static int set_tmd700(struct rpt *myrpt);
static int setrbi_check(struct rpt *myrpt);
static int setxpmr(struct rpt *myrpt, int dotx);



/*
* Define function protos for function table here
*/

static int function_ilink(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_autopatchup(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_autopatchdn(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_status(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_cop(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_remote(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_macro(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_playback(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_localplay(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_meter(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_userout(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);
static int function_cmd(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink);

/*
* Function table
*/

static struct function_table_tag function_table[] = {
	{"cop", function_cop},
	{"autopatchup", function_autopatchup},
	{"autopatchdn", function_autopatchdn},
	{"ilink", function_ilink},
	{"status", function_status},
	{"remote", function_remote},
	{"macro", function_macro},
	{"playback", function_playback},
	{"localplay", function_localplay},
	{"meter", function_meter},
	{"userout", function_userout},
	{"cmd", function_cmd}


} ;


/*
 * *****************************************
 * Generic serial I/O routines             *
 * *****************************************
*/


/*
 * Generic serial port open command 
 */

static int serial_open(char *fname, int speed, int stop2)
{
	struct termios mode;
	int fd;

	fd = open(fname,O_RDWR);
	if (fd == -1)
	{
		if(debug >= 1)
			ast_log(LOG_WARNING,"Cannot open serial port %s\n",fname);
		return -1;
	}
	
	memset(&mode, 0, sizeof(mode));
	if (tcgetattr(fd, &mode)) {
		if(debug >= 1){
			ast_log(LOG_WARNING, "Unable to get serial parameters on %s: %s\n", fname, strerror(errno));
		}
		return -1;
	}
#ifndef	SOLARIS
	cfmakeraw(&mode);
#else
        mode.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                        |INLCR|IGNCR|ICRNL|IXON);
        mode.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
        mode.c_cflag &= ~(CSIZE|PARENB|CRTSCTS);
        mode.c_cflag |= CS8;
	if(stop2)
		mode.c_cflag |= CSTOPB;
	mode.c_cc[VTIME] = 3;
	mode.c_cc[VMIN] = 1; 
#endif

	cfsetispeed(&mode, speed);
	cfsetospeed(&mode, speed);
	if (tcsetattr(fd, TCSANOW, &mode)){
		if(debug >= 1) 
			ast_log(LOG_WARNING, "Unable to set serial parameters on %s: %s\n", fname, strerror(errno));
		return -1;
	}
	usleep(100000);
	if (debug >= 3)
		ast_log(LOG_NOTICE,"Opened serial port %s\n",fname);
	return(fd);	
}

/*
 * Return receiver ready status
 *
 * Return 1 if an Rx byte is avalable
 * Return 0 if none was avaialable after a time out period
 * Return -1 if error
 */


static int serial_rxready(int fd, int timeoutms)
{
int	myms = timeoutms;

	return(ast_waitfor_n_fd(&fd, 1, &myms,NULL));
}

/*
* Remove all RX characters in the receive buffer
*
* Return number of bytes flushed.
* or  return -1 if error
*
*/

static int serial_rxflush(int fd, int timeoutms)
{
	int res, flushed = 0;
	char c;
	
	while((res = serial_rxready(fd, timeoutms)) == 1){
		if(read(fd, &c, 1) == -1){
			res = -1;
			break;
		flushed++;
		}
	}		
	return (res == -1)? res : flushed;
}
/*
 * Receive a string from the serial device
 */

static int serial_rx(int fd, char *rxbuf, int rxmaxbytes, unsigned timeoutms, char termchr)
{
	char c;
	int i, j, res;

	if ((!rxmaxbytes) || (rxbuf == NULL)){ 
		return 0;
	}
	memset(rxbuf,0,rxmaxbytes);
	for(i = 0; i < rxmaxbytes; i++){
		if(timeoutms){
			res = serial_rxready(fd, timeoutms);
			if(res < 0)
				return -1;
			if(!res){
				break;
			}
		}
		j = read(fd,&c,1);
		if(j == -1){
			ast_log(LOG_WARNING,"read failed: %s\n", strerror(errno));
			return -1;
		}
		if (j == 0) 
			return i ;
		rxbuf[i] = c;
		if (termchr){
			rxbuf[i + 1] = 0;
			if (c == termchr) break;
		}
	}					
	if(i && debug >= 6) {
		printf("i = %d\n",i);
		printf("String returned was:\n");
		for(j = 0; j < i; j++)
			printf("%02X ", (unsigned char ) rxbuf[j]);
		printf("\n");
	}
	return i;
}

/*
 * Send a nul-terminated string to the serial device (without RX-flush)
 */

static int serial_txstring(int fd, char *txstring)
{
	int txbytes;

	txbytes = strlen(txstring);

	if(debug > 5)
		ast_log(LOG_NOTICE, "sending: %s\n", txstring);

	if(write(fd, txstring, txbytes) != txbytes){
		ast_log(LOG_WARNING,"write failed: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
		
/*
 * Write some bytes to the serial port, then optionally expect a fixed response
 */

static int serial_io(int fd, char *txbuf, char *rxbuf, int txbytes, int rxmaxbytes, unsigned int timeoutms, char termchr)
{
	int i;

	if(debug >= 7)
		ast_log(LOG_NOTICE,"fd = %d\n",fd);

	if ((rxmaxbytes) && (rxbuf != NULL)){ 
		if((i = serial_rxflush(fd, 10)) == -1)
			return -1;
		if(debug >= 7)
			ast_log(LOG_NOTICE,"%d bytes flushed prior to write\n", i);
	}

	if(write(fd, txbuf, txbytes) != txbytes){
		ast_log(LOG_WARNING,"write failed: %s\n", strerror(errno));
		return -1;
	}

	return serial_rx(fd, rxbuf, rxmaxbytes, timeoutms, termchr);
}

/*
 * ***********************************
 * Uchameleon specific routines      *
 * ***********************************
 */

/* Forward Decl's */

static int uchameleon_do_long( struct daq_entry_tag *t, int pin,
int cmd, void (*exec)(struct daq_pin_entry_tag *), int *arg1, void *arg2);
static int matchkeyword(char *string, char **param, char *keywords[]);
static int explode_string(char *str, char *strp[], int limit, char delim, char quote);
static void *uchameleon_monitor_thread(void *this);
static char *strupr(char *str);


/*
 * Start the Uchameleon monitor thread
 */




static int uchameleon_thread_start(struct daq_entry_tag *t)
{
	int res, tries = 50;
	pthread_attr_t attr;


	ast_mutex_init(&t->lock);


	/*
 	* Start up uchameleon monitor thread
 	*/

       	pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	res = ast_pthread_create(&t->threadid,&attr,uchameleon_monitor_thread,(void *) t);
	if(res){
		ast_log(LOG_WARNING, "Could not start uchameleon monitor thread\n");
		return -1;
	}

	ast_mutex_lock(&t->lock);
	while((!t->active)&&(tries)){
		ast_mutex_unlock(&t->lock);
		usleep(100*1000);
		ast_mutex_lock(&t->lock);
		tries--;
	}
	ast_mutex_unlock(&t->lock);

	if(!tries)
		return -1;


        return 0;
}

static int uchameleon_connect(struct daq_entry_tag *t)
{
	int count;
	static char *idbuf = "id\n";
	static char *ledbuf = "led on\n";
	static char *expect = "Chameleon";
	char rxbuf[20];

        if((t->fd = serial_open(t->dev, B115200, 0)) == -1){
               	ast_log(LOG_WARNING, "serial_open on %s failed!\n", t->name);
                return -1;
        }
        if((count = serial_io(t->fd, idbuf, rxbuf, strlen(idbuf), 14, DAQ_RX_TIMEOUT, 0x0a)) < 1){
              	ast_log(LOG_WARNING, "serial_io on %s failed\n", t->name);
		close(t->fd);
		t->fd = -1;
                return -1;
        }
	if(debug >= 3)
        	ast_log(LOG_NOTICE,"count = %d, rxbuf = %s\n",count,rxbuf);
	if((count != 13)||(strncmp(expect, rxbuf+4, sizeof(expect)))){
		ast_log(LOG_WARNING, "%s is not a uchameleon device\n", t->name);
		close(t->fd);
		t->fd = -1;
		return -1;
	}
	/* uchameleon LED on solid once we communicate with it successfully */
	
	if(serial_io(t->fd, ledbuf, NULL, strlen(ledbuf), 0, DAQ_RX_TIMEOUT, 0) == -1){
		ast_log(LOG_WARNING, "Can't set LED on uchameleon device\n");
		close(t->fd);
		t->fd= -1;
		return -1;
	}
	return 0;
}

/*
 * Uchameleon alarm handler
 */


static void uchameleon_alarm_handler(struct daq_pin_entry_tag *p)
{
	char *valuecopy;
	int i, busy;
	char *s;
	char *argv[7];
	int argc;


	if(!(valuecopy = ast_strdup(p->alarmargs))){
		ast_log(LOG_ERROR,"Out of memory\n");
		return;
	}
	
	argc = explode_string(valuecopy, argv, 6, ',', 0);

	if(debug >= 3){
		ast_log(LOG_NOTICE, "Alarm event on device %s, pin %d, state = %d\n", argv[0], p->num, p->value);
	}

	/*
 	* Node: argv[3]
 	* low function: argv[4]
 	* high function: argv[5]
 	*
 	*/
	i = busy = 0;
	s = (p->value) ? argv[5]: argv[4];
	if((argc == 6)&&(s[0] != '-')){
		for(i = 0; i < nrpts; i++){
			if(!strcmp(argv[3], rpt_vars[i].name)){

				struct rpt *myrpt = &rpt_vars[i];
				rpt_mutex_lock(&myrpt->lock);
				if ((MAXMACRO - strlen(myrpt->macrobuf)) < strlen(s)){
					rpt_mutex_unlock(&myrpt->lock);
					busy=1;
				}
				if(!busy){
					myrpt->macrotimer = MACROTIME;
					strncat(myrpt->macrobuf,s,MAXMACRO - 1);
				}
				rpt_mutex_unlock(&myrpt->lock);

			}
		}
	}
	if(argc != 6){
		ast_log(LOG_WARNING, "Not enough arguments to process alarm\n"); 
	}
	else if(busy){
		ast_log(LOG_WARNING, "Function decoder busy while processing alarm");
	}
	ast_free(valuecopy);
}




/*
 * Initialize pins
 */



static int uchameleon_pin_init(struct daq_entry_tag *t)
{
	int i;
	struct ast_config *ourcfg;
	struct ast_variable *var,*var2;

	/* Pin Initialization */

	#ifdef	NEW_ASTERISK
		ourcfg = ast_config_load("rpt.conf",config_flags);
	#else
		ourcfg = ast_config_load("rpt.conf");
	#endif

	if(!ourcfg)
		return -1;

	var2 = ast_variable_browse(ourcfg, t->name);
	while(var2){
		unsigned int pin;
		int x = 0;
		static char *pin_keywords[]={"inadc","inp","in","out",NULL};
		if((var2->name[0] < '0')||(var2->name[0] > '9')){
			var2 = var2->next;
			continue;
		}
		pin = (unsigned int) atoi(var2->name);
		i = matchkeyword((char *)var2->value, NULL, pin_keywords);
		if(debug >= 3)
			ast_log(LOG_NOTICE, "Pin = %d, Pintype = %d\n", pin, i);
		if(i && i < 5){
			uchameleon_do_long(t, pin, DAQ_CMD_PINSET, NULL, &i, NULL);	 /* Set pin type */
			uchameleon_do_long(t, pin, DAQ_CMD_MONITOR, NULL, &x, NULL); /* Monitor off */
			if(i == DAQ_PT_OUT){
				if(debug >= 3)
					ast_log(LOG_NOTICE,"Set output pin %d low\n", pin); /* Set output pins low */
				uchameleon_do_long(t, pin, DAQ_CMD_OUT, NULL, &x, NULL);
			}
		}
		else
			ast_log(LOG_WARNING,"Invalid pin type: %s\n", var2->value);
		var2 = var2->next;
	}

	/*
 	* Alarm initialization
 	*/

	var = ast_variable_browse(ourcfg,"alarms");
	while(var){
		int ignorefirst,pin;
		char s[64];
		char *argv[7];
		struct daq_pin_entry_tag *p;


		/* Parse alarm entry */

		strncpy(s,var->value,sizeof(s));

		if(explode_string(s, argv, 6, ',', 0) != 6){
			ast_log(LOG_WARNING,"Alarm arguments must be 6 for %s\n", var->name);
			var = var->next;
			continue;
		}

		ignorefirst = atoi(argv[2]);

		if(!(pin = atoi(argv[1]))){
			ast_log(LOG_WARNING,"Pin must be greater than 0 for %s\n",var->name);
			var = var->next;
			continue;
		}

		/* Find the pin entry */
		p = t->pinhead;
		while(p){
			if(p->num == pin)
				break;
			p = p->next;
		}
		if(!p){
			ast_log(LOG_WARNING,"Can't find pin %d for device %s\n", pin, argv[0]);
			var = var->next;
			continue;
		}

		if(!strcmp(argv[0], t->name)){
			strncpy(p->alarmargs, var->value, 64); /* Save the alarm arguments in the pin entry */
			p->alarmargs[63] = 0;
			ast_log(LOG_NOTICE,"Adding alarm %s on pin %d\n", var->name, pin);
			uchameleon_do_long(t, pin, DAQ_CMD_MONITOR, uchameleon_alarm_handler, &ignorefirst, NULL);
		}
		var = var->next;
	}

	ast_config_destroy(ourcfg);
	time(&t->adcacqtime); /* Start ADC Acquisition */ 
	return -0;
}


/*
 * Open the serial channel and test for the uchameleon device at the end of the link
 */

static int uchameleon_open(struct daq_entry_tag *t)
{
	int res;


	if(!t)
		return -1;

	if(uchameleon_connect(t)){
		ast_log(LOG_WARNING,"Cannot open device %s", t->name);
		return -1;
	}

	res = uchameleon_thread_start(t);

	if(!res)
		res = uchameleon_pin_init(t);

	return res;

}

/*
 * Close uchameleon
 */

static int uchameleon_close(struct daq_entry_tag *t)
{
	int res = 0;
	char *ledpat="led pattern 253\n";
	struct daq_pin_entry_tag *p,*pn;
	struct daq_tx_entry_tag *q,*qn;

	if(!t)
		return -1;

	ast_mutex_lock(&t->lock);

	if(t->active){
		res = pthread_kill(t->threadid, 0);
		if(res)
		ast_log(LOG_WARNING, "Can't kill monitor thread");
		ast_mutex_unlock(&t->lock);
		return -1;
	}

	if(t->fd > 0)
		serial_io(t->fd, ledpat, NULL, strlen(ledpat) ,0, 0, 0); /* LED back to flashing */

	/* Free linked lists */

	if(t->pinhead){
		p = t->pinhead;
		while(p){
			pn = p->next;
			ast_free(p);
			p = pn;
		}
		t->pinhead = NULL;
	}


	if(t->txhead){
		q = t->txhead;
		while(q){
			qn = q->next;
			ast_free(q);
			q = qn;
		}
		t->txhead = t->txtail = NULL;
	}
	
	if(t->fd > 0){	
		res = close(t->fd);
		if(res)
			ast_log(LOG_WARNING, "Error closing serial port");
		t->fd = -1;
	}
	ast_mutex_unlock(&t->lock);
	ast_mutex_destroy(&t->lock);
	return res;
}

/*
 * Uchameleon generic interface which supports monitor thread
 */

static int uchameleon_do_long( struct daq_entry_tag *t, int pin,
int cmd, void (*exec)(struct daq_pin_entry_tag *), int *arg1, void *arg2)
{	
	int i,j,x;
	struct daq_pin_entry_tag *p, *listl, *listp;

	if(!t)
		return -1;

	ast_mutex_lock(&t->lock);

	if(!t->active){
		/* Try to restart thread and re-open device */
		ast_mutex_unlock(&t->lock);
		uchameleon_close(t);
		usleep(10*1000);
		if(uchameleon_open(t)){
			ast_log(LOG_WARNING,"Could not re-open Uchameleon\n");
			return -1;
		}
		ast_mutex_lock(&t->lock);
		/* We're back in business! */
	}


	/* Find our pin */

	listp = listl = t->pinhead;
	while(listp){
		listl = listp;
		if(listp->num == pin)
			break;
		listp = listp->next;
	}
	if(listp){
		if(cmd == DAQ_CMD_PINSET){
			if(arg1 && *arg1 && (*arg1 < 19)){
				while(listp->state){
					ast_mutex_unlock(&t->lock);
					usleep(10*1000); /* Wait */
					ast_mutex_lock(&t->lock);
				}
				listp->command = DAQ_CMD_PINSET;
				listp->pintype = *arg1; /* Pin redefinition */
				listp->valuemin = 255;
				listp->valuemax = 0;
				listp->state = DAQ_PS_START;
			}
			else{
				ast_log(LOG_WARNING,"Invalid pin number for pinset\n");
			}
		}
		else{
			/* Return ADC value */

			if(cmd == DAQ_CMD_ADC){
				if(arg2){
					switch(*((int *) arg2)){
						case DAQ_SUB_CUR:
							if(arg1)
								*arg1 = listp->value;
							break;

						case DAQ_SUB_STAVG: /* Short term average */
							x = 0;
							i = listp->adcnextupdate;
							for(j = 0 ; j < ADC_HISTORY_DEPTH; j++){
								if(debug >= 4){
									ast_log(LOG_NOTICE, "Sample for avg: %d\n",
									listp->adchistory[i]);
								}
								x += listp->adchistory[i];
								if(++i >= ADC_HISTORY_DEPTH)
									i = 0;
							}
							x /= ADC_HISTORY_DEPTH;
							if(debug >= 3)
								ast_log(LOG_NOTICE, "Average: %d\n", x);
							if(arg1)
								*arg1 = x;
							break;

						case DAQ_SUB_STMAX: /* Short term maximum */
							x = 0;
							i = listp->adcnextupdate;
							for(j = 0 ; j < ADC_HISTORY_DEPTH; j++){
								if(debug >= 4){
									ast_log(LOG_NOTICE, "Sample for max: %d\n",
									listp->adchistory[i]);
								}
								if(listp->adchistory[i] > x)
									x = listp->adchistory[i];
								if(++i >= ADC_HISTORY_DEPTH)
									i = 0;
							}
							if(debug >= 3)
								ast_log(LOG_NOTICE, "Maximum: %d\n", x);
							if(arg1)
								*arg1 = x;
							break;

						case DAQ_SUB_STMIN: /* Short term minimum */
							x = 255 ;
							i = listp->adcnextupdate;
							if(i >= ADC_HISTORY_DEPTH)
								i = 0;
							for(j = 0 ; j < ADC_HISTORY_DEPTH; j++){
								if(debug >= 4){
									ast_log(LOG_NOTICE, "Sample for min: %d\n",
									listp->adchistory[i]);
								}
								if(listp->adchistory[i] < x)
									x = listp->adchistory[i];
								if(++i >= ADC_HISTORY_DEPTH)
									i = 0;
								}
							if(debug >= 3)
								ast_log(LOG_NOTICE, "Minimum: %d\n", x);
							if(arg1)
								*arg1 = x;
							break;

						case DAQ_SUB_MAX: /* Max since start or reset */
							if(arg1)
								*arg1 = listp->valuemax;
							break;

						case DAQ_SUB_MIN: /* Min since start or reset */
							if(arg1)
								*arg1 = listp->valuemin;
							break;

						default:
							ast_mutex_unlock(&t->lock);
							return -1;
					}
				}
				else{
					if(arg1)
						*arg1 = listp->value;
				}
				ast_mutex_unlock(&t->lock);	
				return 0;
			}

			/* Don't deadlock if monitor has been previously issued for a pin */

			if(listp->state == DAQ_PS_IN_MONITOR){
				if((cmd != DAQ_CMD_MONITOR) || (exec)){
					ast_log(LOG_WARNING,
						"Monitor was previously set on pin %d, command ignored\n",listp->num);
					ast_mutex_unlock(&t->lock);
					return -1;
				}
			}

			/* Rest of commands are processed here */

			while(listp->state){
				ast_mutex_unlock(&t->lock);
				usleep(10*1000); /* Wait */
				ast_mutex_lock(&t->lock);
			}

			if(cmd == DAQ_CMD_MONITOR){
				if(arg1)
					listp->ignorefirstalarm = *arg1;
				listp->monexec = exec;
			}

			listp->command = cmd;

			if(cmd == DAQ_CMD_OUT){
				if(arg1){
					listp->value = *arg1;
				}
				else{
					ast_mutex_unlock(&t->lock);
					return 0;
				}
			}
			listp->state = DAQ_PS_START;
			if((cmd == DAQ_CMD_OUT)||(cmd == DAQ_CMD_MONITOR)){
				ast_mutex_unlock(&t->lock);
				return 0;
			}

 			while(listp->state){
				ast_mutex_unlock(&t->lock);
				usleep(10*1000); /* Wait */
				ast_mutex_lock(&t->lock);
			}
			*arg1 = listp->value;
			ast_mutex_unlock(&t->lock);
			return 0;
		}
	}
	else{ /* Pin not in list */
		if(cmd == DAQ_CMD_PINSET){
			if(arg1 && *arg1 && (*arg1 < 19)){
				/* New pin definition */
				if(!(p = (struct daq_pin_entry_tag *) malloc(sizeof(struct daq_pin_entry_tag)))){
					ast_log(LOG_ERROR,"Out of memory");
					ast_mutex_unlock(&t->lock);
					return -1;
				}
				memset(p, 0, sizeof(struct daq_pin_entry_tag));
				p->pintype = *arg1;
				p->command = DAQ_CMD_PINSET;
				p->num = pin;
				if(!listl){
					t->pinhead = p;
				}
				else{
					listl->next = p; 
				}
				p->state = DAQ_PS_START;
				ast_mutex_unlock(&t->lock);
				return 0;
			}
			else{
				ast_log(LOG_WARNING,"Invalid pin number for pinset\n");
			}
		}
		else{
			ast_log(LOG_WARNING,"Invalid pin number for pin I/O command\n");
		}
	}
	ast_mutex_unlock(&t->lock);
	return -1;
}
 
/*
 * Reset a minimum or maximum reading
 */

static int uchameleon_reset_minmax(struct daq_entry_tag *t, int pin, int minmax)
{
	struct daq_pin_entry_tag *p;

	/* Find the pin */
	p = t->pinhead;
	while(p){
		if(p->num == pin)
			break;
		p = p->next;
	}
	if(!p)
		return -1;
	ast_mutex_lock(&t->lock);
	if(minmax){
		ast_log(LOG_NOTICE, "Resetting maximum on device %s, pin %d\n",t->name, pin);
		p->valuemax = 0;
	}
	else{
		p->valuemin = 255;
		ast_log(LOG_NOTICE, "Resetting minimum on device %s, pin %d\n",t->name, pin);
	}
	ast_mutex_unlock(&t->lock);
	return 0;
}




/*
 * Queue up a tx command (used exclusively by uchameleon_monitor() )
 */

static void uchameleon_queue_tx(struct daq_entry_tag *t, char *txbuff)
{
	struct daq_tx_entry_tag *q;

	if(!t)
		return;
		
	if(!(q = (struct daq_tx_entry_tag *) ast_malloc(sizeof(struct daq_tx_entry_tag)))){
		ast_log(LOG_WARNING, "Out of memory\n");
		return;
	}

	memset(q, 0, sizeof(struct daq_tx_entry_tag));

	strncpy(q->txbuff, txbuff, 32);
	q->txbuff[31] = 0;

	if(t->txtail){
		t->txtail->next = q;
		q->prev = t->txtail;
		t->txtail = q;
	}
	else
		t->txhead = t->txtail = q;
	return;
}


/*
 * Monitor thread for Uchameleon devices
 *
 * started by uchameleon_open() and shutdown by uchameleon_close()
 *
 */
static void *uchameleon_monitor_thread(void *this)
{
	int pin = 0, sample = 0;
	int i,res,valid,adc_acquire;
	time_t now;
	char rxbuff[32];
	char txbuff[32];
	char *rxargs[4];
	struct daq_entry_tag *t = (struct daq_entry_tag *) this;
	struct daq_pin_entry_tag *p;
	struct daq_tx_entry_tag *q;



	if(debug)
		ast_log(LOG_NOTICE, "DAQ: thread started\n");

	ast_mutex_lock(&t->lock);
	t->active = 1;
	ast_mutex_unlock(&t->lock);

	for(;;){
		adc_acquire = 0;
		 /* If receive data */
		res = serial_rx(t->fd, rxbuff, sizeof(rxbuff), DAQ_RX_TIMEOUT, 0x0a);
		if(res == -1){
			ast_log(LOG_ERROR,"serial_rx failed\n");
			close(t->fd);
			ast_mutex_lock(&t->lock);
			t->fd = -1;
			t->active = 0;
			ast_mutex_unlock(&t->lock);
			return this; /* Now, we die */
		}
		if(res){
			if(debug >= 5) 
				ast_log(LOG_NOTICE, "Received: %s\n", rxbuff);
			valid = 0;
			/* Parse return string */
			i = explode_string(rxbuff, rxargs, 3, ' ', 0);
			if(i == 3){
				if(!strcmp(rxargs[0],"pin")){
					valid = 1;
					pin = atoi(rxargs[1]);
					sample = atoi(rxargs[2]);
				}
				if(!strcmp(rxargs[0],"adc")){
					valid = 2;
					pin = atoi(rxargs[1]);
					sample = atoi(rxargs[2]);
				}
			}
			if(valid){
				/* Update the correct pin list entry */
				ast_mutex_lock(&t->lock);
				p = t->pinhead;
				while(p){
					if(p->num == pin){
						if((valid == 1)&&((p->pintype == DAQ_PT_IN)||
							(p->pintype == DAQ_PT_INP)||(p->pintype == DAQ_PT_OUT))){
							p->value = sample ? 1 : 0;
							if(debug >= 3)
								ast_log(LOG_NOTICE,"Input pin %d is a %d\n",
									p->num, p->value);
							/* Exec monitor fun if state is monitor */

							if(p->state == DAQ_PS_IN_MONITOR){
								if(!p->alarmmask && !p->ignorefirstalarm && p->monexec){
									(*p->monexec)(p);
								}
								p->ignorefirstalarm = 0;
							}
							else
								p->state = DAQ_PS_IDLE;
						}
						if((valid == 2)&&(p->pintype == DAQ_PT_INADC)){
							p->value = sample;
							if(sample > p->valuemax)
								p->valuemax = sample;
							if(sample < p->valuemin)
								p->valuemin = sample;
							p->adchistory[p->adcnextupdate++] = sample;
							if(p->adcnextupdate >= ADC_HISTORY_DEPTH)
								p->adcnextupdate = 0;
							p->state = DAQ_PS_IDLE;
						}
						break;
					}
					p = p->next;
				}	 
				ast_mutex_unlock(&t->lock);
			}
		}
		

		if(time(&now) >= t->adcacqtime){
			t->adcacqtime = now + DAQ_ADC_ACQINT;
			if(debug >= 4)
				ast_log(LOG_NOTICE,"Acquiring analog data\n");
			adc_acquire = 1;
		}

		/* Go through the pin linked list looking for new work */
		ast_mutex_lock(&t->lock);		
		p = t->pinhead;
		while(p){
			/* Time to acquire all ADC channels ? */
			if((adc_acquire) && (p->pintype == DAQ_PT_INADC)){
				p->state = DAQ_PS_START;
				p->command = DAQ_CMD_ADC;
			}
			if(p->state == DAQ_PS_START){
				p->state = DAQ_PS_BUSY; /* Assume we are busy */
				switch(p->command){
					case DAQ_CMD_OUT:
						if(p->pintype == DAQ_PT_OUT){
							snprintf(txbuff,sizeof(txbuff),"pin %d %s\n", p->num, (p->value) ?
							"hi" : "lo");
							if(debug >= 3)
								ast_log(LOG_NOTICE, "DAQ_CMD_OUT: %s\n", txbuff);
							uchameleon_queue_tx(t, txbuff);
							p->state = DAQ_PS_IDLE; /* TX is considered done */ 
						}
						else{
							ast_log(LOG_WARNING,"Wrong pin type for out command\n");
							p->state = DAQ_PS_IDLE;
						}
						break;

					case DAQ_CMD_MONITOR:
						snprintf(txbuff, sizeof(txbuff), "pin %d monitor %s\n", 
						p->num, p->monexec ? "on" : "off");
						uchameleon_queue_tx(t, txbuff);
						if(!p->monexec)
							p->state = DAQ_PS_IDLE; /* Restore to idle channel */
						else{
							p->state = DAQ_PS_IN_MONITOR;
						}
						break;

					case DAQ_CMD_IN:
						if((p->pintype == DAQ_PT_IN)||
							(p->pintype == DAQ_PT_INP)||(p->pintype == DAQ_PT_OUT)){
							snprintf(txbuff,sizeof(txbuff),"pin %d state\n", p->num);
							uchameleon_queue_tx(t, txbuff);
						}
						else{
							ast_log(LOG_WARNING,"Wrong pin type for in or inp command\n");
							p->state = DAQ_PS_IDLE;
						}
						break;
					
					case DAQ_CMD_ADC:
						if(p->pintype == DAQ_PT_INADC){
							snprintf(txbuff,sizeof(txbuff),"adc %d\n", p->num);
							uchameleon_queue_tx(t, txbuff);
						}
						else{
							ast_log(LOG_WARNING,"Wrong pin type for adc command\n");
							p->state = DAQ_PS_IDLE;
						}
						break;

					case DAQ_CMD_PINSET:
						if((!p->num)||(p->num > 18)){
							ast_log(LOG_WARNING,"Invalid pin number %d\n", p->num);
							p->state = DAQ_PS_IDLE;
						}
						switch(p->pintype){
							case DAQ_PT_IN:
							case DAQ_PT_INADC:
							case DAQ_PT_INP:
								if((p->pintype == DAQ_PT_INADC) && (p->num > 8)){
									ast_log(LOG_WARNING,
									"Invalid ADC pin number %d\n", p->num);
									p->state = DAQ_PS_IDLE;
									break;
								}					
								if((p->pintype == DAQ_PT_INP) && (p->num < 9)){
									ast_log(LOG_WARNING,
									"Invalid INP pin number %d\n", p->num);
									p->state = DAQ_PS_IDLE;
									break;
								}
								snprintf(txbuff, sizeof(txbuff), "pin %d in\n", p->num);
								uchameleon_queue_tx(t, txbuff);
								if(p->num > 8){
									snprintf(txbuff, sizeof(txbuff),
									"pin %d pullup %d\n", p->num,
									(p->pintype == DAQ_PT_INP) ? 1 : 0);
									uchameleon_queue_tx(t, txbuff);
								}
								p->valuemin = 255;
								p->valuemax = 0;
								p->state = DAQ_PS_IDLE;
								break;

							case DAQ_PT_OUT:
                        					snprintf(txbuff, sizeof(txbuff), "pin %d out\n", p->num);
								uchameleon_queue_tx(t, txbuff);
								p->state = DAQ_PS_IDLE;
								break;

							default:
								break;
						}
						break;

					default:
						ast_log(LOG_WARNING,"Unrecognized uchameleon command\n");
						p->state = DAQ_PS_IDLE;
						break;
				} /* switch */
			} /* if */
		p = p->next;
		} /* while */
		
		/* Transmit queued commands */
		while(t->txhead){
			q = t->txhead;
			strncpy(txbuff,q->txbuff,sizeof(txbuff));
			txbuff[sizeof(txbuff)-1] = 0;
			t->txhead = q->next;
			if(t->txhead)
				t->txhead->prev = NULL;
			else
				t->txtail = NULL;
			ast_free(q);
			ast_mutex_unlock(&t->lock);
			if(serial_txstring(t->fd, txbuff) == -1){
				close(t->fd);
				ast_mutex_lock(&t->lock);
				t->active= 0;
				t->fd = -1;
				ast_mutex_unlock(&t->lock);
				ast_log(LOG_ERROR,"Tx failed, terminating monitor thread\n");
				return this; /* Now, we die */
			}
				
			ast_mutex_lock(&t->lock);
		}/* while */
		ast_mutex_unlock(&t->lock);
	} /* for(;;) */
	return this;
}


/*
 * **************************
 * Generic DAQ functions    *
 * **************************
 */

/*
 * Forward Decl's
 */

static int saynum(struct ast_channel *mychannel, int num);
static int sayfile(struct ast_channel *mychannel,char *fname);
static int wait_interval(struct rpt *myrpt, int type, struct ast_channel *chan);
static void rpt_telem_select(struct rpt *myrpt, int command_source, struct rpt_link *mylink);
static void rpt_telem_select(struct rpt *myrpt, int command_source, struct rpt_link *mylink);

/*
 * Open a daq device
 */

static struct daq_entry_tag *daq_open(int type, char *name, char *dev)
{
	int fd;
	struct daq_entry_tag *t;


	if(!name)
		return NULL;

        if((t = ast_malloc(sizeof(struct daq_entry_tag))) == NULL){
		ast_log(LOG_WARNING,"daq_open out of memory\n");
		return NULL;
	}


	memset(t, 0, sizeof(struct daq_entry_tag));


	/* Save the device path for open*/
	if(dev){
		strncpy(t->dev, dev, MAX_DAQ_DEV);
		t->dev[MAX_DAQ_DEV - 1] = 0;
	}



	/* Save the name*/
	strncpy(t->name, name, MAX_DAQ_NAME);
	t->dev[MAX_DAQ_NAME - 1] = 0;


	switch(type){
		case DAQ_TYPE_UCHAMELEON:
			if((fd = uchameleon_open(t)) == -1){
				ast_free(t);
				return NULL;
			}
			break;

		default:
			ast_free(t);
			return NULL;
	}
	t->type = type;
	return t;
}

/*
 * Close a daq device
 */


static int daq_close(struct daq_entry_tag *t)
{
	int res  = -1;

	if(!t)
		return res;

	switch(t->type){
		case DAQ_TYPE_UCHAMELEON:
			res = uchameleon_close(t);
			break;
		default:
			break;
	}

	ast_free(t);
	return res;
}

/*
 * Look up a device entry for a particular device name
 */

static struct daq_entry_tag *daq_devtoentry(char *name)
{
	struct daq_entry_tag *e = daq.hw;

	while(e){
		if(!strcmp(name, e->name))
			break; 
		e = e->next;
	}
	return e;
}



/*
 * Do something with the daq subsystem
 */

static int daq_do_long( struct daq_entry_tag *t, int pin, int cmd,
void (*exec)(struct daq_pin_entry_tag *), int *arg1, void *arg2)
{
	int res = -1;

	switch(t->type){
		case DAQ_TYPE_UCHAMELEON:
			res = uchameleon_do_long(t, pin, cmd, exec, arg1, arg2);
			break;
		default:
			break;
	}
	return res;
}

/*
 * Short version of above
 */

static int daq_do( struct daq_entry_tag *t, int pin, int cmd, int arg1)
{
	int a1 = arg1;

	return daq_do_long(t, pin, cmd, NULL, &a1, NULL);
}


/*
 * Function to reset the long term minimum or maximum
 */

static int daq_reset_minmax(char *device, int pin, int minmax)
{
	int res = -1;
	struct daq_entry_tag *t;
	
	if(!(t = daq_devtoentry(device)))
		return -1;
	switch(t->type){
		case DAQ_TYPE_UCHAMELEON:
			res = uchameleon_reset_minmax(t, pin, minmax);
			break;
		default:
			break;
	}
	return res;
}

/*
 * Initialize DAQ subsystem
 */

static void daq_init(struct ast_config *cfg)
{
	struct ast_variable *var;
	struct daq_entry_tag **t_next, *t = NULL;
	char s[64];
	daq.ndaqs = 0;
	t_next = &daq.hw;
	var = ast_variable_browse(cfg,"daq-list");
	while(var){
		char *p;
		if(strncmp("device",var->name,6)){
			ast_log(LOG_WARNING,"Error in daq_entries stanza on line %d\n", var->lineno);
			break;
		}
		strncpy(s,var->value,sizeof(s)); /* Make copy of device entry */
		if(!(p = (char *) ast_variable_retrieve(cfg,s,"hwtype"))){
			ast_log(LOG_WARNING,"hwtype variable required for %s stanza\n", s);
			break;
		}
		if(strncmp(p,"uchameleon",10)){
			ast_log(LOG_WARNING,"Type must be uchameleon for %s stanza\n", s);
			break;
		}
                if(!(p = (char *) ast_variable_retrieve(cfg,s,"devnode"))){
                        ast_log(LOG_WARNING,"devnode variable required for %s stanza\n", s);
                        break;
                }
		if(!(t = daq_open(DAQ_TYPE_UCHAMELEON, (char *) s, (char *) p))){
			ast_log(LOG_WARNING,"Cannot open device name %s\n",p);
			break;
		}
		/* Add to linked list */
		*t_next = t;
		t_next = &t->next;

		daq.ndaqs++;	
		if(daq.ndaqs >= MAX_DAQ_ENTRIES)
			break;
		var = var->next;
	}


}		

/*
 * Uninitialize DAQ Subsystem
 */

static void daq_uninit(void)
{
	struct daq_entry_tag *t_next, *t;

	/* Free daq memory */
	t = daq.hw;
	while(t){
		t_next = t->next;
		daq_close(t);
		t = t_next;
	}
	daq.hw = NULL;
}


/*
 * Parse a request METER request for telemetry thread
 * This is passed in a comma separated list of items from the function table entry
 * There should be 3 or 4 fields in the function table entry: device, channel, meter face, and  optionally: filter
 */


static int handle_meter_tele(struct rpt *myrpt, struct ast_channel *mychannel, char *args)
{
	int i,res,files,filter,val;
	int pin = 0;
	int pintype = 0;
	int device = 0;
	int metertype = 0;
	int numranges = 0;
	int filtertype = 0;
	int rangemin,rangemax;
	float scaledval = 0.0, scalepre = 0.0, scalepost = 0.0, scalediv = 1.0, valtoround;
	char *myargs,*meter_face;
	const char *p;
	char *start, *end;
	char *sounds = NULL;
	char *rangephrase = NULL;
	char *argv[5];
	char *sound_files[MAX_METER_FILES+1];
	char *range_strings[MAX_DAQ_RANGES+1];
	char *bitphrases[3];	
	static char *filter_keywords[]={"none","max","min","stmin","stmax","stavg",NULL};
	struct daq_entry_tag *entry;
	
	if(!(myargs = ast_strdup(args))){ /* Make a local copy to slice and dice */
		ast_log(LOG_WARNING, "Out of memory\n");
		return -1;
	}
	
	i = explode_string(myargs, argv, 4, ',', 0);
	if((i != 4) && (i != 3)){ /* Must have 3 or 4 substrings, no more, no less */
		ast_log(LOG_WARNING,"Wrong number of arguments for meter telemetry function is: %d s/b 3 or 4", i);
		ast_free(myargs);
		return -1;
	}
	if(debug >= 3){
		ast_log(LOG_NOTICE,"Device: %s, Pin: %s, Meter Face: %s Filter: %s\n",
		argv[0],argv[1],argv[2], argv[3]);	
	}

	if(i == 4){
		filter = matchkeyword(argv[3], NULL, filter_keywords);
		if(!filter){
			ast_log(LOG_WARNING,"Unsupported filter type: %s\n",argv[3]);
			ast_free(myargs);
			return -1;
		}
		filter--;
	}
	else
		filter = DAQ_SUB_CUR;	
	
	/* Find our device */
	if(!(entry = daq_devtoentry(argv[0]))){
		ast_log(LOG_WARNING,"Cannot find device %s in daq-list\n",argv[0]);
		ast_free(myargs);
		return -1;
	}

	/* Check for compatible pin type */
	if(!(p = ast_variable_retrieve(myrpt->cfg,argv[0],argv[1]))){
		ast_log(LOG_WARNING,"Channel %s not defined for %s\n", argv[1], argv[0]);
		ast_free(myargs);
		return -1;
	}

	if(!strcmp("inadc",p))
		pintype = 1;
	if((!strcmp("inp",p))||(!strcmp("in",p)||(!strcmp("out", p))))
		pintype = 2;
	if(!pintype){
		ast_log(LOG_WARNING,"Pin type must be one of inadc, inp, in, or out for channel %s\n",argv[1]);
		ast_free(myargs);
		return -1;
	}
	if(debug >= 3)
		ast_log(LOG_NOTICE,"Pintype = %d\n",pintype);

	pin = atoi(argv[1]);

	/*
 	Look up and parse the meter face

	[meter-faces]
	batvolts=scale(0,12.8,0),thevoltage,is,volts
	winddir=range(0-33:north,34-96:west,97-160:south,161-224:east,225-255:north),thewindis,?
	door=bit(closed,open),thedooris,?

	*/

	if(!(p = ast_variable_retrieve(myrpt->cfg,"meter-faces", argv[2]))){
		ast_log(LOG_WARNING,"Meter face %s not found", argv[2]);
		ast_free(myargs);		
		return -1;
	}

	if(!(meter_face = ast_strdup(p))){
		ast_log(LOG_WARNING,"Out of memory");
		ast_free(myargs);
		return -1;
	}
	
	if(!strncmp("scale", meter_face, 5)){ /* scale function? */
		metertype = 1;
		if((!(end = strchr(meter_face,')')))||
			(!(start = strchr(meter_face, '(')))||
			(!end[1])||(!end[2])||(end[1] != ',')){ /* Properly formed? */
			ast_log(LOG_WARNING,"Syntax error in meter face %s\n", argv[2]);
			ast_free(myargs);
			ast_free(meter_face);
			return -1;
		}
		*start++ = 0; /* Points to comma delimited scaling values */
		*end = 0;
		sounds = end + 2; /* Start of sounds part */
		if(sscanf(start,"%f,%f,%f",&scalepre, &scalediv, &scalepost) != 3){
			ast_log(LOG_WARNING,"Scale must have 3 args in meter face %s\n", argv[2]);
			ast_free(myargs);
			ast_free(meter_face);
			return -1;
		}
		if(scalediv < 1.0){
			ast_log(LOG_WARNING,"scalediv must be >= 1\n");
			ast_free(myargs);
			ast_free(meter_face);
			return -1;

		}		
	}
	else if(!strncmp("range", meter_face, 5)){ /* range function */
		metertype = 2;
		if((!(end = strchr(meter_face,')')))||
			(!(start = strchr(meter_face, '(')))||
			(!end[1])||(!end[2])||(end[1] != ',')){ /* Properly formed? */
			ast_log(LOG_WARNING,"Syntax error in meter face %s\n", argv[2]);
			ast_free(myargs);
			ast_free(meter_face);
			return -1;
		}
		*start++ = 0;
		*end = 0;
		sounds = end + 2; 
		/*
 		* Parse range entries
 		*/
		if((numranges = explode_string(start, range_strings, MAX_DAQ_RANGES, ',', 0)) < 2 ){
			ast_log(LOG_WARNING, "At least 2 ranges required for range() in meter face %s\n", argv[2]);
			ast_free(myargs);
			ast_free(meter_face);
			return -1;
		}

	}
	else if(!strncmp("bit", meter_face, 3)){ /* bit function */
		metertype = 3;
		if((!(end = strchr(meter_face,')')))||
			(!(start = strchr(meter_face, '(')))||
			(!end[1])||(!end[2])||(end[1] != ',')){ /* Properly formed? */
			ast_log(LOG_WARNING,"Syntax error in meter face %s\n", argv[2]);
			ast_free(myargs);
			ast_free(meter_face);
			return -1;
		}
		*start++ = 0;
		*end = 0;
		sounds = end + 2;
		if(2 != explode_string(start, bitphrases, 2, ',', 0)){
			ast_log(LOG_WARNING, "2 phrases required for bit() in meter face %s\n", argv[2]);
			ast_free(myargs);
			ast_free(meter_face);
			return -1;
		}		
	}
	else{
		ast_log(LOG_WARNING,"Meter face %s needs to specify one of scale, range or bit\n", argv[2]);
		ast_free(myargs);
		ast_free(meter_face);
		return -1;
	}

	/*
 	* Acquire 
 	*/

	val = 0;
	if(pintype == 1){
		res = daq_do_long(entry, pin, DAQ_CMD_ADC, NULL, &val, &filter);
		if(!res)
			scaledval = ((val + scalepre)/scalediv) + scalepost;
	}
	else{
		res = daq_do_long(entry, pin, DAQ_CMD_IN, NULL, &val, NULL);
	}

	if(res){ /* DAQ Subsystem is down */
		ast_free(myargs);
		ast_free(meter_face);
		return res;
	}

	/*
 	* Select Range
 	*/

	if(metertype == 2){
		for(i = 0; i < numranges; i++){
			if(2 != sscanf(range_strings[i],"%u-%u:", &rangemin, &rangemax)){
				ast_log(LOG_WARNING,"Range variable error on meter face %s\n", argv[2]);
				ast_free(myargs);
				ast_free(meter_face);
				return -1;
			}
			if((!(rangephrase = strchr(range_strings[i],':')) || (!rangephrase[1]))){
				ast_log(LOG_WARNING,"Range phrase missing on meter face %s\n", argv[2]);
				ast_free(myargs);
				ast_free(meter_face);
				return -1;
			}
			rangephrase++;
			if((val >= rangemin) && (val <= rangemax))
				break;
		}
		if(i == numranges){
			ast_log(LOG_WARNING,"Range missing on meter face %s for value %d\n", argv[2], val);
			ast_free(myargs);
			ast_free(meter_face);
			return -1;
		}
	}

	if(debug >= 3){ /* Spew the variables */
		ast_log(LOG_NOTICE,"device = %d, pin = %d, pintype = %d, metertype = %d\n",device, pin, pintype, metertype);
		ast_log(LOG_NOTICE,"raw value = %d\n", val);
		if(metertype == 1){
			ast_log(LOG_NOTICE,"scalepre = %f, scalediv = %f, scalepost = %f\n",scalepre, scalediv, scalepost);
			ast_log(LOG_NOTICE,"scaled value = %f\n", scaledval);
		}
		if(metertype == 2){
			ast_log(LOG_NOTICE,"Range phrase is: %s for meter face %s\n", rangephrase, argv[2]);
		ast_log(LOG_NOTICE,"filtertype = %d\n", filtertype);
		}
		ast_log(LOG_NOTICE,"sounds = %s\n", sounds);

 	}
	
	/* Wait the normal telemetry delay time */
	
	if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) goto done;
	

	/* Split up the sounds string */
	
	files = explode_string(sounds, sound_files, MAX_METER_FILES, ',', 0);
	if(files == 0){
		ast_log(LOG_WARNING,"No sound files to say for meter %s\n",argv[2]);
		ast_free(myargs);
		ast_free(meter_face);
		return -1;
	}
	/* Say the files one by one acting specially on the ? character */
	res = 0;
	for(i = 0; i < files && !res; i++){
		if(sound_files[i][0] == '?'){ /* Insert sample */
			if(metertype == 1){
				int integer, decimal, precision = 0;
				if((scalediv >= 10) && (scalediv < 100)) /* Adjust precision of decimal places */
					precision = 10; 
				else if(scalediv >= 100)
					precision = 100;
				integer = (int) scaledval;
				valtoround = ((scaledval - integer) * precision);
				 /* grrr.. inline lroundf doesn't work with uClibc! */
				decimal = (int) ((valtoround + ((valtoround >= 0) ? 0.5 : -0.5)));
				if((precision) && (decimal == precision)){
					decimal = 0;
					integer++;
				}
				if(debug)
					ast_log(LOG_NOTICE,"integer = %d, decimal = %d\n", integer, decimal);
				res = saynum(mychannel, integer);
				if(!res && precision && decimal){
					res = sayfile(mychannel,"point");
					if(!res)
						res = saynum(mychannel, decimal);
				}
			}
			if(metertype == 2){
				res = sayfile(mychannel, rangephrase);
			}
			if(metertype == 3){
				res = sayfile(mychannel, bitphrases[(val) ? 1: 0]);
			}
	
		}
		else{
			res = sayfile(mychannel, sound_files[i]); /* Say the next word in the list */
		}					
	}
done: 	
	/* Done */
	ast_free(myargs);
	ast_free(meter_face);
	return 0;
}

/*
 * Handle USEROUT telemetry
 */

static int handle_userout_tele(struct rpt *myrpt, struct ast_channel *mychannel, char *args)
{
	int argc, i, pin, reqstate, res;
	char *myargs;
	char *argv[11];
	struct daq_entry_tag *t;

	if(!(myargs = ast_strdup(args))){ /* Make a local copy to slice and dice */
		ast_log(LOG_WARNING, "Out of memory\n");
		return -1;
	}

	if(debug >= 3)
		ast_log(LOG_NOTICE, "String: %s\n", myargs);

	argc = explode_string(myargs, argv, 10, ',', 0);
	if(argc < 4){ /* Must have at least 4 arguments */
		ast_log(LOG_WARNING,"Incorrect number of arguments for USEROUT function");
		ast_free(myargs);
		return -1;
	}
	if(debug >= 3){
		ast_log(LOG_NOTICE,"USEROUT Device: %s, Pin: %s, Requested state: %s\n",
		argv[0],argv[1],argv[2]);	
	}
	pin = atoi(argv[1]);
	reqstate = atoi(argv[2]);

	/* Find our device */
	if(!(t = daq_devtoentry(argv[0]))){
		ast_log(LOG_WARNING,"Cannot find device %s in daq-list\n",argv[0]);
		ast_free(myargs);
		return -1;
	}

	if(debug >= 3){
		ast_log(LOG_NOTICE, "Output to pin %d a value of %d with argc = %d\n", pin, reqstate, argc);
	}

	/* Set or reset the bit */

	res = daq_do( t, pin, DAQ_CMD_OUT, reqstate);
	
	/* Wait the normal telemetry delay time */
	
	if(!res)
		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) goto done;

	/* Say the files one by one at argc index 3 */
	for(i = 3; i < argc && !res; i++){
		res = sayfile(mychannel, argv[i]); /* Say the next word in the list */
	}					
	
done:
	ast_free(myargs);
	return 0;
}


/*
*  Playback a meter reading
*/

static int function_meter(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink)
{

	if (myrpt->remote)
		return DC_ERROR;

	if(debug)
		ast_log(LOG_NOTICE, "meter param = %s, digitbuf = %s\n", (param)? param : "(null)", digitbuf);
	
	rpt_telem_select(myrpt,command_source,mylink);
	rpt_telemetry(myrpt,METER,param);
	return DC_COMPLETE;
}



/*
*  Set or reset a USER Output bit
*/

static int function_userout(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink)
{

	if (myrpt->remote)
		return DC_ERROR;

		ast_log(LOG_NOTICE, "userout param = %s, digitbuf = %s\n", (param)? param : "(null)", digitbuf);
	
	rpt_telem_select(myrpt,command_source,mylink);
	rpt_telemetry(myrpt,USEROUT,param);
	return DC_COMPLETE;
}


/*
*  Execute shell command
*/

static int function_cmd(struct rpt *myrpt, char *param, char *digitbuf, int command_source, struct rpt_link *mylink)
{
	char *cp;

	if (myrpt->remote)
		return DC_ERROR;

	ast_log(LOG_NOTICE, "cmd param = %s, digitbuf = %s\n", (param)? param : "(null)", digitbuf);
	
	if (param) {
		if (*param == '#') /* to execute asterisk cli command */
		{
			ast_cli_command(nullfd,param + 1);
		}
		else
		{			
			cp = ast_malloc(strlen(param) + 10);
			if (!cp)
			{
				ast_log(LOG_NOTICE,"Unable to alloc");
				return DC_ERROR;
			}
			memset(cp,0,strlen(param) + 10);
			sprintf(cp,"%s &",param);
			ast_safe_system(cp);
			free(cp);
		}
	}
	return DC_COMPLETE;
}


/*
 ********************** 
* End of DAQ functions*
* *********************
*/


static long diskavail(struct rpt *myrpt)
{
struct	statfs statfsbuf;

	if (!myrpt->p.archivedir) return(0);
	if (statfs(myrpt->p.archivedir,&statfsbuf) == -1)
	{
		ast_log(LOG_WARNING,"Cannot get filesystem size for %s node %s\n",
			myrpt->p.archivedir,myrpt->name);
		return(-1);
	}
	return(statfsbuf.f_bavail);
}

static void flush_telem(struct rpt *myrpt)
{
	struct rpt_tele *telem;
	if(debug > 2)
		ast_log(LOG_NOTICE, "flush_telem()!!");
	rpt_mutex_lock(&myrpt->lock);
	telem = myrpt->tele.next;
	while(telem != &myrpt->tele)
	{
		if (telem->mode != SETREMOTE) ast_softhangup(telem->chan,AST_SOFTHANGUP_DEV);
		telem = telem->next;
	}
	rpt_mutex_unlock(&myrpt->lock);
}
/*
	return via error priority
*/
static int priority_jump(struct rpt *myrpt, struct ast_channel *chan)
{
	int res=0;

	// if (ast_test_flag(&flags,OPT_JUMP) && ast_goto_if_exists(chan, chan->context, chan->exten, chan->priority + 101) == 0){
	if (ast_goto_if_exists(chan, chan->context, chan->exten, chan->priority + 101) == 0){
		res = 0;
	} else {
		res = -1;
	}
	return res;
}
/*
*/

static void init_linkmode(struct rpt *myrpt, struct rpt_link *mylink, int linktype)
{

	if (!myrpt) return;
	if (!mylink) return;
	switch(myrpt->p.linkmode[linktype])
	{
	    case LINKMODE_OFF:
		mylink->linkmode = 0;
		break;
	    case LINKMODE_ON:
		mylink->linkmode = 0x7fffffff;
		break;
	    case LINKMODE_FOLLOW:
		mylink->linkmode = 0x7ffffffe;
		break;
	    case LINKMODE_DEMAND:
		mylink->linkmode = 1;
		break;
	}
	return;
}

static void set_linkmode(struct rpt_link *mylink, int linkmode)
{

	if (!mylink) return;
 	switch(linkmode)
	{
	    case LINKMODE_OFF:
		mylink->linkmode = 0;
		break;
	    case LINKMODE_ON:
		mylink->linkmode = 0x7fffffff;
		break;
	    case LINKMODE_FOLLOW:
		mylink->linkmode = 0x7ffffffe;
		break;
	    case LINKMODE_DEMAND:
		mylink->linkmode = 1;
		break;
	}
	return;
}

static void rpt_telem_select(struct rpt *myrpt, int command_source, struct rpt_link *mylink)
{
int	src;

	if (mylink && mylink->chan)
	{
		src = LINKMODE_GUI;
		if (mylink->phonemode) src = LINKMODE_PHONE;
		else if (!strncasecmp(mylink->chan->name,"echolink",8)) src = LINKMODE_ECHOLINK;
		else if (!strncasecmp(mylink->chan->name,"tlb",8)) src = LINKMODE_TLB;
		if (myrpt->p.linkmodedynamic[src] && (mylink->linkmode >= 1) && 
		    (mylink->linkmode < 0x7ffffffe))
				mylink->linkmode = LINK_HANG_TIME;
	}
	if (!myrpt->p.telemdynamic) return;
	if (myrpt->telemmode == 0) return;
	if (myrpt->telemmode == 0x7fffffff) return;
	myrpt->telemmode = TELEM_HANG_TIME;
	return;
}

static int altlink(struct rpt *myrpt,struct rpt_link *mylink)
{
	if (!myrpt) return(0);
	if (!mylink) return(0);
	if (!mylink->chan) return(0);
	if ((myrpt->p.duplex == 3) && mylink->phonemode && myrpt->keyed) return(0);
	/* if doesnt qual as a foreign link */
	if ((mylink->name[0] > '0') && (mylink->name[0] <= '9') &&
	    (!mylink->phonemode) &&
	    strncasecmp(mylink->chan->name,"echolink",8) &&
		strncasecmp(mylink->chan->name,"tlb",3)) return(0);
	if ((myrpt->p.duplex < 2) && (myrpt->tele.next == &myrpt->tele)) return(0);
	if (mylink->linkmode < 2) return(0);
	if (mylink->linkmode == 0x7fffffff) return(1);
	if (mylink->linkmode < 0x7ffffffe) return(1);
	if (myrpt->telemmode > 1) return(1);
	return(0);
}

static int altlink1(struct rpt *myrpt,struct rpt_link *mylink)
{
struct  rpt_tele *tlist;
int	nonlocals;

	if (!myrpt) return(0);
	if (!mylink) return(0);
	if (!mylink->chan) return(0);
	nonlocals = 0;
	tlist = myrpt->tele.next;
        if (tlist != &myrpt->tele)
        {
                while(tlist != &myrpt->tele)
		{
                        if ((tlist->mode == PLAYBACK) || 
			    (tlist->mode == STATS_GPS_LEGACY) ||
			      (tlist->mode == ID1) || 
				(tlist->mode == TEST_TONE)) nonlocals++;
			tlist = tlist->next;
		}
	}
	if ((!myrpt->p.duplex) || (!nonlocals)) return(0);
	/* if doesnt qual as a foreign link */
	if ((mylink->name[0] > '0') && (mylink->name[0] <= '9') &&
	    (!mylink->phonemode) &&
	    strncasecmp(mylink->chan->name,"echolink",8) &&
		strncasecmp(mylink->chan->name,"tlb",3)) return(1);
	if (mylink->linkmode < 2) return(0);
	if (mylink->linkmode == 0x7fffffff) return(1);
	if (mylink->linkmode < 0x7ffffffe) return(1);
	if (myrpt->telemmode > 1) return(1);
	return(0);
}

static void rpt_qwrite(struct rpt_link *l,struct ast_frame *f)
{
struct	ast_frame *f1;

	if (!l->chan) return;
	f1 = ast_frdup(f);
	memset(&f1->frame_list,0,sizeof(f1->frame_list));
	AST_LIST_INSERT_TAIL(&l->textq,f1,frame_list);
	return;
}

static int linkcount(struct rpt *myrpt)
{
	struct	rpt_link *l;
	char *reverse_patch_state;
 	int numoflinks;

	reverse_patch_state = "DOWN";
	numoflinks = 0;
	l = myrpt->links.next;
	while(l && (l != &myrpt->links)){
		if(numoflinks >= MAX_STAT_LINKS){
			ast_log(LOG_WARNING,
			"maximum number of links exceeds %d in rpt_do_stats()!",MAX_STAT_LINKS);
			break;
		}
		//if (l->name[0] == '0'){ /* Skip '0' nodes */
		//	reverse_patch_state = "UP";
		//	l = l->next;
		//	continue;
		//}
		numoflinks++;
	 
		l = l->next;
	}
//	ast_log(LOG_NOTICE, "numoflinks=%i\n",numoflinks);
	return numoflinks;
}
/*
 * Retrieve a memory channel
 * Return 0 if sucessful,
 * -1 if channel not found,
 *  1 if parse error
 */
static int retrieve_memory(struct rpt *myrpt, char *memory)
{
	char tmp[30], *s, *s1, *s2, *val;

	if (debug)ast_log(LOG_NOTICE, "memory=%s block=%s\n",memory,myrpt->p.memory);

	val = (char *) ast_variable_retrieve(myrpt->cfg, myrpt->p.memory, memory);
	if (!val){
		return -1;
	}			
	strncpy(tmp,val,sizeof(tmp) - 1);
	tmp[sizeof(tmp)-1] = 0;

	s = strchr(tmp,',');
	if (!s)
		return 1; 
	*s++ = 0;
	s1 = strchr(s,',');
	if (!s1)
		return 1;
	*s1++ = 0;
	s2 = strchr(s1,',');
	if (!s2) s2 = s1;
	else *s2++ = 0;
	strncpy(myrpt->freq, tmp, sizeof(myrpt->freq) - 1);
	strncpy(myrpt->rxpl, s, sizeof(myrpt->rxpl) - 1);
	strncpy(myrpt->txpl, s, sizeof(myrpt->rxpl) - 1);
	myrpt->remmode = REM_MODE_FM;
	myrpt->offset = REM_SIMPLEX;
	myrpt->powerlevel = REM_MEDPWR;
	myrpt->txplon = myrpt->rxplon = 0;
	myrpt->splitkhz = 0;
	if (s2 != s1) myrpt->splitkhz = atoi(s1);
	while(*s2){
		switch(*s2++){
			case 'A':
			case 'a':
				strcpy(myrpt->rxpl, "100.0");
				strcpy(myrpt->txpl, "100.0");
				myrpt->remmode = REM_MODE_AM;	
				break;
			case 'B':
			case 'b':
				strcpy(myrpt->rxpl, "100.0");
				strcpy(myrpt->txpl, "100.0");
				myrpt->remmode = REM_MODE_LSB;
				break;
			case 'F':
				myrpt->remmode = REM_MODE_FM;
				break;
			case 'L':
			case 'l':
				myrpt->powerlevel = REM_LOWPWR;
				break;					
			case 'H':
			case 'h':
				myrpt->powerlevel = REM_HIPWR;
				break;
					
			case 'M':
			case 'm':
				myrpt->powerlevel = REM_MEDPWR;
				break;
						
			case '-':
				myrpt->offset = REM_MINUS;
				break;
						
			case '+':
				myrpt->offset = REM_PLUS;
				break;
						
			case 'S':
			case 's':
				myrpt->offset = REM_SIMPLEX;
				break;
						
			case 'T':
			case 't':
				myrpt->txplon = 1;
				break;
						
			case 'R':
			case 'r':
				myrpt->rxplon = 1;
				break;

			case 'U':
			case 'u':
				strcpy(myrpt->rxpl, "100.0");
				strcpy(myrpt->txpl, "100.0");
				myrpt->remmode = REM_MODE_USB;
				break;
			default:
				return 1;
		}
	}
	return 0;
}
/*

*/

/*
 * Routine that hangs up all links and frees all threads related to them
 * hence taking a "bird bath".  Makes a lot of noise/cleans up the mess
 */
static void birdbath(struct rpt *myrpt)
{
	struct rpt_tele *telem;
	if(debug > 2)
		ast_log(LOG_NOTICE, "birdbath!!");
	rpt_mutex_lock(&myrpt->lock);
	telem = myrpt->tele.next;
	while(telem != &myrpt->tele)
	{
		if (telem->mode == PARROT) ast_softhangup(telem->chan,AST_SOFTHANGUP_DEV);
		telem = telem->next;
	}
	rpt_mutex_unlock(&myrpt->lock);
}

/* must be called locked */
static void cancel_pfxtone(struct rpt *myrpt)
{
	struct rpt_tele *telem;
	if(debug > 2)
		ast_log(LOG_NOTICE, "cancel_pfxfone!!");
	telem = myrpt->tele.next;
	while(telem != &myrpt->tele)
	{
		if (telem->mode == PFXTONE) ast_softhangup(telem->chan,AST_SOFTHANGUP_DEV);
		telem = telem->next;
	}
}

static void do_dtmf_phone(struct rpt *myrpt, struct rpt_link *mylink, char c)
{
struct        rpt_link *l;

       l = myrpt->links.next;
       /* go thru all the links */
       while(l != &myrpt->links)
       {
               if (!l->phonemode)
               {
                       l = l->next;
                       continue;
               }
               /* dont send to self */
               if (mylink && (l == mylink))
               {
                       l = l->next;
                       continue;
               }
#ifdef	NEW_ASTERISK
               if (l->chan) ast_senddigit(l->chan,c,0);
#else
               if (l->chan) ast_senddigit(l->chan,c);
#endif
               l = l->next;
       }
       return;
}

/* node logging function */
static void donodelog(struct rpt *myrpt,char *str)
{
struct nodelog *nodep;
char	datestr[100];

	if (!myrpt->p.archivedir) return;
	nodep = (struct nodelog *)ast_malloc(sizeof(struct nodelog));
	if (nodep == NULL)
	{
		ast_log(LOG_ERROR,"Cannot get memory for node log");
		return;
	}
	time(&nodep->timestamp);
	strncpy(nodep->archivedir,myrpt->p.archivedir,
		sizeof(nodep->archivedir) - 1);
	strftime(datestr,sizeof(datestr) - 1,"%Y%m%d%H%M%S",
		localtime(&nodep->timestamp));
	snprintf(nodep->str,sizeof(nodep->str) - 1,"%s %s,%s\n",
		myrpt->name,datestr,str);
	ast_mutex_lock(&nodeloglock);
	insque((struct qelem *) nodep, (struct qelem *) nodelog.prev);
	ast_mutex_unlock(&nodeloglock);
}

/* must be called locked */
static void do_dtmf_local(struct rpt *myrpt, char c)
{
int	i;
char	digit;

	if (c)
	{
		snprintf(myrpt->dtmf_local_str + strlen(myrpt->dtmf_local_str),sizeof(myrpt->dtmf_local_str) - 1,"%c",c);
		if (!myrpt->dtmf_local_timer) 
			 myrpt->dtmf_local_timer = DTMF_LOCAL_STARTTIME;
	}
	/* if at timeout */
	if (myrpt->dtmf_local_timer == 1)
	{
		if(debug > 6)
			ast_log(LOG_NOTICE,"time out dtmf_local_timer=%i\n",myrpt->dtmf_local_timer);

		/* if anything in the string */
		if (myrpt->dtmf_local_str[0])
		{
			digit = myrpt->dtmf_local_str[0];
			myrpt->dtmf_local_str[0] = 0;
			for(i = 1; myrpt->dtmf_local_str[i]; i++)
			{
				myrpt->dtmf_local_str[i - 1] =
					myrpt->dtmf_local_str[i];
			}
			myrpt->dtmf_local_str[i - 1] = 0;
			myrpt->dtmf_local_timer = DTMF_LOCAL_TIME;
			rpt_mutex_unlock(&myrpt->lock);
			if (!strncasecmp(myrpt->txchannel->name,"rtpdir",6))
			{
#ifdef	NEW_ASTERISK
				ast_senddigit(myrpt->txchannel,digit,0);
#else
				ast_senddigit(myrpt->txchannel,digit);
#endif
			} 
			else
			{
				if (digit >= '0' && digit <='9')
					ast_playtones_start(myrpt->txchannel, 0, dtmf_tones[digit-'0'], 0);
				else if (digit >= 'A' && digit <= 'D')
					ast_playtones_start(myrpt->txchannel, 0, dtmf_tones[digit-'A'+10], 0);
				else if (digit == '*')
					ast_playtones_start(myrpt->txchannel, 0, dtmf_tones[14], 0);
				else if (digit == '#')
					ast_playtones_start(myrpt->txchannel, 0, dtmf_tones[15], 0);
				else {
					/* not handled */
					ast_log(LOG_DEBUG, "Unable to generate DTMF tone '%c' for '%s'\n", digit, myrpt->txchannel->name);
				}
			}
			rpt_mutex_lock(&myrpt->lock);
		}
		else
		{
			myrpt->dtmf_local_timer = 0;
		}
	}
}

/*
 * Routine to set the Data Terminal Ready (DTR) pin on a serial interface
*/

static int setdtr(struct rpt *myrpt,int fd, int enable)
{
struct termios mode;

	if (fd < 0) return -1;
	if (tcgetattr(fd, &mode)) {
		ast_log(LOG_WARNING, "Unable to get serial parameters for dtr: %s\n", strerror(errno));
		return -1;
	}
	if (enable)
	{
		cfsetspeed(&mode, myrpt->p.iospeed);
	}
	else
	{
		cfsetspeed(&mode, B0);
		usleep(100000);
	}
	if (tcsetattr(fd, TCSADRAIN, &mode)) {
		ast_log(LOG_WARNING, "Unable to set serial parameters for dtr: %s\n", strerror(errno));
		return -1;
	}
	if (enable) usleep(100000);
	return 0;
}

/* 
 * open the serial port
 */
 
static int openserial(struct rpt *myrpt,char *fname)
{
	struct termios mode;
	int fd;

	fd = open(fname,O_RDWR);
	if (fd == -1)
	{
		ast_log(LOG_WARNING,"Cannot open serial port %s\n",fname);
		return -1;
	}
	memset(&mode, 0, sizeof(mode));
	if (tcgetattr(fd, &mode)) {
		ast_log(LOG_WARNING, "Unable to get serial parameters on %s: %s\n", fname, strerror(errno));
		return -1;
	}
#ifndef	SOLARIS
	cfmakeraw(&mode);
#else
        mode.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                        |INLCR|IGNCR|ICRNL|IXON);
        mode.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
        mode.c_cflag &= ~(CSIZE|PARENB|CRTSCTS);
        mode.c_cflag |= CS8;
	mode.c_cc[VTIME] = 3;
	mode.c_cc[VMIN] = 1; 
#endif

	cfsetispeed(&mode, myrpt->p.iospeed);
	cfsetospeed(&mode, myrpt->p.iospeed);
	if (tcsetattr(fd, TCSANOW, &mode)) 
		ast_log(LOG_WARNING, "Unable to set serial parameters on %s: %s\n", fname, strerror(errno));
	if(!strcmp(myrpt->remoterig, remote_rig_kenwood)) setdtr(myrpt,fd,0); 
	usleep(100000);
	if (debug)ast_log(LOG_NOTICE,"Opened serial port %s\n",fname);
	return(fd);	
}


/*
 * Process DTMF keys passed
 */ 

static void local_dtmfkey_helper(struct rpt *myrpt,char c)
{
int	i;
char	*val;

	i = strlen(myrpt->dtmfkeybuf);
	if (i >= (sizeof(myrpt->dtmfkeybuf) - 1)) return;
	myrpt->dtmfkeybuf[i++] = c;
	myrpt->dtmfkeybuf[i] = 0;
	val = (char *) ast_variable_retrieve(myrpt->cfg, 
		myrpt->p.dtmfkeys,myrpt->dtmfkeybuf);
	if (!val) return;
	strncpy(myrpt->curdtmfuser,val,MAXNODESTR - 1);
	myrpt->dtmfkeyed = 1;
	myrpt->dtmfkeybuf[0] = 0;
	return;
}

static void mdc1200_notify(struct rpt *myrpt,char *fromnode, char *data)
{
	FILE *fp;
	char str[50];
	struct flock fl;
	time_t	t;

	rpt_manager_trigger(myrpt, "MDC-1200", data);

	if (!fromnode)
	{
		ast_verbose("Got MDC-1200 data %s from local system (%s)\n",
			data,myrpt->name);
		if (myrpt->p.mdclog) 
		{
			fp = fopen(myrpt->p.mdclog,"a");
			if (!fp)
			{
				ast_log(LOG_ERROR,"Cannot open MDC1200 log file %s\n",myrpt->p.mdclog);
				return;
			}
			fl.l_type = F_WRLCK;
			fl.l_whence = SEEK_SET;
			fl.l_start = 0;
			fl.l_len = 0;
			fl.l_pid = pthread_self();
			if (fcntl(fileno(fp),F_SETLKW,&fl) == -1)
			{
				ast_log(LOG_ERROR,"Cannot get lock on MDC1200 log file %s\n",myrpt->p.mdclog);			
				fclose(fp);
				return;
			}
			time(&t);
			strftime(str,sizeof(str) - 1,"%Y%m%d%H%M%S",localtime(&t));
			fprintf(fp,"%s %s %s\n",str ,myrpt->name,data);
			fl.l_type = F_UNLCK;
			fcntl(fileno(fp),F_SETLK,&fl);
			fclose(fp);
		}
	}
	else
	{
		ast_verbose("Got MDC-1200 data %s from node %s (%s)\n",
			data,fromnode,myrpt->name);
	}
}

#ifdef	_MDC_DECODE_H_

static void mdc1200_send(struct rpt *myrpt, char *data)
{
struct rpt_link *l;
struct	ast_frame wf;
char	str[200];


	if (!myrpt->keyed) return;

	sprintf(str,"I %s %s",myrpt->name,data);

	wf.frametype = AST_FRAME_TEXT;
	wf.subclass = 0;
	wf.offset = 0;
	wf.mallocd = 0;
	AST_FRAME_DATA(wf) = str; 
	wf.datalen = strlen(str) + 1;  // Isuani, 20141001
	wf.samples = 0;
	wf.src = "mdc1200_send";


	l = myrpt->links.next;
	/* otherwise, send it to all of em */
	while(l != &myrpt->links)
	{
		/* Dont send to IAXRPT client, unless main channel is Voter */
		if (((l->name[0] == '0') && strncasecmp(myrpt->rxchannel->name,"voter/", 6)) || (l->phonemode))
		{
			l = l->next;
			continue;
		}
		if (l->chan) rpt_qwrite(l,&wf); 
		l = l->next;
	}
	return;
}

static void mdc1200_cmd(struct rpt *myrpt, char *data)
{
	char busy,*myval;
	int i;

	busy = 0;
	if ((data[0] == 'I') && (!strcmp(data,myrpt->lastmdc))) return;
	myval = (char *) my_variable_match(myrpt->cfg, myrpt->p.mdcmacro, data);
	if (myval) 
	{
		if (option_verbose) ast_verbose("MDCMacro for %s doing %s on node %s\n",data,myval,myrpt->name);
		if ((*myval == 'K') || (*myval == 'k'))
		{
			if (!myrpt->keyed)
			{
				for(i = 1; myval[i]; i++) local_dtmfkey_helper(myrpt,myval[i]);
			}
			return;
		}
		if (!myrpt->keyed) return;
		rpt_mutex_lock(&myrpt->lock);
		if ((MAXMACRO - strlen(myrpt->macrobuf)) < strlen(myval))
		{
			rpt_mutex_unlock(&myrpt->lock);
			busy=1;
		}
		if(!busy)
		{
			myrpt->macrotimer = MACROTIME;
			strncat(myrpt->macrobuf,myval,MAXMACRO - 1);
		}
		rpt_mutex_unlock(&myrpt->lock);
	}
 	if ((data[0] == 'I') && (!busy)) strcpy(myrpt->lastmdc,data);
	return;
}

#ifdef	_MDC_ENCODE_H_

static void mdc1200_ack_status(struct rpt *myrpt, short UnitID)
{
struct	mdcparams *mdcp;

	mdcp = ast_calloc(1,sizeof(struct mdcparams));
	if (!mdcp)
	{
		ast_log(LOG_ERROR,"Cannot alloc!!\n");
		return;
	}
	memset(mdcp,0,sizeof(mdcp));
	mdcp->type[0] = 'A';
	mdcp->UnitID = UnitID;
	rpt_telemetry(myrpt,MDC1200,(void *)mdcp);
	return;
}

#endif
#endif

/*
 * Translate function
 */
 
static char func_xlat(struct rpt *myrpt,char c,struct rpt_xlat *xlat)
{
time_t	now;
int	gotone;

	time(&now);
	gotone = 0;
	/* if too much time, reset the skate machine */
	if ((now - xlat->lastone) > MAXXLATTIME)
	{
		xlat->funcindex = xlat->endindex = 0;
	}
	if (xlat->funccharseq[0] && (c == xlat->funccharseq[xlat->funcindex++]))
	{
		time(&xlat->lastone);
		gotone = 1;
		if (!xlat->funccharseq[xlat->funcindex])
		{
			xlat->funcindex = xlat->endindex = 0;
			return(myrpt->p.funcchar);
		}
	} else xlat->funcindex = 0;
	if (xlat->endcharseq[0] && (c == xlat->endcharseq[xlat->endindex++]))
	{
		time(&xlat->lastone);
		gotone = 1;
		if (!xlat->endcharseq[xlat->endindex])
		{
			xlat->funcindex = xlat->endindex = 0;
			return(myrpt->p.endchar);
		}
	} else xlat->endindex = 0;
	/* if in middle of decode seq, send nothing back */
	if (gotone) return(0);
	/* if no pass chars specified, return em all */
	if (!xlat->passchars[0]) return(c);
	/* if a "pass char", pass it */
	if (strchr(xlat->passchars,c)) return(c);
	return(0);
}

/*
 * Return a pointer to the first non-whitespace character
 */

static char *eatwhite(char *s)
{
	while((*s == ' ') || (*s == 0x09)){ /* get rid of any leading white space */
		if(!*s)
			break;
		s++;
	}
	return s;
}

/*
 * Function to translate characters to APRSTT data
 */

static char aprstt_xlat(char *instr,char *outstr)
{
int	i,j;
char	b,c,lastnum,overlay,cksum;
static char a_xlat[] = {0,0,'A','D','G','J','M','P','T','W'};
static char b_xlat[] = {0,0,'B','E','H','K','N','Q','U','X'};
static char c_xlat[] = {0,0,'C','F','I','L','O','R','V','Y'};
static char d_xlat[] = {0,0,0,0,0,0,0,'S',0,'Z'};

	if (strlen(instr) < 4) return 0;
	lastnum = 0;
	for(i = 1; instr[i + 2]; i++)
	{
		c = instr[i];
		switch (c)
		{
		    case 'A' :
			if (!lastnum) return 0;
			b = a_xlat[lastnum - '0'];
			if (!b) return 0;
			*outstr++ = b;
			lastnum = 0;
			break;
		    case 'B' :
			if (!lastnum) return 0;
			b = b_xlat[lastnum - '0'];
			if (!b) return 0;
			*outstr++ = b;
			lastnum = 0;
			break;
		    case 'C' :
			if (!lastnum) return 0;
			b = c_xlat[lastnum - '0'];
			if (!b) return 0;
			*outstr++ = b;
			lastnum = 0;
			break;
		    case 'D' :
			if (!lastnum) return 0;
			b = d_xlat[lastnum - '0'];
			if (!b) return 0;
			*outstr++ = b;
			lastnum = 0;
			break;
		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			if (lastnum) *outstr++ = lastnum;
			lastnum = c;
			break;
		    default:
			return 0;
		}
	}
	*outstr = 0;
	overlay = instr[i++];
	cksum = instr[i];	
	for(i = 0,j = 0; instr[i + 1]; i++)
	{
		if ((instr[i] >= '0') && (instr[i] <= '9')) j += (instr[i] - '0');
		else if ((instr[i] >= 'A') && (instr[i] <= 'D')) j += (instr[i] - 'A') + 10;
	}
	if ((cksum - '0') != (j % 10)) return 0;
	return overlay;
}

static char *strupr(char *instr)
{
char *str = instr;
        while (*str)
           {
                *str = toupper(*str);
                str++;
           }
        return(instr);
}

/*
* Break up a delimited string into a table of substrings
*
* str - delimited string ( will be modified )
* strp- list of pointers to substrings (this is built by this function), NULL will be placed at end of list
* limit- maximum number of substrings to process
* delim- user specified delimeter
* quote- user specified quote for escaping a substring. Set to zero to escape nothing.
*
* Note: This modifies the string str, be suer to save an intact copy if you need it later.
*
* Returns number of substrings found.
*/
	

static int explode_string(char *str, char *strp[], int limit, char delim, char quote)
{
int     i,l,inquo;

        inquo = 0;
        i = 0;
        strp[i++] = str;
        if (!*str)
           {
                strp[0] = 0;
                return(0);
           }
        for(l = 0; *str && (l < limit) ; str++)
        {
		if(quote)
		{
                	if (*str == quote)
                   	{	
                        	if (inquo)
                           	{
                                	*str = 0;
                                	inquo = 0;
                           	}
                        	else
                           	{
                                	strp[i - 1] = str + 1;
                                	inquo = 1;
                           	}
			}
		}	
                if ((*str == delim) && (!inquo))
                {
                        *str = 0;
			l++;
                        strp[i++] = str + 1;
                }
        }
        strp[i] = 0;
        return(i);

}
/*
* Break up a delimited string into a table of substrings
*
* str - delimited string ( will be modified )
* strp- list of pointers to substrings (this is built by this function), NULL will be placed at end of list
* limit- maximum number of substrings to process
*/
	


static int finddelim(char *str, char *strp[], int limit)
{
	return explode_string(str, strp, limit, DELIMCHR, QUOTECHR);
}

static char *string_toupper(char *str)
{
int	i;

	for(i = 0; str[i]; i++)
		if (islower(str[i])) str[i] = toupper(str[i]);
	return str;
}

static int elink_cmd(char *cmd, char *outstr, int outlen)
{
FILE	*tf;

	tf = tmpfile();
	if (!tf) return -1;
	if (debug)  ast_log(LOG_DEBUG,"elink_cmd sent %s\n",cmd);
	ast_cli_command(fileno(tf),cmd);
	rewind(tf);
	outstr[0] = 0;
	if (!fgets(outstr,outlen,tf)) 
	{
		fclose(tf);
		return 0;
	}
	fclose(tf);
	if (!strlen(outstr)) return 0;
	if (outstr[strlen(outstr) - 1] == '\n')
		outstr[strlen(outstr) - 1] = 0;
	if (debug)  ast_log(LOG_DEBUG,"elink_cmd ret. %s\n",outstr);
	return(strlen(outstr));
}

static int elink_db_get(char *lookup, char c, char *nodenum,char *callsign, char *ipaddr)
{
char	str[100],str1[100],*strs[5];
int	n;

	snprintf(str,sizeof(str) - 1,"echolink dbget %c %s",c,lookup);
	n = elink_cmd(str,str1,sizeof(str1));
	if (n < 1) return(n);
	n = explode_string(str1, strs, 5, '|', '\"');
	if (n < 3) return(0);
	if (nodenum) strcpy(nodenum,strs[0]);
	if (callsign) strcpy(callsign,strs[1]);
	if (ipaddr) strcpy(ipaddr,strs[2]);
	return(1);
}

static int tlb_node_get(char *lookup, char c, char *nodenum,char *callsign, char *ipaddr, char *port)
{
char	str[100],str1[100],*strs[6];
int	n;

	snprintf(str,sizeof(str) - 1,"tlb nodeget %c %s",c,lookup);
	n = elink_cmd(str,str1,sizeof(str1));
	if (n < 1) return(n);
	n = explode_string(str1, strs, 6, '|', '\"');
	if (n < 4) return(0);
	if (nodenum) strcpy(nodenum,strs[0]);
	if (callsign) strcpy(callsign,strs[1]);
	if (ipaddr) strcpy(ipaddr,strs[2]);
	if (port) strcpy(port,strs[3]);
	return(1);
}

/*
	send asterisk frame text message on the current tx channel
*/
static int send_usb_txt(struct rpt *myrpt, char *txt) 
{
	struct ast_frame wf;
 
	/* if (debug) */ ast_log(LOG_NOTICE, "send_usb_txt %s\n",txt);
	wf.frametype = AST_FRAME_TEXT;
	wf.subclass = 0;
	wf.offset = 0;
	wf.mallocd = 0;
	wf.datalen = strlen(txt) + 1;
	AST_FRAME_DATA(wf) = txt;
	wf.samples = 0;
	wf.src = "send_usb_txt";
	ast_write(myrpt->txchannel,&wf); 
	return 0;
}
/*
	send asterisk frame text message on the current tx channel
*/
static int send_link_pl(struct rpt *myrpt, char *txt) 
{
	struct ast_frame wf;
	struct	rpt_link *l;
	char	str[300];
 
	if (!strcmp(myrpt->p.ctgroup,"0")) return 0;
	snprintf(str, sizeof(str), "C %s %s %s", myrpt->name, myrpt->p.ctgroup, txt);
/* if (debug) */ ast_log(LOG_NOTICE, "send_link_pl %s\n",str);
	wf.frametype = AST_FRAME_TEXT;
	wf.subclass = 0;
	wf.offset = 0;
	wf.mallocd = 0;
	wf.datalen = strlen(str) + 1;
	AST_FRAME_DATA(wf) = str;
	wf.samples = 0;
	wf.src = "send_link_pl";
	l = myrpt->links.next;
	while(l && (l != &myrpt->links))
	{
		if ((l->chan) && l->name[0] && (l->name[0] != '0'))
		{
			ast_write(l->chan,&wf); 
		}
		l = l->next;
	}
	return 0;
}

/* must be called locked */
static void __mklinklist(struct rpt *myrpt, struct rpt_link *mylink, char *buf,int flag)
{
struct rpt_link *l;
char mode;
int	i,spos;

	buf[0] = 0; /* clear output buffer */
	if (myrpt->remote) return;
	/* go thru all links */
	for(l = myrpt->links.next; l != &myrpt->links; l = l->next)
	{
		/* if is not a real link, ignore it */
		if (l->name[0] == '0') continue;
		if (l->mode > 1) continue; /* dont report local modes */
		/* dont count our stuff */
		if (l == mylink) continue;
		if (mylink && (!strcmp(l->name,mylink->name))) continue;
		/* figure out mode to report */
		mode = 'T'; /* use Tranceive by default */
		if (!l->mode) mode = 'R'; /* indicate RX for our mode */
		if (!l->thisconnected) 	mode = 'C'; /* indicate connecting */
		spos = strlen(buf); /* current buf size (b4 we add our stuff) */
		if (spos)
		{
			strcat(buf,",");
			spos++;
		}
		if (flag)
		{
			snprintf(buf + spos,MAXLINKLIST - spos,
				"%s%c%c",l->name,mode,(l->lastrx1) ? 'K' : 'U');
		}
		else
		{
			/* add nodes into buffer */
			if (l->linklist[0])
			{
				snprintf(buf + spos,MAXLINKLIST - spos,
					"%c%s,%s",mode,l->name,l->linklist);
			}
			else /* if no nodes, add this node into buffer */
			{
				snprintf(buf + spos,MAXLINKLIST - spos,
					"%c%s",mode,l->name);
			}	
		}
		/* if we are in tranceive mode, let all modes stand */
		if (mode == 'T') continue;
		/* downgrade everyone on this node if appropriate */
		for(i = spos; buf[i]; i++)
		{
			if (buf[i] == 'T') buf[i] = mode;
			if ((buf[i] == 'R') && (mode == 'C')) buf[i] = mode;
		}
	}
	return;
}

/* must be called locked */
static void __kickshort(struct rpt *myrpt)
{
struct rpt_link *l;

	for(l = myrpt->links.next; l != &myrpt->links; l = l->next)
	{
		/* if is not a real link, ignore it */
		if (l->name[0] == '0') continue;
		l->linklisttimer = LINKLISTSHORTTIME;
	}
	myrpt->linkposttimer = LINKPOSTSHORTTIME;
	myrpt->lastgpstime = 0;
	return;
}

/*
 * Routine to process events for rpt_master threads
 */

static void rpt_event_process(struct rpt *myrpt)
{
char	*myval,*argv[5],*cmpvar,*var,*var1,*cmd,c;
char	buf[1000],valbuf[500],action;
int	i,l,argc,varp,var1p,thisAction,maxActions;
struct ast_variable *v;
struct ast_var_t *newvariable;


	if (!starttime) return;
	for (v = ast_variable_browse(myrpt->cfg, myrpt->p.events); v; v = v->next)
	{
		/* make a local copy of the value of this entry */
		myval = ast_strdupa(v->value);
		/* separate out specification into pipe-delimited fields */
		argc = ast_app_separate_args(myval, '|', argv, sizeof(argv) / sizeof(argv[0]));
		if (argc < 1) continue;
		if (argc != 3)
		{
			ast_log(LOG_ERROR,"event exec item malformed: %s\n",v->value);
			continue;
		}
		action = toupper(*argv[0]);
		if (!strchr("VGFCS",action))
		{
			ast_log(LOG_ERROR,"Unrecognized event action (%c) in exec item malformed: %s\n",action,v->value);
			continue;
		}
		/* start indicating no command to do */
		cmd = NULL;
		c = toupper(*argv[1]);
		if (c == 'E') /* if to merely evaluate the statement */
		{
			if (!strncasecmp(v->name,"RPT",3))
			{
				ast_log(LOG_ERROR,"%s is not a valid name for an event variable!!!!\n",v->name);
				continue;
			}
			if (!strncasecmp(v->name,"XX_",3))
			{
				ast_log(LOG_ERROR,"%s is not a valid name for an event variable!!!!\n",v->name);
				continue;
			}
			/* see if this var exists yet */
			myval = (char *) pbx_builtin_getvar_helper(myrpt->rxchannel,v->name);
			/* if not, set it to zero, in case of the value being self-referenced */
			if (!myval) pbx_builtin_setvar_helper(myrpt->rxchannel,v->name,"0");
			snprintf(valbuf,sizeof(valbuf) - 1,"$[ %s ]",argv[2]);
			buf[0] = 0;
			pbx_substitute_variables_helper(myrpt->rxchannel,
				valbuf,buf,sizeof(buf) - 1);
			if (pbx_checkcondition(buf)) cmd = "TRUE";
		}
		else
		{
			var = (char *) pbx_builtin_getvar_helper(myrpt->rxchannel,argv[2]);
			if (!var)
			{
				ast_log(LOG_ERROR,"Event variable %s not found\n",argv[2]);
				continue;
			}
			/* set to 1 if var is true */
			varp = ((pbx_checkcondition(var) > 0));
			for(i = 0; (!cmd) && (c = *(argv[1] + i)); i++)
			{
				cmpvar = (char *)ast_malloc(strlen(argv[2]) + 10);
				if (!cmpvar)
				{
					ast_log(LOG_NOTICE,"Cannot malloc()\n");
					return;
				}
				sprintf(cmpvar,"XX_%s",argv[2]);
				var1 = (char *) pbx_builtin_getvar_helper(myrpt->rxchannel,cmpvar);
				var1p = !varp; /* start with it being opposite */
				if (var1)
				{
					/* set to 1 if var is true */
					var1p = ((pbx_checkcondition(var1) > 0));
				}
//				pbx_builtin_setvar_helper(myrpt->rxchannel,cmpvar,var);
				ast_free(cmpvar);			
				c = toupper(c);
				if (!strchr("TFNI",c))
				{
					ast_log(LOG_ERROR,"Unrecognized event type (%c) in exec item malformed: %s\n",c,v->value);
					continue;
				}
				switch(c)
				{
				    case 'N': /* if no change */
					if (var1 && (varp == var1p)) cmd = (char *)v->name;
					break;
				    case 'I': /* if didnt exist (initial state) */
					if (!var1) cmd = (char *)v->name;
					break;
				    case 'F': /* transition to false */
					if (var1 && (var1p == 1) && (varp == 0)) cmd = (char *)v->name;
					break;
				    case 'T': /* transition to true */
					if ((var1p == 0) && (varp == 1)) cmd = (char *)v->name;
					break;
				}
			}
		}
		if (action == 'V') /* set a variable */
		{
			pbx_builtin_setvar_helper(myrpt->rxchannel,v->name,(cmd) ? "1" : "0");
			continue;
		}
		if (action == 'G') /* set a global variable */
		{
			pbx_builtin_setvar_helper(NULL,v->name,(cmd) ? "1" : "0");
			continue;
		}
		/* if not command to execute, go to next one */
		if (!cmd) continue;
		if (action == 'F') /* excecute a function */
		{
			rpt_mutex_lock(&myrpt->lock);
			if ((MAXMACRO - strlen(myrpt->macrobuf)) >= strlen(cmd))
			{
				if (option_verbose > 2)
					ast_verbose(VERBOSE_PREFIX_3 "Event on node %s doing macro %s for condition %s\n",
						myrpt->name,cmd,v->value);
				myrpt->macrotimer = MACROTIME;
				strncat(myrpt->macrobuf,cmd,MAXMACRO - 1);
			}
			else
			{
				ast_log(LOG_NOTICE,"Could not execute event %s for %s: Macro buffer overflow\n",cmd,argv[1]);
			}
			rpt_mutex_unlock(&myrpt->lock);
		}
		if (action == 'C') /* excecute a command */
		{

			/* make a local copy of the value of this entry */
			myval = ast_strdupa(cmd);
			/* separate out specification into comma-delimited fields */
			argc = ast_app_separate_args(myval, ',', argv, sizeof(argv) / sizeof(argv[0]));
			if (argc < 1)
			{
				ast_log(LOG_ERROR,"event exec rpt command item malformed: %s\n",cmd);
				continue;
			}
			/* Look up the action */
			l = strlen(argv[0]);
			thisAction = -1;
			maxActions = sizeof(function_table)/sizeof(struct function_table_tag);
			for(i = 0 ; i < maxActions; i++)
			{
				if(!strncasecmp(argv[0], function_table[i].action, l))
				{
					thisAction = i;
					break;
				} 
			} 
			if (thisAction < 0)
			{
				ast_log(LOG_ERROR, "Unknown action name %s.\n", argv[0]);
				continue;
			} 
			if (option_verbose > 2)
				ast_verbose(VERBOSE_PREFIX_3 "Event on node %s doing rpt command %s for condition %s\n",
					myrpt->name,cmd,v->value);
			rpt_mutex_lock(&myrpt->lock);
			if (myrpt->cmdAction.state == CMD_STATE_IDLE)
			{
				myrpt->cmdAction.state = CMD_STATE_BUSY;
				myrpt->cmdAction.functionNumber = thisAction;
				myrpt->cmdAction.param[0] = 0;
				if (argc > 1)
					strncpy(myrpt->cmdAction.param, argv[1], MAXDTMF);
				myrpt->cmdAction.digits[0] = 0;
				if (argc > 2)
					strncpy(myrpt->cmdAction.digits, argv[2], MAXDTMF);
				myrpt->cmdAction.command_source = SOURCE_RPT;
				myrpt->cmdAction.state = CMD_STATE_READY;
			} 
			else
			{
				ast_log(LOG_NOTICE,"Could not execute event %s for %s: Command buffer in use\n",cmd,argv[1]);
			}
			rpt_mutex_unlock(&myrpt->lock);
			continue;
		}
		if (action == 'S') /* excecute a shell command */
		{
			char *cp;

			if (option_verbose > 2)
				ast_verbose(VERBOSE_PREFIX_3 "Event on node %s doing shell command %s for condition %s\n",
					myrpt->name,cmd,v->value);
			cp = ast_malloc(strlen(cmd) + 10);
			if (!cp)
			{
				ast_log(LOG_NOTICE,"Unable to alloc");
				return;
			}
			memset(cp,0,strlen(cmd) + 10);
			sprintf(cp,"%s &",cmd);
			ast_safe_system(cp);
			free(cp);
			continue;
		}
	}
	for (v = ast_variable_browse(myrpt->cfg, myrpt->p.events); v; v = v->next)
	{
		/* make a local copy of the value of this entry */
		myval = ast_strdupa(v->value);
		/* separate out specification into pipe-delimited fields */
		argc = ast_app_separate_args(myval, '|', argv, sizeof(argv) / sizeof(argv[0]));
		if (argc != 3) continue;
		action = toupper(*argv[0]);
		if (!strchr("VGFCS",action)) continue;
		c = *argv[1];
		if (c == 'E') continue;
		var = (char *) pbx_builtin_getvar_helper(myrpt->rxchannel,argv[2]);
		if (!var) continue;
		/* set to 1 if var is true */
		varp = ((pbx_checkcondition(var) > 0));
		cmpvar = (char *)ast_malloc(strlen(argv[2]) + 10);
		if (!cmpvar)
		{
			ast_log(LOG_NOTICE,"Cannot malloc()\n");
			return;
		}
		sprintf(cmpvar,"XX_%s",argv[2]);
		var1 = (char *) pbx_builtin_getvar_helper(myrpt->rxchannel,cmpvar);
		pbx_builtin_setvar_helper(myrpt->rxchannel,cmpvar,var);
		ast_free(cmpvar);			
	}
	if (option_verbose < 5) return;
	i = 0;
	ast_verbose("Node Variable dump for node %s:\n",myrpt->name);
	ast_channel_lock(myrpt->rxchannel);
	AST_LIST_TRAVERSE (&myrpt->rxchannel->varshead, newvariable, entries) {
		i++;
		ast_verbose("   %s=%s\n", ast_var_name(newvariable), ast_var_value(newvariable));
	}
	ast_channel_unlock(myrpt->rxchannel);
	ast_verbose("    -- %d variables\n", i);
	return;
}

/*
 * Routine to update boolean values used in currently referenced rpt structure
 */
 

static void rpt_update_boolean(struct rpt *myrpt,char *varname, int newval)
{
char	buf[10];

	if ((!varname) || (!*varname)) return;
	buf[0] = '0';
	buf[1] = 0;
	if (newval > 0) buf[0] = '1';
	pbx_builtin_setvar_helper(myrpt->rxchannel, varname, buf);
	rpt_manager_trigger(myrpt, varname, buf);
	if (newval >= 0) rpt_event_process(myrpt);
	return;
}

/*
 * Updates the active links (channels) list that that the repeater has
 */
 

static void rpt_update_links(struct rpt *myrpt)
{
char buf[MAXLINKLIST],obuf[MAXLINKLIST + 20],*strs[MAXLINKLIST];
int	n;

	ast_mutex_lock(&myrpt->lock);
	__mklinklist(myrpt,NULL,buf,1);
	ast_mutex_unlock(&myrpt->lock);
	/* parse em */
	n = finddelim(strdupa(buf),strs,MAXLINKLIST);
	if (n) snprintf(obuf,sizeof(obuf) - 1,"%d,%s",n,buf);
	else strcpy(obuf,"0");
	pbx_builtin_setvar_helper(myrpt->rxchannel,"RPT_ALINKS",obuf);
	rpt_manager_trigger(myrpt, "RPT_ALINKS", obuf);
	snprintf(obuf,sizeof(obuf) - 1,"%d",n);
	pbx_builtin_setvar_helper(myrpt->rxchannel,"RPT_NUMALINKS",obuf);
	rpt_manager_trigger(myrpt, "RPT_NUMALINKS", obuf);
	ast_mutex_lock(&myrpt->lock);
	__mklinklist(myrpt,NULL,buf,0);
	ast_mutex_unlock(&myrpt->lock);
	/* parse em */
	n = finddelim(strdupa(buf),strs,MAXLINKLIST);
	if (n) snprintf(obuf,sizeof(obuf) - 1,"%d,%s",n,buf);
	else strcpy(obuf,"0");
	pbx_builtin_setvar_helper(myrpt->rxchannel,"RPT_LINKS",obuf);
	rpt_manager_trigger(myrpt, "RPT_LINKS", obuf);
	snprintf(obuf,sizeof(obuf) - 1,"%d",n);
	pbx_builtin_setvar_helper(myrpt->rxchannel,"RPT_NUMLINKS",obuf);
	rpt_manager_trigger(myrpt, "RPT_NUMLINKS", obuf);
	rpt_event_process(myrpt);
	return;
}

static void dodispgm(struct rpt *myrpt,char *them)
{
char 	*a;
int	i;

	if (!myrpt->p.discpgm) return;
	i = strlen(them) + strlen(myrpt->p.discpgm) + 100;
	a = ast_malloc(i);
	if (!a) 
	{
		ast_log(LOG_NOTICE,"Unable to alloc");
		return;
	}
	memset(a,0,i);
	sprintf(a,"%s %s %s &",myrpt->p.discpgm,
		myrpt->name,them);
	ast_safe_system(a);
	free(a);
	return;
}

static void doconpgm(struct rpt *myrpt,char *them)
{
char 	*a;
int	i;

	if (!myrpt->p.connpgm) return;
	i = strlen(them) + strlen(myrpt->p.connpgm) +  + 100;
	a = ast_malloc(i);
	if (!a) 
	{
		ast_log(LOG_NOTICE,"Unable to alloc");
		return;
	}
	memset(a,0,i);
	sprintf(a,"%s %s %s &",myrpt->p.connpgm,
		myrpt->name,them);
	ast_safe_system(a);
	ast_free(a);
	return;
}


static void statpost(struct rpt *myrpt,char *pairs)
{
char *str,*astr;
char *astrs[100];
int	n,pid;
time_t	now;
unsigned int seq;

	if (!myrpt->p.statpost_url) return;
	str = ast_malloc(strlen(pairs) + strlen(myrpt->p.statpost_url) + 200);
	astr = ast_strdup(myrpt->p.statpost_program);
	if ((!str) || (!astr)) return;
	n = finddelim(astr,astrs,100);
	if (n < 1)
	{
		ast_free(str);
		ast_free(astr);
		return;
	}
	ast_mutex_lock(&myrpt->statpost_lock);
	seq = ++myrpt->statpost_seqno;
	ast_mutex_unlock(&myrpt->statpost_lock);
	astrs[n++] = str;
	astrs[n] = NULL;
	time(&now);
	sprintf(str,"%s?node=%s&time=%u&seqno=%u",myrpt->p.statpost_url,
		myrpt->name,(unsigned int) now,seq);
	if (pairs) sprintf(str + strlen(str),"&%s",pairs);
	if (!(pid = fork()))
	{
		execv(astrs[0],astrs);
		ast_log(LOG_ERROR, "exec of %s failed.\n", astrs[0]);
		perror("asterisk");
		exit(0);
	}
	ast_free(astr);
	ast_free(str);
	return;
}


/* 
 * Function stream data 
 */
 
static void startoutstream(struct rpt *myrpt)
{
char *str;
char *strs[100];
int	n;

	if (!myrpt->p.outstreamcmd) return;
	if (option_verbose > 2)
		ast_verbose(VERBOSE_PREFIX_3 "app_rpt node %s starting output stream %s\n",
			myrpt->name,myrpt->p.outstreamcmd);
	str = ast_strdup(myrpt->p.outstreamcmd);
	if (!str)
	{
		ast_log(LOG_ERROR,"Malloc Failed!\n");
		return;
	}
	n = finddelim(str,strs,100);
	if (n < 1) return;
	if (pipe(myrpt->outstreampipe) == -1)
	{
		ast_log(LOG_ERROR,"pipe() failed!\n");
		ast_free(str);
		return;
	}
	if (fcntl(myrpt->outstreampipe[1],F_SETFL,O_NONBLOCK) == -1)
	{
		ast_log(LOG_ERROR,"Error: cannot set pipe to NONBLOCK");
		ast_free(str);
		return;
	}
	if (!(myrpt->outstreampid = fork()))
	{
		close(myrpt->outstreampipe[1]);
		if (dup2(myrpt->outstreampipe[0],fileno(stdin)) == -1)
		{
			ast_log(LOG_ERROR,"Error: cannot dup2() stdin");
			exit(0);
		}
		if (dup2(nullfd,fileno(stdout)) == -1)
		{
			ast_log(LOG_ERROR,"Error: cannot dup2() stdout");
			exit(0);
		}
		if (dup2(nullfd,fileno(stderr)) == -1)
		{
			ast_log(LOG_ERROR,"Error: cannot dup2() stderr");
			exit(0);
		}
		execv(strs[0],strs);
		ast_log(LOG_ERROR, "exec of %s failed.\n", strs[0]);
		perror("asterisk");
		exit(0);
	}
	ast_free(str);
	close(myrpt->outstreampipe[0]);
	if (myrpt->outstreampid == -1)
	{
		ast_log(LOG_ERROR,"fork() failed!!\n");
		close(myrpt->outstreampipe[1]);
		return;
	}
	return;
}

/* 
 * AllStar Network node lookup function.  This function will take the nodelist that has been read into memory
 * and try to match the node number that was passed to it.  If it is found, the function requested will succeed.
 * If not, it will fail.  Called when a connection to a remote node is requested.
 */

static int node_lookup(struct rpt *myrpt,char *digitbuf,char *str, int strmax, int wilds)
{

char *val;
int longestnode,i,j,found;
struct stat mystat;
struct ast_config *ourcfg;
struct ast_variable *vp;

	/* try to look it up locally first */
	val = (char *) ast_variable_retrieve(myrpt->cfg, myrpt->p.nodes, digitbuf);
	if (val)
	{
		if (str && strmax)
			snprintf(str,strmax,val,digitbuf);
		return(1);
	}
	if (wilds)
	{
		vp = ast_variable_browse(myrpt->cfg, myrpt->p.nodes);
		while(vp)
		{
			if (ast_extension_match(vp->name, digitbuf))
			{
				if (str && strmax)
					snprintf(str,strmax,vp->value,digitbuf);
				return(1);
			}
			vp = vp->next;
		}
	}
	ast_mutex_lock(&nodelookuplock);
	if (!myrpt->p.extnodefilesn) 
	{
		ast_mutex_unlock(&nodelookuplock);
		return(0);
	}
	/* determine longest node length again */		
	longestnode = 0;
	vp = ast_variable_browse(myrpt->cfg, myrpt->p.nodes);
	while(vp)
	{
		j = strlen(vp->name);
		if (*vp->name == '_') j--;
		if (j > longestnode)
			longestnode = j;
		vp = vp->next;
	}
	found = 0;
	for(i = 0; i < myrpt->p.extnodefilesn; i++)
	{
#ifdef	NEW_ASTERISK
		ourcfg = ast_config_load(myrpt->p.extnodefiles[i],config_flags);
#else
		ourcfg = ast_config_load(myrpt->p.extnodefiles[i]);
#endif
		/* if file does not exist */
		if (stat(myrpt->p.extnodefiles[i],&mystat) == -1) continue;
		/* if file not there, try next */
		if (!ourcfg) continue;
		vp = ast_variable_browse(ourcfg, myrpt->p.extnodes);
		while(vp)
		{
			j = strlen(vp->name);
			if (*vp->name == '_') j--;
			if (j > longestnode)
				longestnode = j;
			vp = vp->next;
		}
		if (!found)
		{
			val = (char *) ast_variable_retrieve(ourcfg, myrpt->p.extnodes, digitbuf);
			if (val)
			{
				found = 1;
				if (str && strmax)
					snprintf(str,strmax,val,digitbuf);
			}
		}
		ast_config_destroy(ourcfg);
	}
	myrpt->longestnode = longestnode;
	ast_mutex_unlock(&nodelookuplock);
	return(found);
}

static char *forward_node_lookup(struct rpt *myrpt,char *digitbuf, struct ast_config *cfg)
{

char *val,*efil,*enod,*strs[100];
int i,n;
struct stat mystat;
static struct ast_config *ourcfg;

	val = (char *) ast_variable_retrieve(cfg, "proxy", "extnodefile");
	if (!val) val = EXTNODEFILE;
	enod = (char *) ast_variable_retrieve(cfg, "proxy", "extnodes");
	if (!enod) enod = EXTNODES;
	ast_mutex_lock(&nodelookuplock);
	efil = ast_strdup(val);
	if (!efil) 
	{
		ast_config_destroy(ourcfg);
		if (ourcfg) ast_config_destroy(ourcfg);
		ourcfg = NULL;
		ast_mutex_unlock(&nodelookuplock);
		return NULL;
	}
	n = finddelim(efil,strs,100);
	if (n < 1)
	{
		ast_free(efil);
		ast_config_destroy(ourcfg);
		if (ourcfg) ast_config_destroy(ourcfg);
		ourcfg = NULL;
		ast_mutex_unlock(&nodelookuplock);
		return NULL;
	}
	if (ourcfg) ast_config_destroy(ourcfg);
	val = NULL;
	for(i = 0; i < n; i++)
	{
		/* if file does not exist */
		if (stat(strs[i],&mystat) == -1) continue;
#ifdef	NEW_ASTERISK
		ourcfg = ast_config_load(strs[i],config_flags);
#else
		ourcfg = ast_config_load(strs[i]);
#endif
		/* if file not there, try next */
		if (!ourcfg) continue;
		if (!val) val = (char *) ast_variable_retrieve(ourcfg, enod, digitbuf);
	}
        if (!val)
        {
                if (ourcfg) ast_config_destroy(ourcfg);
                ourcfg = NULL;
        }
	ast_mutex_unlock(&nodelookuplock);
	ast_free(efil);
	return(val);
}

/*
* Match a keyword in a list, and return index of string plus 1 if there was a match,* else return 0.
* If param is passed in non-null, then it will be set to the first character past the match
*/

static int matchkeyword(char *string, char **param, char *keywords[])
{
int	i,ls;
	for( i = 0 ; keywords[i] ; i++){
		ls = strlen(keywords[i]);
		if(!ls){
			if(param)
				*param = NULL;
			return 0;
		}
		if(!strncmp(string, keywords[i], ls)){
			if(param)
				*param = string + ls;
			return i + 1; 
		}
	}
	if(param)
		*param = NULL;
	return 0;
}

/*
* Skip characters in string which are in charlist, and return a pointer to the
* first non-matching character
*/

static char *skipchars(char *string, char *charlist)
{
int i;	
	while(*string){
		for(i = 0; charlist[i] ; i++){
			if(*string == charlist[i]){
				string++;
				break;
			}
		}
		if(!charlist[i])
			return string;
	}
	return string;
}	
					


static int myatoi(char *str)
{
int	ret;

	if (str == NULL) return -1;
	/* leave this %i alone, non-base-10 input is useful here */
	if (sscanf(str,"%i",&ret) != 1) return -1;
	return ret;
}

static int mycompar(const void *a, const void *b)
{
char	**x = (char **) a;
char	**y = (char **) b;
int	xoff,yoff;

	if ((**x < '0') || (**x > '9')) xoff = 1; else xoff = 0;
	if ((**y < '0') || (**y > '9')) yoff = 1; else yoff = 0;
	return(strcmp((*x) + xoff,(*y) + yoff));
}

static int topcompar(const void *a, const void *b)
{
struct rpt_topkey *x = (struct rpt_topkey *) a;
struct rpt_topkey *y = (struct rpt_topkey *) b;

	return(x->timesince - y->timesince);
}

#ifdef	__RPT_NOTCH

/* rpt filter routine */
static void rpt_filter(struct rpt *myrpt, volatile short *buf, int len)
{
int	i,j;
struct	rptfilter *f;

	for(i = 0; i < len; i++)
	{
		for(j = 0; j < MAXFILTERS; j++)
		{
			f = &myrpt->filters[j];
			if (!*f->desc) continue;
			f->x0 = f->x1; f->x1 = f->x2;
		        f->x2 = ((float)buf[i]) / f->gain;
		        f->y0 = f->y1; f->y1 = f->y2;
		        f->y2 =   (f->x0 + f->x2) +   f->const0 * f->x1
		                     + (f->const1 * f->y0) + (f->const2 * f->y1);
			buf[i] = (short)f->y2;
		}
	}
}

#endif


/*
 Get the time for the machine's time zone
 Note: Asterisk requires a copy of localtime
 in the /etc directory for this to work properly.
 If /etc/localtime is not present, you will get
 GMT time! This is especially important on systems
 running embedded linux distributions as they don't usually
 have support for locales. 

 If OLD_ASTERISK is defined, then the older localtime_r
 function will be used. The /etc/localtime file is not
 required in this case. This provides backward compatibility
 with Asterisk 1.2 systems.

*/

#ifdef	NEW_ASTERISK
static void rpt_localtime( time_t *t, struct ast_tm *lt, char *tz)
{
struct timeval tv;

	tv.tv_sec = *t;
	tv.tv_usec = 0;
	ast_localtime(&tv, lt, tz);

}

static time_t rpt_mktime(struct ast_tm *tm,char *zone)
{
struct timeval now;

	now = ast_mktime(tm,zone);
	return now.tv_sec;
}


#else

static void rpt_localtime( time_t *t, struct tm *lt, char *tz)
{
#ifdef OLD_ASTERISK
	localtime_r(t, lt);
#else
	ast_localtime(t, lt, tz);
#endif
}

static time_t rpt_mktime(struct tm *tm,char *zone)
{
#ifdef OLD_ASTERISK
	return(mktime(tm));
#else
	return(ast_mktime(tm,zone));
#endif
}

#endif

/* Retrieve an int from a config file */
                                                                                
static int retrieve_astcfgint(struct rpt *myrpt,char *category, char *name, int min, int max, int defl)
{
        char *var;
        int ret;
	char include_zero = 0;

	if(min < 0){ /* If min is negative, this means include 0 as a valid entry */
		min = -min;
		include_zero = 1;
	}           
                                                                     
        var = (char *) ast_variable_retrieve(myrpt->cfg, category, name);
        if(var){
                ret = myatoi(var);
		if(include_zero && !ret)
			return 0;
                if(ret < min)
                        ret = min;
                if(ret > max)
                        ret = max;
        }
        else
                ret = defl;
        return ret;
}

/* 
 * This is the initialization function.  This routine takes the data in rpt.conf and setup up the variables needed for each of
 * the repeaters that it finds.  There is some minor sanity checking done on the data passed, but not much.
 * 
 * Note that this is kind of a mess to read.  It uses the asterisk native function to read config files and pass back values assigned to
 * keywords.
 */

static void load_rpt_vars(int n,int init)
{
char *this,*val;
int	i,j,longestnode;
struct ast_variable *vp;
struct ast_config *cfg;
char *strs[100];
char s1[256];
static char *cs_keywords[] = {"rptena","rptdis","apena","apdis","lnkena","lnkdis","totena","totdis","skena","skdis",
				"ufena","ufdis","atena","atdis","noice","noicd","slpen","slpds",NULL};

	if (option_verbose > 2)
		ast_verbose(VERBOSE_PREFIX_3 "%s config for repeater %s\n",
			(init) ? "Loading initial" : "Re-Loading",rpt_vars[n].name);
	ast_mutex_lock(&rpt_vars[n].lock);
	if (rpt_vars[n].cfg) ast_config_destroy(rpt_vars[n].cfg);
#ifdef	NEW_ASTERISK
	cfg = ast_config_load("rpt.conf",config_flags);
#else
	cfg = ast_config_load("rpt.conf");
#endif
	if (!cfg) {
		ast_mutex_unlock(&rpt_vars[n].lock);
 		ast_log(LOG_NOTICE, "Unable to open radio repeater configuration rpt.conf.  Radio Repeater disabled.\n");
		pthread_exit(NULL);
	}
	rpt_vars[n].cfg = cfg; 
	this = rpt_vars[n].name;
 	memset(&rpt_vars[n].p,0,sizeof(rpt_vars[n].p));
	if (init)
	{
		char *cp;
		int savearea = (char *)&rpt_vars[n].p - (char *)&rpt_vars[n];

		cp = (char *) &rpt_vars[n].p;
		memset(cp + sizeof(rpt_vars[n].p),0,
			sizeof(rpt_vars[n]) - (sizeof(rpt_vars[n].p) + savearea));
		rpt_vars[n].tele.next = &rpt_vars[n].tele;
		rpt_vars[n].tele.prev = &rpt_vars[n].tele;
		rpt_vars[n].rpt_thread = AST_PTHREADT_NULL;
		rpt_vars[n].tailmessagen = 0;
	}
#ifdef	__RPT_NOTCH
	/* zot out filters stuff */
	memset(&rpt_vars[n].filters,0,sizeof(rpt_vars[n].filters));
#endif
	val = (char *) ast_variable_retrieve(cfg,this,"context");
	if (val) rpt_vars[n].p.ourcontext = val;
	else rpt_vars[n].p.ourcontext = this;
	val = (char *) ast_variable_retrieve(cfg,this,"callerid");
	if (val) rpt_vars[n].p.ourcallerid = val;
	val = (char *) ast_variable_retrieve(cfg,this,"accountcode");
	if (val) rpt_vars[n].p.acctcode = val;
	val = (char *) ast_variable_retrieve(cfg,this,"idrecording");
	if (val) rpt_vars[n].p.ident = val;
	val = (char *) ast_variable_retrieve(cfg,this,"hangtime");
	if (val) rpt_vars[n].p.hangtime = atoi(val);
		else rpt_vars[n].p.hangtime = (ISRANGER(rpt_vars[n].name) ? 1 : HANGTIME);
	if (rpt_vars[n].p.hangtime < 1) rpt_vars[n].p.hangtime = 1;
	val = (char *) ast_variable_retrieve(cfg,this,"althangtime");
	if (val) rpt_vars[n].p.althangtime = atoi(val);
		else rpt_vars[n].p.althangtime = (ISRANGER(rpt_vars[n].name) ? 1 : HANGTIME);
	if (rpt_vars[n].p.althangtime < 1) rpt_vars[n].p.althangtime = 1;
	val = (char *) ast_variable_retrieve(cfg,this,"totime");
	if (val) rpt_vars[n].p.totime = atoi(val);
		else rpt_vars[n].p.totime = (ISRANGER(rpt_vars[n].name) ? 9999999 : TOTIME);
	val = (char *) ast_variable_retrieve(cfg,this,"voxtimeout");
	if (val) rpt_vars[n].p.voxtimeout_ms = atoi(val);
		else rpt_vars[n].p.voxtimeout_ms = VOX_TIMEOUT_MS;
	val = (char *) ast_variable_retrieve(cfg,this,"voxrecover");
	if (val) rpt_vars[n].p.voxrecover_ms = atoi(val);
		else rpt_vars[n].p.voxrecover_ms = VOX_RECOVER_MS;
	val = (char *) ast_variable_retrieve(cfg,this,"simplexpatchdelay");
	if (val) rpt_vars[n].p.simplexpatchdelay = atoi(val);
		else rpt_vars[n].p.simplexpatchdelay = SIMPLEX_PATCH_DELAY;
	val = (char *) ast_variable_retrieve(cfg,this,"simplexphonedelay");
	if (val) rpt_vars[n].p.simplexphonedelay = atoi(val);
		else rpt_vars[n].p.simplexphonedelay = SIMPLEX_PHONE_DELAY;
	val = (char *) ast_variable_retrieve(cfg,this,"statpost_program");
	if (val) rpt_vars[n].p.statpost_program = val;
		else rpt_vars[n].p.statpost_program = STATPOST_PROGRAM;
	rpt_vars[n].p.statpost_url = 
		(char *) ast_variable_retrieve(cfg,this,"statpost_url");
	rpt_vars[n].p.tailmessagetime = retrieve_astcfgint(&rpt_vars[n],this, "tailmessagetime", 0, 200000000, 0);		
	rpt_vars[n].p.tailsquashedtime = retrieve_astcfgint(&rpt_vars[n],this, "tailsquashedtime", 0, 200000000, 0);		
	rpt_vars[n].p.duplex = retrieve_astcfgint(&rpt_vars[n],this,"duplex",0,4,(ISRANGER(rpt_vars[n].name) ? 0 : 2));
	rpt_vars[n].p.idtime = retrieve_astcfgint(&rpt_vars[n],this, "idtime", -60000, 2400000, IDTIME);	/* Enforce a min max including zero */
	rpt_vars[n].p.politeid = retrieve_astcfgint(&rpt_vars[n],this, "politeid", 30000, 300000, POLITEID); /* Enforce a min max */
	j  = retrieve_astcfgint(&rpt_vars[n],this, "elke", 0, 40000000, 0);
	rpt_vars[n].p.elke  = j * 1210;
	val = (char *) ast_variable_retrieve(cfg,this,"tonezone");
	if (val) rpt_vars[n].p.tonezone = val;
	rpt_vars[n].p.tailmessages[0] = 0;
	rpt_vars[n].p.tailmessagemax = 0;
	val = (char *) ast_variable_retrieve(cfg,this,"tailmessagelist");
	if (val) rpt_vars[n].p.tailmessagemax = finddelim(val, rpt_vars[n].p.tailmessages, 500);
	rpt_vars[n].p.aprstt = (char *) ast_variable_retrieve(cfg,this,"aprstt");
	val = (char *) ast_variable_retrieve(cfg,this,"memory");
	if (!val) val = MEMORY;
	rpt_vars[n].p.memory = val;
	val = (char *) ast_variable_retrieve(cfg,this,"morse");
	if (!val) val = MORSE;
	rpt_vars[n].p.morse = val;
	val = (char *) ast_variable_retrieve(cfg,this,"telemetry");
	if (!val) val = TELEMETRY;
	rpt_vars[n].p.telemetry = val;
	val = (char *) ast_variable_retrieve(cfg,this,"macro");
	if (!val) val = MACRO;
	rpt_vars[n].p.macro = val;
	val = (char *) ast_variable_retrieve(cfg,this,"tonemacro");
	if (!val) val = TONEMACRO;
	rpt_vars[n].p.tonemacro = val;
	val = (char *) ast_variable_retrieve(cfg,this,"mdcmacro");
	if (!val) val = MDCMACRO;
	rpt_vars[n].p.mdcmacro = val;
	val = (char *) ast_variable_retrieve(cfg,this,"startup_macro");
	if (val) rpt_vars[n].p.startupmacro = val;
	val = (char *) ast_variable_retrieve(cfg,this,"iobase");
	/* do not use atoi() here, we need to be able to have
		the input specified in hex or decimal so we use
		sscanf with a %i */
	if ((!val) || (sscanf(val,"%i",&rpt_vars[n].p.iobase) != 1))
		rpt_vars[n].p.iobase = DEFAULT_IOBASE;
	val = (char *) ast_variable_retrieve(cfg,this,"ioport");
	rpt_vars[n].p.ioport = val;
	val = (char *) ast_variable_retrieve(cfg,this,"functions");
	if (!val)
		{
			val = FUNCTIONS;
			rpt_vars[n].p.simple = 1;
		} 
	rpt_vars[n].p.functions = val;
	val =  (char *) ast_variable_retrieve(cfg,this,"link_functions");
	if (val) rpt_vars[n].p.link_functions = val;
	else 
		rpt_vars[n].p.link_functions = rpt_vars[n].p.functions;
	val = (char *) ast_variable_retrieve(cfg,this,"phone_functions");
	if (val) rpt_vars[n].p.phone_functions = val;
	else if (ISRANGER(rpt_vars[n].name)) rpt_vars[n].p.phone_functions = rpt_vars[n].p.functions;
	val = (char *) ast_variable_retrieve(cfg,this,"dphone_functions");
	if (val) rpt_vars[n].p.dphone_functions = val;
	else if (ISRANGER(rpt_vars[n].name)) rpt_vars[n].p.dphone_functions = rpt_vars[n].p.functions;
	val = (char *) ast_variable_retrieve(cfg,this,"alt_functions");
	if (val) rpt_vars[n].p.alt_functions = val;
	val = (char *) ast_variable_retrieve(cfg,this,"funcchar");
	if (!val) rpt_vars[n].p.funcchar = FUNCCHAR; else 
		rpt_vars[n].p.funcchar = *val;		
	val = (char *) ast_variable_retrieve(cfg,this,"endchar");
	if (!val) rpt_vars[n].p.endchar = ENDCHAR; else 
		rpt_vars[n].p.endchar = *val;		
	val = (char *) ast_variable_retrieve(cfg,this,"nobusyout");
	if (val) rpt_vars[n].p.nobusyout = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"notelemtx");
	if (val) rpt_vars[n].p.notelemtx = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"propagate_dtmf");
	if (val) rpt_vars[n].p.propagate_dtmf = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"propagate_phonedtmf");
	if (val) rpt_vars[n].p.propagate_phonedtmf = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"linktolink");
	if (val) rpt_vars[n].p.linktolink = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"nodes");
	if (!val) val = NODES;
	rpt_vars[n].p.nodes = val;
	val = (char *) ast_variable_retrieve(cfg,this,"extnodes");
	if (!val) val = EXTNODES;
	rpt_vars[n].p.extnodes = val;
	val = (char *) ast_variable_retrieve(cfg,this,"extnodefile");
	if (!val) val = EXTNODEFILE;
	rpt_vars[n].p.extnodefilesn = 
	    explode_string(val,rpt_vars[n].p.extnodefiles,MAX_EXTNODEFILES,',',0);
	val = (char *) ast_variable_retrieve(cfg,this,"locallinknodes");
	if (val) rpt_vars[n].p.locallinknodesn = explode_string(ast_strdup(val),rpt_vars[n].p.locallinknodes,MAX_LOCALLINKNODES,',',0);
	val = (char *) ast_variable_retrieve(cfg,this,"lconn");
	if (val) rpt_vars[n].p.nlconn = explode_string(strupr(ast_strdup(val)),rpt_vars[n].p.lconn,MAX_LSTUFF,',',0);
	val = (char *) ast_variable_retrieve(cfg,this,"ldisc");
	if (val) rpt_vars[n].p.nldisc = explode_string(strupr(ast_strdup(val)),rpt_vars[n].p.ldisc,MAX_LSTUFF,',',0);
	val = (char *) ast_variable_retrieve(cfg,this,"patchconnect");
	rpt_vars[n].p.patchconnect = val;
	val = (char *) ast_variable_retrieve(cfg,this,"archivedir");
	if (val) rpt_vars[n].p.archivedir = val;
	val = (char *) ast_variable_retrieve(cfg,this,"authlevel");
	if (val) rpt_vars[n].p.authlevel = atoi(val); 
	else rpt_vars[n].p.authlevel = 0;
	val = (char *) ast_variable_retrieve(cfg,this,"parrot");
	if (val) rpt_vars[n].p.parrotmode = (ast_true(val)) ? 2 : 0;
	else rpt_vars[n].p.parrotmode = 0;
	val = (char *) ast_variable_retrieve(cfg,this,"parrottime");
	if (val) rpt_vars[n].p.parrottime = atoi(val); 
	else rpt_vars[n].p.parrottime = PARROTTIME;
	val = (char *) ast_variable_retrieve(cfg,this,"rptnode");
	rpt_vars[n].p.rptnode = val;
	val = (char *) ast_variable_retrieve(cfg,this,"mars");
	if (val) rpt_vars[n].p.remote_mars = atoi(val); 
	else rpt_vars[n].p.remote_mars = 0;
	val = (char *) ast_variable_retrieve(cfg,this,"monminblocks");
	if (val) rpt_vars[n].p.monminblocks = atol(val); 
	else rpt_vars[n].p.monminblocks = DEFAULT_MONITOR_MIN_DISK_BLOCKS;
	val = (char *) ast_variable_retrieve(cfg,this,"remote_inact_timeout");
	if (val) rpt_vars[n].p.remoteinacttimeout = atoi(val); 
	else rpt_vars[n].p.remoteinacttimeout = DEFAULT_REMOTE_INACT_TIMEOUT;
	val = (char *) ast_variable_retrieve(cfg,this,"civaddr");
	if (val) rpt_vars[n].p.civaddr = atoi(val); 
	else rpt_vars[n].p.civaddr = DEFAULT_CIV_ADDR;
	val = (char *) ast_variable_retrieve(cfg,this,"remote_timeout");
	if (val) rpt_vars[n].p.remotetimeout = atoi(val); 
	else rpt_vars[n].p.remotetimeout = DEFAULT_REMOTE_TIMEOUT;
	val = (char *) ast_variable_retrieve(cfg,this,"remote_timeout_warning");
	if (val) rpt_vars[n].p.remotetimeoutwarning = atoi(val); 
	else rpt_vars[n].p.remotetimeoutwarning = DEFAULT_REMOTE_TIMEOUT_WARNING;
	val = (char *) ast_variable_retrieve(cfg,this,"remote_timeout_warning_freq");
	if (val) rpt_vars[n].p.remotetimeoutwarningfreq = atoi(val); 
	else rpt_vars[n].p.remotetimeoutwarningfreq = DEFAULT_REMOTE_TIMEOUT_WARNING_FREQ;
	val = (char *) ast_variable_retrieve(cfg,this,"erxgain");
	if (!val) val = DEFAULT_ERXGAIN;
	rpt_vars[n].p.erxgain = pow(10.0,atof(val) / 20.0); 
	val = (char *) ast_variable_retrieve(cfg,this,"etxgain");
	if (!val) val = DEFAULT_ETXGAIN;
	rpt_vars[n].p.etxgain = pow(10.0,atof(val) / 20.0);
	val = (char *) ast_variable_retrieve(cfg,this,"eannmode");
	if (val) rpt_vars[n].p.eannmode = atoi(val);
	else rpt_vars[n].p.eannmode = DEFAULT_EANNMODE;
	if (rpt_vars[n].p.eannmode < 0) rpt_vars[n].p.eannmode = 0;
	if (rpt_vars[n].p.eannmode > 3) rpt_vars[n].p.eannmode = 3;
	val = (char *) ast_variable_retrieve(cfg,this,"trxgain");
	if (!val) val = DEFAULT_TRXGAIN;
	rpt_vars[n].p.trxgain = pow(10.0,atof(val) / 20.0); 
	val = (char *) ast_variable_retrieve(cfg,this,"ttxgain");
	if (!val) val = DEFAULT_TTXGAIN;
	rpt_vars[n].p.ttxgain = pow(10.0,atof(val) / 20.0);
	val = (char *) ast_variable_retrieve(cfg,this,"tannmode");
	if (val) rpt_vars[n].p.tannmode = atoi(val);
	else rpt_vars[n].p.tannmode = DEFAULT_TANNMODE;
	if (rpt_vars[n].p.tannmode < 1) rpt_vars[n].p.tannmode = 1;
	if (rpt_vars[n].p.tannmode > 3) rpt_vars[n].p.tannmode = 3;
	val = (char *) ast_variable_retrieve(cfg,this,"linkmongain");
	if (!val) val = DEFAULT_LINKMONGAIN;
	rpt_vars[n].p.linkmongain = pow(10.0,atof(val) / 20.0);
	val = (char *) ast_variable_retrieve(cfg,this,"discpgm");
	rpt_vars[n].p.discpgm = val; 
	val = (char *) ast_variable_retrieve(cfg,this,"connpgm");
	rpt_vars[n].p.connpgm = val;
	val = (char *) ast_variable_retrieve(cfg,this,"mdclog");
	rpt_vars[n].p.mdclog = val;
	val = (char *) ast_variable_retrieve(cfg,this,"lnkactenable");
	if (val) rpt_vars[n].p.lnkactenable = ast_true(val);
	else rpt_vars[n].p.lnkactenable = 0;
	rpt_vars[n].p.lnkacttime = retrieve_astcfgint(&rpt_vars[n],this, "lnkacttime", -120, 90000, 0);	/* Enforce a min max including zero */
	val = (char *) ast_variable_retrieve(cfg, this, "lnkactmacro");
	rpt_vars[n].p.lnkactmacro = val;
	val = (char *) ast_variable_retrieve(cfg, this, "lnkacttimerwarn");
	rpt_vars[n].p.lnkacttimerwarn = val;
	val = (char *) ast_variable_retrieve(cfg, this, "nolocallinkct");
	rpt_vars[n].p.nolocallinkct = ast_true(val);
	rpt_vars[n].p.rptinacttime = retrieve_astcfgint(&rpt_vars[n],this, "rptinacttime", -120, 90000, 0);	/* Enforce a min max including zero */
	val = (char *) ast_variable_retrieve(cfg, this, "rptinactmacro");
	rpt_vars[n].p.rptinactmacro = val;
	val = (char *) ast_variable_retrieve(cfg, this, "nounkeyct");
	rpt_vars[n].p.nounkeyct = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg, this, "holdofftelem");
	rpt_vars[n].p.holdofftelem = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg, this, "beaconing");
	rpt_vars[n].p.beaconing = ast_true(val); 
	val = (char *) ast_variable_retrieve(cfg,this,"rxburstfreq");
	if (val) rpt_vars[n].p.rxburstfreq = atoi(val);
	else rpt_vars[n].p.rxburstfreq = 0;
	val = (char *) ast_variable_retrieve(cfg,this,"rxbursttime");
	if (val) rpt_vars[n].p.rxbursttime = atoi(val);
	else rpt_vars[n].p.rxbursttime = DEFAULT_RXBURST_TIME;
	val = (char *) ast_variable_retrieve(cfg,this,"rxburstthreshold");
	if (val) rpt_vars[n].p.rxburstthreshold = atoi(val);
	else rpt_vars[n].p.rxburstthreshold = DEFAULT_RXBURST_THRESHOLD;
	val = (char *) ast_variable_retrieve(cfg,this,"litztime");
	if (val) rpt_vars[n].p.litztime = atoi(val);
	else rpt_vars[n].p.litztime = DEFAULT_LITZ_TIME;
	val = (char *) ast_variable_retrieve(cfg,this,"litzchar");
	if (!val) val = DEFAULT_LITZ_CHAR;
	rpt_vars[n].p.litzchar = val;
	val = (char *) ast_variable_retrieve(cfg,this,"litzcmd");
	rpt_vars[n].p.litzcmd = val;
	val = (char *) ast_variable_retrieve(cfg,this,"itxctcss");
	if (val) rpt_vars[n].p.itxctcss = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"gpsfeet");
	if (val) rpt_vars[n].p.gpsfeet = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"split2m");
	if (val) rpt_vars[n].p.default_split_2m = atoi(val);
	else rpt_vars[n].p.default_split_2m = DEFAULT_SPLIT_2M;
	val = (char *) ast_variable_retrieve(cfg,this,"split70cm");
	if (val) rpt_vars[n].p.default_split_70cm = atoi(val);
	else rpt_vars[n].p.default_split_70cm = DEFAULT_SPLIT_70CM;
	val = (char *) ast_variable_retrieve(cfg,this,"dtmfkey");
	if (val) rpt_vars[n].p.dtmfkey = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"dtmfkeys");
	if (!val) val = DTMFKEYS;
	rpt_vars[n].p.dtmfkeys = val;
	val = (char *) ast_variable_retrieve(cfg,this,"outstreamcmd");
	rpt_vars[n].p.outstreamcmd = val;
	val = (char *) ast_variable_retrieve(cfg,this,"eloutbound");
	rpt_vars[n].p.eloutbound = val;
	val = (char *) ast_variable_retrieve(cfg,this,"events");
	if (!val) val = "events";
	rpt_vars[n].p.events = val;
	val = (char *) ast_variable_retrieve(cfg,this,"timezone");
	rpt_vars[n].p.timezone = val;

#ifdef	__RPT_NOTCH
	val = (char *) ast_variable_retrieve(cfg,this,"rxnotch");
	if (val) {
		i = finddelim(val,strs,MAXFILTERS * 2);
		i &= ~1; /* force an even number, rounded down */
		if (i >= 2) for(j = 0; j < i; j += 2)
		{
			rpt_mknotch(atof(strs[j]),atof(strs[j + 1]),
			  &rpt_vars[n].filters[j >> 1].gain,
			    &rpt_vars[n].filters[j >> 1].const0,
				&rpt_vars[n].filters[j >> 1].const1,
				    &rpt_vars[n].filters[j >> 1].const2);
			sprintf(rpt_vars[n].filters[j >> 1].desc,"%s Hz, BW = %s",
				strs[j],strs[j + 1]);
		}

	}
#endif
	val = (char *) ast_variable_retrieve(cfg,this,"telemnomdb");
	if (!val) val = "0";
	rpt_vars[n].p.telemnomgain = pow(10.0,atof(val) / 20.0);
	val = (char *) ast_variable_retrieve(cfg,this,"telemduckdb");
	if (!val) val = DEFAULT_TELEMDUCKDB;
	rpt_vars[n].p.telemduckgain = pow(10.0,atof(val) / 20.0);
	val = (char *) ast_variable_retrieve(cfg,this,"telemdefault");
	if (val) rpt_vars[n].p.telemdefault = atoi(val);
	else rpt_vars[n].p.telemdefault = DEFAULT_RPT_TELEMDEFAULT;
	val = (char *) ast_variable_retrieve(cfg,this,"telemdynamic");
	if (val) rpt_vars[n].p.telemdynamic = ast_true(val);
	else rpt_vars[n].p.telemdynamic = DEFAULT_RPT_TELEMDYNAMIC;
	if (!rpt_vars[n].p.telemdefault) 
		rpt_vars[n].telemmode = 0;
	else if (rpt_vars[n].p.telemdefault == 2)
		rpt_vars[n].telemmode = 1;
	else rpt_vars[n].telemmode = 0x7fffffff;

	val = (char *) ast_variable_retrieve(cfg,this,"guilinkdefault");
	if (val) rpt_vars[n].p.linkmode[LINKMODE_GUI] = atoi(val);
	else rpt_vars[n].p.linkmode[LINKMODE_GUI] = DEFAULT_GUI_LINK_MODE;
	val = (char *) ast_variable_retrieve(cfg,this,"guilinkdynamic");
	if (val) rpt_vars[n].p.linkmodedynamic[LINKMODE_GUI] = ast_true(val);
	else rpt_vars[n].p.linkmodedynamic[LINKMODE_GUI] = DEFAULT_GUI_LINK_MODE_DYNAMIC;

	val = (char *) ast_variable_retrieve(cfg,this,"phonelinkdefault");
	if (val) rpt_vars[n].p.linkmode[LINKMODE_PHONE] = atoi(val);
	else rpt_vars[n].p.linkmode[LINKMODE_PHONE] = DEFAULT_PHONE_LINK_MODE;
	val = (char *) ast_variable_retrieve(cfg,this,"phonelinkdynamic");
	if (val) rpt_vars[n].p.linkmodedynamic[LINKMODE_PHONE] = ast_true(val);
	else rpt_vars[n].p.linkmodedynamic[LINKMODE_PHONE] = DEFAULT_PHONE_LINK_MODE_DYNAMIC;

	val = (char *) ast_variable_retrieve(cfg,this,"echolinkdefault");
	if (val) rpt_vars[n].p.linkmode[LINKMODE_ECHOLINK] = atoi(val);
	else rpt_vars[n].p.linkmode[LINKMODE_ECHOLINK] = DEFAULT_ECHOLINK_LINK_MODE;
	val = (char *) ast_variable_retrieve(cfg,this,"echolinkdynamic");
	if (val) rpt_vars[n].p.linkmodedynamic[LINKMODE_ECHOLINK] = ast_true(val);
	else rpt_vars[n].p.linkmodedynamic[LINKMODE_ECHOLINK] = DEFAULT_ECHOLINK_LINK_MODE_DYNAMIC;

	val = (char *) ast_variable_retrieve(cfg,this,"tlbdefault");
	if (val) rpt_vars[n].p.linkmode[LINKMODE_TLB] = atoi(val);
	else rpt_vars[n].p.linkmode[LINKMODE_TLB] = DEFAULT_TLB_LINK_MODE;
	val = (char *) ast_variable_retrieve(cfg,this,"tlbdynamic");
	if (val) rpt_vars[n].p.linkmodedynamic[LINKMODE_TLB] = ast_true(val);
	else rpt_vars[n].p.linkmodedynamic[LINKMODE_TLB] = DEFAULT_TLB_LINK_MODE_DYNAMIC;

	val = (char *) ast_variable_retrieve(cfg,this,"locallist");
	if (val) {
		memset(rpt_vars[n].p.locallist,0,sizeof(rpt_vars[n].p.locallist));
		rpt_vars[n].p.nlocallist = finddelim(val,rpt_vars[n].p.locallist,16);
	}

	val = (char *) ast_variable_retrieve(cfg,this,"ctgroup");
	if (val) {
		strncpy(rpt_vars[n].p.ctgroup,val,sizeof(rpt_vars[n].p.ctgroup) - 1);
	} else strcpy(rpt_vars[n].p.ctgroup,"0");

	val = (char *) ast_variable_retrieve(cfg,this,"inxlat");
	if (val) {
		memset(&rpt_vars[n].p.inxlat,0,sizeof(struct rpt_xlat));
		i = finddelim(val,strs,3);
		if (i) strncpy(rpt_vars[n].p.inxlat.funccharseq,strs[0],MAXXLAT - 1);
		if (i > 1) strncpy(rpt_vars[n].p.inxlat.endcharseq,strs[1],MAXXLAT - 1);
		if (i > 2) strncpy(rpt_vars[n].p.inxlat.passchars,strs[2],MAXXLAT - 1);
		if (i > 3) rpt_vars[n].p.dopfxtone = ast_true(strs[3]);
	}
	val = (char *) ast_variable_retrieve(cfg,this,"outxlat");
	if (val) {
		memset(&rpt_vars[n].p.outxlat,0,sizeof(struct rpt_xlat));
		i = finddelim(val,strs,3);
		if (i) strncpy(rpt_vars[n].p.outxlat.funccharseq,strs[0],MAXXLAT - 1);
		if (i > 1) strncpy(rpt_vars[n].p.outxlat.endcharseq,strs[1],MAXXLAT - 1);
		if (i > 2) strncpy(rpt_vars[n].p.outxlat.passchars,strs[2],MAXXLAT - 1);
	}
	val = (char *) ast_variable_retrieve(cfg,this,"sleeptime");
	if (val) rpt_vars[n].p.sleeptime = atoi(val);
	else rpt_vars[n].p.sleeptime = SLEEPTIME;
	/* retrieve the stanza name for the control states if there is one */
	val = (char *) ast_variable_retrieve(cfg,this,"controlstates");
	rpt_vars[n].p.csstanzaname = val;
		
	/* retrieve the stanza name for the scheduler if there is one */
	val = (char *) ast_variable_retrieve(cfg,this,"scheduler");
	rpt_vars[n].p.skedstanzaname = val;

	/* retrieve the stanza name for the txlimits */
	val = (char *) ast_variable_retrieve(cfg,this,"txlimits");
	rpt_vars[n].p.txlimitsstanzaname = val;

	rpt_vars[n].p.iospeed = B9600;
	if (!strcasecmp(rpt_vars[n].remoterig,remote_rig_ft950))
		rpt_vars[n].p.iospeed = B38400;
	if (!strcasecmp(rpt_vars[n].remoterig,remote_rig_ft100))
		rpt_vars[n].p.iospeed = B4800;
	if (!strcasecmp(rpt_vars[n].remoterig,remote_rig_ft897))
		rpt_vars[n].p.iospeed = B4800;
	val = (char *) ast_variable_retrieve(cfg,this,"dias");
	if (val) rpt_vars[n].p.dias = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"dusbabek");
	if (val) rpt_vars[n].p.dusbabek = ast_true(val);
	val = (char *) ast_variable_retrieve(cfg,this,"iospeed");
	if (val)
	{
	    switch(atoi(val))
	    {
		case 2400:
			rpt_vars[n].p.iospeed = B2400;
			break;
		case 4800:
			rpt_vars[n].p.iospeed = B4800;
			break;
		case 19200:
			rpt_vars[n].p.iospeed = B19200;
			break;
		case 38400:
			rpt_vars[n].p.iospeed = B38400;
			break;
		case 57600:
			rpt_vars[n].p.iospeed = B57600;
			break;
		default:
			ast_log(LOG_ERROR,"%s is not valid baud rate for iospeed\n",val);
			break;
	    }
	}

	longestnode = 0;

	vp = ast_variable_browse(cfg, rpt_vars[n].p.nodes);
		
	while(vp){
		j = strlen(vp->name);
		if (j > longestnode)
			longestnode = j;
		vp = vp->next;
	}

	rpt_vars[n].longestnode = longestnode;
		
	/*
	* For this repeater, Determine the length of the longest function 
	*/
	rpt_vars[n].longestfunc = 0;
	vp = ast_variable_browse(cfg, rpt_vars[n].p.functions);
	while(vp){
		j = strlen(vp->name);
		if (j > rpt_vars[n].longestfunc)
			rpt_vars[n].longestfunc = j;
		vp = vp->next;
	}
	/*
	* For this repeater, Determine the length of the longest function 
	*/
	rpt_vars[n].link_longestfunc = 0;
	vp = ast_variable_browse(cfg, rpt_vars[n].p.link_functions);
	while(vp){
		j = strlen(vp->name);
		if (j > rpt_vars[n].link_longestfunc)
			rpt_vars[n].link_longestfunc = j;
		vp = vp->next;
	}
	rpt_vars[n].phone_longestfunc = 0;
	if (rpt_vars[n].p.phone_functions)
	{
		vp = ast_variable_browse(cfg, rpt_vars[n].p.phone_functions);
		while(vp){
			j = strlen(vp->name);
			if (j > rpt_vars[n].phone_longestfunc)
				rpt_vars[n].phone_longestfunc = j;
			vp = vp->next;
		}
	}
	rpt_vars[n].dphone_longestfunc = 0;
	if (rpt_vars[n].p.dphone_functions)
	{
		vp = ast_variable_browse(cfg, rpt_vars[n].p.dphone_functions);
		while(vp){
			j = strlen(vp->name);
			if (j > rpt_vars[n].dphone_longestfunc)
				rpt_vars[n].dphone_longestfunc = j;
			vp = vp->next;
		}
	}
	rpt_vars[n].alt_longestfunc = 0;
	if (rpt_vars[n].p.alt_functions)
	{
		vp = ast_variable_browse(cfg, rpt_vars[n].p.alt_functions);
		while(vp){
			j = strlen(vp->name);
			if (j > rpt_vars[n].alt_longestfunc)
				rpt_vars[n].alt_longestfunc = j;
			vp = vp->next;
		}
	}
	rpt_vars[n].macro_longest = 1;
	vp = ast_variable_browse(cfg, rpt_vars[n].p.macro);
	while(vp){
		j = strlen(vp->name);
		if (j > rpt_vars[n].macro_longest)
			rpt_vars[n].macro_longest = j;
		vp = vp->next;
	}
	
	/* Browse for control states */
	if(rpt_vars[n].p.csstanzaname)
		vp = ast_variable_browse(cfg, rpt_vars[n].p.csstanzaname);
	else
		vp = NULL;
	for( i = 0 ; vp && (i < MAX_SYSSTATES) ; i++){ /* Iterate over the number of control state lines in the stanza */
		int k,nukw,statenum;
		statenum=atoi(vp->name);
		strncpy(s1, vp->value, 255);
		s1[255] = 0;
		nukw  = finddelim(s1,strs,32);
		
		for (k = 0 ; k < nukw ; k++){ /* for each user specified keyword */	
			for(j = 0 ; cs_keywords[j] != NULL ; j++){ /* try to match to one in our internal table */
				if(!strcmp(strs[k],cs_keywords[j])){
					switch(j){
						case 0: /* rptena */
							rpt_vars[n].p.s[statenum].txdisable = 0;
							break;
						case 1: /* rptdis */
							rpt_vars[n].p.s[statenum].txdisable = 1;
							break;
			
						case 2: /* apena */
							rpt_vars[n].p.s[statenum].autopatchdisable = 0;
							break;

						case 3: /* apdis */
							rpt_vars[n].p.s[statenum].autopatchdisable = 1;
							break;

						case 4: /* lnkena */
							rpt_vars[n].p.s[statenum].linkfundisable = 0;
							break;
	
						case 5: /* lnkdis */
							rpt_vars[n].p.s[statenum].linkfundisable = 1;
							break;

						case 6: /* totena */
							rpt_vars[n].p.s[statenum].totdisable = 0;
							break;
					
						case 7: /* totdis */
							rpt_vars[n].p.s[statenum].totdisable = 1;
							break;

						case 8: /* skena */
							rpt_vars[n].p.s[statenum].schedulerdisable = 0;
							break;

						case 9: /* skdis */
							rpt_vars[n].p.s[statenum].schedulerdisable = 1;
							break;

						case 10: /* ufena */
							rpt_vars[n].p.s[statenum].userfundisable = 0;
							break;

						case 11: /* ufdis */
							rpt_vars[n].p.s[statenum].userfundisable = 1;
							break;

						case 12: /* atena */
							rpt_vars[n].p.s[statenum].alternatetail = 1;
							break;

						case 13: /* atdis */
							rpt_vars[n].p.s[statenum].alternatetail = 0;
							break;

						case 14: /* noice */
							rpt_vars[n].p.s[statenum].noincomingconns = 1;
							break;

						case 15: /* noicd */
							rpt_vars[n].p.s[statenum].noincomingconns = 0;
							break;

						case 16: /* slpen */
							rpt_vars[n].p.s[statenum].sleepena = 1;
							break;

						case 17: /* slpds */
							rpt_vars[n].p.s[statenum].sleepena = 0;
							break;

						default:
							ast_log(LOG_WARNING,
								"Unhandled control state keyword %s", cs_keywords[i]);
							break;
					}
				}
			}
		}
		vp = vp->next;
	}
	ast_mutex_unlock(&rpt_vars[n].lock);
}

/*
* Enable or disable debug output at a given level at the console
*/
                                                                                                                                 
static int rpt_do_debug(int fd, int argc, char *argv[])
{
	int newlevel;

        if (argc != 4)
                return RESULT_SHOWUSAGE;
        newlevel = myatoi(argv[3]);
        if((newlevel < 0) || (newlevel > 7))
                return RESULT_SHOWUSAGE;
        if(newlevel)
                ast_cli(fd, "app_rpt Debugging enabled, previous level: %d, new level: %d\n", debug, newlevel);
        else
                ast_cli(fd, "app_rpt Debugging disabled\n");

        debug = newlevel;                                                                                                                          
        return RESULT_SUCCESS;
}

/*
* Dump rpt struct debugging onto console
*/
                                                                                                                                 
static int rpt_do_dump(int fd, int argc, char *argv[])
{
	int i;

        if (argc != 3)
                return RESULT_SHOWUSAGE;

	for(i = 0; i < nrpts; i++)
	{
		if (!strcmp(argv[2],rpt_vars[i].name))
		{
			rpt_vars[i].disgorgetime = time(NULL) + 10; /* Do it 10 seconds later */
		        ast_cli(fd, "app_rpt struct dump requested for node %s\n",argv[2]);
		        return RESULT_SUCCESS;
		}
	}
	return RESULT_FAILURE;
}

/*
* Dump statistics onto console
*/

static int rpt_do_stats(int fd, int argc, char *argv[])
{
	int i,j,numoflinks;
	int dailytxtime, dailykerchunks;
	time_t now;
	int totalkerchunks, dailykeyups, totalkeyups, timeouts;
	int totalexecdcommands, dailyexecdcommands, hours, minutes, seconds;
	int uptime;
	long long totaltxtime;
	struct	rpt_link *l;
	char *listoflinks[MAX_STAT_LINKS];	
	char *lastdtmfcommand,*parrot_ena;
	char *tot_state, *ider_state, *patch_state;
	char *reverse_patch_state, *sys_ena, *tot_ena, *link_ena, *patch_ena;
	char *sch_ena, *input_signal, *called_number, *user_funs, *tail_type;
	char *iconns;
	struct rpt *myrpt;

	static char *not_applicable = "N/A";

	if(argc != 3)
		return RESULT_SHOWUSAGE;

	tot_state = ider_state = 
	patch_state = reverse_patch_state = 
	input_signal = not_applicable;
	called_number = lastdtmfcommand = NULL;

	time(&now);
	for(i = 0; i < nrpts; i++)
	{
		if (!strcmp(argv[2],rpt_vars[i].name)){
			/* Make a copy of all stat variables while locked */
			myrpt = &rpt_vars[i];
			rpt_mutex_lock(&myrpt->lock); /* LOCK */
			uptime = (int)(now - starttime);
			dailytxtime = myrpt->dailytxtime;
			totaltxtime = myrpt->totaltxtime;
			dailykeyups = myrpt->dailykeyups;
			totalkeyups = myrpt->totalkeyups;
			dailykerchunks = myrpt->dailykerchunks;
			totalkerchunks = myrpt->totalkerchunks;
			dailyexecdcommands = myrpt->dailyexecdcommands;
			totalexecdcommands = myrpt->totalexecdcommands;
			timeouts = myrpt->timeouts;

			/* Traverse the list of connected nodes */
			reverse_patch_state = "DOWN";
			numoflinks = 0;
			l = myrpt->links.next;
			while(l && (l != &myrpt->links)){
				if(numoflinks >= MAX_STAT_LINKS){
					ast_log(LOG_NOTICE,
					"maximum number of links exceeds %d in rpt_do_stats()!",MAX_STAT_LINKS);
					break;
				}
				if (l->name[0] == '0'){ /* Skip '0' nodes */
					reverse_patch_state = "UP";
					l = l->next;
					continue;
				}
				listoflinks[numoflinks] = ast_strdup(l->name);
				if(listoflinks[numoflinks] == NULL){
					break;
				}
				else{
					numoflinks++;
				}
				l = l->next;
			}

			if(myrpt->keyed)
				input_signal = "YES";
			else
				input_signal = "NO";

			if(myrpt->p.parrotmode)
				parrot_ena = "ENABLED";
			else
				parrot_ena = "DISABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].txdisable)
				sys_ena = "DISABLED";
			else
				sys_ena = "ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].totdisable)
				tot_ena = "DISABLED";
			else
				tot_ena = "ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].linkfundisable)
				link_ena = "DISABLED";
			else
				link_ena = "ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].autopatchdisable)
				patch_ena = "DISABLED";
			else
				patch_ena = "ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].schedulerdisable)
				sch_ena = "DISABLED";
			else
				sch_ena = "ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].userfundisable)
				user_funs = "DISABLED";
			else
				user_funs = "ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].alternatetail)
				tail_type = "ALTERNATE";
			else
				tail_type = "STANDARD";

			if(myrpt->p.s[myrpt->p.sysstate_cur].noincomingconns)
				iconns = "DISABLED";
			else
				iconns = "ENABLED";

			if(!myrpt->totimer)
				tot_state = "TIMED OUT!";
			else if(myrpt->totimer != myrpt->p.totime)
				tot_state = "ARMED";
			else
				tot_state = "RESET";

			if(myrpt->tailid)
				ider_state = "QUEUED IN TAIL";
			else if(myrpt->mustid)
				ider_state = "QUEUED FOR CLEANUP";
			else
				ider_state = "CLEAN";

			switch(myrpt->callmode){
				case 1:
					patch_state = "DIALING";
					break;
				case 2:
					patch_state = "CONNECTING";
					break;
				case 3:
					patch_state = "UP";
					break;

				case 4:
					patch_state = "CALL FAILED";
					break;

				default:
					patch_state = "DOWN";
			}

			if(strlen(myrpt->exten)){
				called_number = ast_strdup(myrpt->exten);
			}

			if(strlen(myrpt->lastdtmfcommand)){
				lastdtmfcommand = ast_strdup(myrpt->lastdtmfcommand);
			}
			rpt_mutex_unlock(&myrpt->lock); /* UNLOCK */

			ast_cli(fd, "************************ NODE %s STATISTICS *************************\n\n", myrpt->name);
			ast_cli(fd, "Selected system state............................: %d\n", myrpt->p.sysstate_cur);
			ast_cli(fd, "Signal on input..................................: %s\n", input_signal);
			ast_cli(fd, "System...........................................: %s\n", sys_ena);
			ast_cli(fd, "Parrot Mode......................................: %s\n", parrot_ena);
			ast_cli(fd, "Scheduler........................................: %s\n", sch_ena);
			ast_cli(fd, "Tail Time........................................: %s\n", tail_type);
			ast_cli(fd, "Time out timer...................................: %s\n", tot_ena);
			ast_cli(fd, "Incoming connections.............................: %s\n", iconns);
			ast_cli(fd, "Time out timer state.............................: %s\n", tot_state);
			ast_cli(fd, "Time outs since system initialization............: %d\n", timeouts);
			ast_cli(fd, "Identifier state.................................: %s\n", ider_state);
			ast_cli(fd, "Kerchunks today..................................: %d\n", dailykerchunks);
			ast_cli(fd, "Kerchunks since system initialization............: %d\n", totalkerchunks);
			ast_cli(fd, "Keyups today.....................................: %d\n", dailykeyups);
			ast_cli(fd, "Keyups since system initialization...............: %d\n", totalkeyups);
			ast_cli(fd, "DTMF commands today..............................: %d\n", dailyexecdcommands);
			ast_cli(fd, "DTMF commands since system initialization........: %d\n", totalexecdcommands);
			ast_cli(fd, "Last DTMF command executed.......................: %s\n",
			(lastdtmfcommand && strlen(lastdtmfcommand)) ? lastdtmfcommand : not_applicable);
			hours = dailytxtime/3600000;
			dailytxtime %= 3600000;
			minutes = dailytxtime/60000;
			dailytxtime %= 60000;
			seconds = dailytxtime/1000;
			dailytxtime %= 1000;

			ast_cli(fd, "TX time today....................................: %02d:%02d:%02d:%02d\n",
				hours, minutes, seconds, dailytxtime);

			hours = (int) totaltxtime/3600000;
			totaltxtime %= 3600000;
			minutes = (int) totaltxtime/60000;
			totaltxtime %= 60000;
			seconds = (int)  totaltxtime/1000;
			totaltxtime %= 1000;

			ast_cli(fd, "TX time since system initialization..............: %02d:%02d:%02d:%02d\n",
				 hours, minutes, seconds, (int) totaltxtime);

                       	hours = uptime/3600;
                        uptime %= 3600;
                        minutes = uptime/60;
                        uptime %= 60;

                        ast_cli(fd, "Uptime...........................................: %02d:%02d:%02d\n",
                                hours, minutes, uptime);

			ast_cli(fd, "Nodes currently connected to us..................: ");
                        if(!numoflinks){
  	                      ast_cli(fd,"<NONE>");
                        }
			else{
				for(j = 0 ;j < numoflinks; j++){
					ast_cli(fd, "%s", listoflinks[j]);
					if(j % 4 == 3){
						ast_cli(fd, "\n");
						ast_cli(fd, "                                                 : ");
					}	
					else{
						if((numoflinks - 1) - j  > 0)
							ast_cli(fd, ", ");
					}
				}
			}
			ast_cli(fd,"\n");

			ast_cli(fd, "Autopatch........................................: %s\n", patch_ena);
			ast_cli(fd, "Autopatch state..................................: %s\n", patch_state);
			ast_cli(fd, "Autopatch called number..........................: %s\n",
			(called_number && strlen(called_number)) ? called_number : not_applicable);
			ast_cli(fd, "Reverse patch/IAXRPT connected...................: %s\n", reverse_patch_state);
			ast_cli(fd, "User linking commands............................: %s\n", link_ena);
			ast_cli(fd, "User functions...................................: %s\n\n", user_funs);

			for(j = 0; j < numoflinks; j++){ /* ast_free() all link names */
				ast_free(listoflinks[j]);
			}
			if(called_number){
				ast_free(called_number);
			}
			if(lastdtmfcommand){
				ast_free(lastdtmfcommand);
			}
		        return RESULT_SUCCESS;
		}
	}
	return RESULT_FAILURE;
}

/*
* Link stats function
*/

static int rpt_do_lstats(int fd, int argc, char *argv[])
{
	int i,j;
	char *connstate;
	struct rpt *myrpt;
	struct rpt_link *l;
	struct rpt_lstat *s,*t;
	struct rpt_lstat s_head;
	if(argc != 3)
		return RESULT_SHOWUSAGE;

	s = NULL;
	s_head.next = &s_head;
	s_head.prev = &s_head;

	for(i = 0; i < nrpts; i++)
	{
		if (!strcmp(argv[2],rpt_vars[i].name)){
			/* Make a copy of all stat variables while locked */
			myrpt = &rpt_vars[i];
			rpt_mutex_lock(&myrpt->lock); /* LOCK */
			/* Traverse the list of connected nodes */
			j = 0;
			l = myrpt->links.next;
			while(l && (l != &myrpt->links)){
				if (l->name[0] == '0'){ /* Skip '0' nodes */
					l = l->next;
					continue;
				}
				if((s = (struct rpt_lstat *) ast_malloc(sizeof(struct rpt_lstat))) == NULL){
					ast_log(LOG_ERROR, "Malloc failed in rpt_do_lstats\n");
					rpt_mutex_unlock(&myrpt->lock); /* UNLOCK */
					return RESULT_FAILURE;
				}
				memset(s, 0, sizeof(struct rpt_lstat));
				strncpy(s->name, l->name, MAXREMSTR - 1);
				if (l->chan) pbx_substitute_variables_helper(l->chan, "${IAXPEER(CURRENTCHANNEL)}", s->peer, MAXPEERSTR - 1);
				else strcpy(s->peer,"(none)");
				s->mode = l->mode;
				s->outbound = l->outbound;
				s->reconnects = l->reconnects;
				s->connecttime = l->connecttime;
				s->thisconnected = l->thisconnected;
				memcpy(s->chan_stat,l->chan_stat,NRPTSTAT * sizeof(struct rpt_chan_stat));
				insque((struct qelem *) s, (struct qelem *) s_head.next);
				memset(l->chan_stat,0,NRPTSTAT * sizeof(struct rpt_chan_stat));
				l = l->next;
			}
			rpt_mutex_unlock(&myrpt->lock); /* UNLOCK */
			ast_cli(fd, "NODE      PEER                RECONNECTS  DIRECTION  CONNECT TIME        CONNECT STATE\n");
			ast_cli(fd, "----      ----                ----------  ---------  ------------        -------------\n");

			for(s = s_head.next; s != &s_head; s = s->next){
				int hours, minutes, seconds;
				long long connecttime = s->connecttime;
				char conntime[21];
				hours = connecttime/3600000L;
				connecttime %= 3600000L;
				minutes =  connecttime/60000L;
				connecttime %= 60000L;
				seconds =  connecttime/1000L;
				connecttime %= 1000L;
				snprintf(conntime, 20, "%02d:%02d:%02d:%02d",
					hours, minutes, seconds, (int) connecttime);
				conntime[20] = 0;
				if(s->thisconnected)
					connstate  = "ESTABLISHED";
				else
					connstate = "CONNECTING";
				ast_cli(fd, "%-10s%-20s%-12d%-11s%-20s%-20s\n",
					s->name, s->peer, s->reconnects, (s->outbound)? "OUT":"IN", conntime, connstate);
			}	
			/* destroy our local link queue */
			s = s_head.next;
			while(s != &s_head){
				t = s;
				s = s->next;
				remque((struct qelem *)t);
				ast_free(t);
			}			
			return RESULT_SUCCESS;
		}
	}
	return RESULT_FAILURE;
}

static int rpt_do_xnode(int fd, int argc, char *argv[])
{
	int i,j;
	char ns;
	char lbuf[MAXLINKLIST],*strs[MAXLINKLIST];
	struct rpt *myrpt;
	struct ast_var_t *newvariable;
	char *connstate;
	struct rpt_link *l;
	struct rpt_lstat *s,*t;
	struct rpt_lstat s_head;
	if(argc != 3)
		return RESULT_SHOWUSAGE;

	s = NULL;
	s_head.next = &s_head;
	s_head.prev = &s_head;


	char *parrot_ena, *sys_ena, *tot_ena, *link_ena, *patch_ena, *patch_state;
	char *sch_ena, *user_funs, *tail_type, *iconns, *tot_state, *ider_state, *tel_mode; 

	for(i = 0; i < nrpts; i++)
	{
		if (!strcmp(argv[2],rpt_vars[i].name)){
			/* Make a copy of all stat variables while locked */
			myrpt = &rpt_vars[i];
			rpt_mutex_lock(&myrpt->lock); /* LOCK */

//### GET RPT STATUS STATES WHILE LOCKED ########################
			if(myrpt->p.parrotmode)
				parrot_ena = "1";	//"ENABLED";
			else
				parrot_ena = "0";	//"DISABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].txdisable)
				sys_ena = "0";	//"DISABLED";
			else
				sys_ena = "1";	//"ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].totdisable)
				tot_ena = "0";	//"DISABLED";
			else
				tot_ena = "1";	//"ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].linkfundisable)
				link_ena = "0";	//"DISABLED";
			else
				link_ena = "1";	//"ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].autopatchdisable)
				patch_ena = "0";	//"DISABLED";
			else
				patch_ena = "1";	//"ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].schedulerdisable)
				sch_ena = "0";	//"DISABLED";
			else
				sch_ena = "1";	//"ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].userfundisable)
				user_funs = "0";	//"DISABLED";
			else
				user_funs = "1";	//"ENABLED";

			if(myrpt->p.s[myrpt->p.sysstate_cur].alternatetail)
				tail_type = "1";	//"ALTERNATE";
			else
				tail_type = "0";	//"STANDARD";

			if(myrpt->p.s[myrpt->p.sysstate_cur].noincomingconns)
				iconns = "0";	//"DISABLED";
			else
				iconns = "1";	//"ENABLED";

			if(!myrpt->totimer)
				tot_state = "0";	//"TIMED OUT!";
			else if(myrpt->totimer != myrpt->p.totime)
				tot_state = "1";	//"ARMED";
			else
				tot_state = "2";	//"RESET";

			if(myrpt->tailid)
				ider_state = "0";	//"QUEUED IN TAIL";
			else if(myrpt->mustid)
				ider_state = "1";	//"QUEUED FOR CLEANUP";
			else
				ider_state = "2";	//"CLEAN";


			switch(myrpt->callmode){
				case 1:
					patch_state = "0";		//"DIALING";
					break;
				case 2:
					patch_state = "1";		//"CONNECTING";
					break;
				case 3:
					patch_state = "2";		//"UP";
					break;

				case 4:
					patch_state = "3";		//"CALL FAILED";
					break;

				default:
					patch_state = "4";		//"DOWN";
			}


			if (myrpt->p.telemdynamic)
			{
				if(myrpt->telemmode == 0x7fffffff)
					tel_mode = "1";
				else if(myrpt->telemmode == 0x00)
					tel_mode = "0";
				else
					tel_mode = "2";
			}
			else{
				tel_mode= "3";
			}


//### GET CONNECTED NODE INFO ####################
			// Traverse the list of connected nodes 

			__mklinklist(myrpt,NULL,lbuf,0);

			j = 0;
			l = myrpt->links.next;
			while(l && (l != &myrpt->links)){
				if (l->name[0] == '0'){ // Skip '0' nodes 
					l = l->next;
					continue;
				}
				if((s = (struct rpt_lstat *) ast_malloc(sizeof(struct rpt_lstat))) == NULL){
					ast_log(LOG_ERROR, "Malloc failed in rpt_do_lstats\n");
					rpt_mutex_unlock(&myrpt->lock); // UNLOCK 
					return RESULT_FAILURE;
				}
				memset(s, 0, sizeof(struct rpt_lstat));
				strncpy(s->name, l->name, MAXREMSTR - 1);
				if (l->chan) pbx_substitute_variables_helper(l->chan, "${IAXPEER(CURRENTCHANNEL)}", s->peer, MAXPEERSTR - 1);
				else strcpy(s->peer,"(none)");
				s->mode = l->mode;
				s->outbound = l->outbound;
				s->reconnects = l->reconnects;
				s->connecttime = l->connecttime;
				s->thisconnected = l->thisconnected;
				memcpy(s->chan_stat,l->chan_stat,NRPTSTAT * sizeof(struct rpt_chan_stat));
				insque((struct qelem *) s, (struct qelem *) s_head.next);
				memset(l->chan_stat,0,NRPTSTAT * sizeof(struct rpt_chan_stat));
				l = l->next;
			}
			rpt_mutex_unlock(&myrpt->lock); // UNLOCK 
			for(s = s_head.next; s != &s_head; s = s->next){
				int hours, minutes, seconds;
				long long connecttime = s->connecttime;
				char conntime[21];
				hours = connecttime/3600000L;
				connecttime %= 3600000L;
				minutes = connecttime/60000L;
				connecttime %= 60000L;
				seconds = (int)  connecttime/1000L;
				connecttime %= 1000;
				snprintf(conntime, 20, "%02d:%02d:%02d",
					hours, minutes, seconds);
				conntime[20] = 0;
				if(s->thisconnected)
					connstate  = "ESTABLISHED";
				else
					connstate = "CONNECTING";
				ast_cli(fd, "%-10s%-20s%-12d%-11s%-20s%-20s~",
					s->name, s->peer, s->reconnects, (s->outbound)? "OUT":"IN", conntime, connstate);
			}	
			ast_cli(fd,"\n\n");
			// destroy our local link queue 
			s = s_head.next;
			while(s != &s_head){
				t = s;
				s = s->next;
				remque((struct qelem *)t);
				ast_free(t);
			}	

//### GET ALL LINKED NODES INFO ####################
			/* parse em */
			ns = finddelim(lbuf,strs,MAXLINKLIST);
			/* sort em */
			if (ns) qsort((void *)strs,ns,sizeof(char *),mycompar);
			for(j = 0 ;; j++){
				if(!strs[j]){
					if(!j){
						ast_cli(fd,"<NONE>");
					}
					break;
				}
				ast_cli(fd, "%s", strs[j]);
				if(strs[j + 1])
					ast_cli(fd, ", ");

			}
			ast_cli(fd,"\n\n");

//### GET VARIABLES INFO ####################
			j = 0;
			ast_channel_lock(rpt_vars[i].rxchannel);
			AST_LIST_TRAVERSE (&rpt_vars[i].rxchannel->varshead, newvariable, entries) {
				j++;
				ast_cli(fd,"%s=%s\n", ast_var_name(newvariable), ast_var_value(newvariable));
			}
			ast_channel_unlock(rpt_vars[i].rxchannel);
			ast_cli(fd,"\n");

//### OUTPUT RPT STATUS STATES ##############
			ast_cli(fd, "parrot_ena=%s\n", parrot_ena);
			ast_cli(fd, "sys_ena=%s\n", sys_ena);
			ast_cli(fd, "tot_ena=%s\n", tot_ena);
			ast_cli(fd, "link_ena=%s\n", link_ena);
			ast_cli(fd, "patch_ena=%s\n", patch_ena);
			ast_cli(fd, "patch_state=%s\n", patch_state);
			ast_cli(fd, "sch_ena=%s\n", sch_ena);
			ast_cli(fd, "user_funs=%s\n", user_funs);
			ast_cli(fd, "tail_type=%s\n", tail_type);
			ast_cli(fd, "iconns=%s\n", iconns);
			ast_cli(fd, "tot_state=%s\n", tot_state);
			ast_cli(fd, "ider_state=%s\n", ider_state);
			ast_cli(fd, "tel_mode=%s\n\n", tel_mode);

			return RESULT_SUCCESS;
		}
	}
	return RESULT_FAILURE;
}

/*
* List all nodes connected, directly or indirectly
*/

static int rpt_do_nodes(int fd, int argc, char *argv[])
{
	int i,j;
	char ns;
	char lbuf[MAXLINKLIST],*strs[MAXLINKLIST];
	struct rpt *myrpt;
	if(argc != 3)
		return RESULT_SHOWUSAGE;

	for(i = 0; i < nrpts; i++)
	{
		if (!strcmp(argv[2],rpt_vars[i].name)){
			/* Make a copy of all stat variables while locked */
			myrpt = &rpt_vars[i];
			rpt_mutex_lock(&myrpt->lock); /* LOCK */
			__mklinklist(myrpt,NULL,lbuf,0);
			rpt_mutex_unlock(&myrpt->lock); /* UNLOCK */
			/* parse em */
			ns = finddelim(lbuf,strs,MAXLINKLIST);
			/* sort em */
			if (ns) qsort((void *)strs,ns,sizeof(char *),mycompar);
			ast_cli(fd,"\n");
			ast_cli(fd, "************************* CONNECTED NODES *************************\n\n");
			for(j = 0 ;; j++){
				if(!strs[j]){
					if(!j){
						ast_cli(fd,"<NONE>");
					}
					break;
				}
				ast_cli(fd, "%s", strs[j]);
				if(j % 8 == 7){
					ast_cli(fd, "\n");
				}
				else{
					if(strs[j + 1])
						ast_cli(fd, ", ");
				}
			}
			ast_cli(fd,"\n\n");
			return RESULT_SUCCESS;
		}
	}
	return RESULT_FAILURE;
}

/*
* List all locally configured nodes
*/

static int rpt_do_local_nodes(int fd, int argc, char *argv[])
{

    int i;
    ast_cli(fd, "\nNode\n----\n");
    for (i=0; i< nrpts; i++)
    {
	if (rpt_vars[i].name[0])
	        ast_cli(fd, "%s\n", rpt_vars[i].name);        
    } /* for i */
    ast_cli(fd,"\n");
    return RESULT_SUCCESS;
} 


/*
* reload vars 
*/

static int rpt_do_reload(int fd, int argc, char *argv[])
{
        if (argc > 2) return RESULT_SHOWUSAGE;

	reload();

	return RESULT_FAILURE;
}

/*
* restart app_rpt
*/
                                                                                                                                 
static int rpt_do_restart(int fd, int argc, char *argv[])
{
int	i;

        if (argc > 2) return RESULT_SHOWUSAGE;
	for(i = 0; i < nrpts; i++)
	{
		if (rpt_vars[i].rxchannel) ast_softhangup(rpt_vars[i].rxchannel,AST_SOFTHANGUP_DEV);
	}
	return RESULT_FAILURE;
}


/*
* send an app_rpt DTMF function from the CLI
*/
                                                                                                                                 
static int rpt_do_fun(int fd, int argc, char *argv[])
{
	int	i,busy=0;

        if (argc != 4) return RESULT_SHOWUSAGE;

	for(i = 0; i < nrpts; i++){
		if(!strcmp(argv[2], rpt_vars[i].name)){
			struct rpt *myrpt = &rpt_vars[i];
			rpt_mutex_lock(&myrpt->lock);
			if ((MAXMACRO - strlen(myrpt->macrobuf)) < strlen(argv[3])){
				rpt_mutex_unlock(&myrpt->lock);
				busy=1;
			}
			if(!busy){
				myrpt->macrotimer = MACROTIME;
				strncat(myrpt->macrobuf,argv[3],MAXMACRO - 1);
			}
			rpt_mutex_unlock(&myrpt->lock);
		}
	}
	if(busy){
		ast_cli(fd, "Function decoder busy");
	}
	return RESULT_FAILURE;
}
/*
* send an Audio File from the CLI
*/
                                                                                                                                 
static int rpt_do_playback(int fd, int argc, char *argv[])
{
	int	i;

        if (argc != 4) return RESULT_SHOWUSAGE;

	for(i = 0; i < nrpts; i++){
		if(!strcmp(argv[2], rpt_vars[i].name)){
			struct rpt *myrpt = &rpt_vars[i];
			rpt_telemetry(myrpt,PLAYBACK,argv[3]);			
		}
	}
	return RESULT_SUCCESS;
}

static int rpt_do_localplay(int fd, int argc, char *argv[])
{
        int     i;

        if (argc != 4) return RESULT_SHOWUSAGE;

        for(i = 0; i < nrpts; i++){
                if(!strcmp(argv[2], rpt_vars[i].name)){
                        struct rpt *myrpt = &rpt_vars[i];
                        rpt_telemetry(myrpt,LOCALPLAY,argv[3]);
                }
        }
        return RESULT_SUCCESS;
}

static int rpt_do_sendtext(int fd, int argc, char *argv[])
{
        int     i;
	struct	rpt_link *l;
	char str[MAX_TEXTMSG_SIZE];

        if (argc < 5) return RESULT_SHOWUSAGE;

	string_toupper(argv[2]);
	string_toupper(argv[3]);
	snprintf(str,sizeof(str) - 1,"M %s %s ",argv[2],argv[3]);
	for(i = 4; i < argc; i++)
	{
		if (i > 3) strncat(str," ",sizeof(str) - 1);
		strncat(str,argv[i],sizeof(str) - 1);
	}	
        for(i = 0; i < nrpts; i++)
	{
                if(!strcmp(argv[2], rpt_vars[i].name))
		{
                        struct rpt *myrpt = &rpt_vars[i];
			rpt_mutex_lock(&myrpt->lock);
			l = myrpt->links.next;
			/* otherwise, send it to all of em */
			while(l != &myrpt->links)
			{
				if (l->name[0] == '0') 
				{
					l = l->next;
					continue;
				}
				if (l->chan) ast_sendtext(l->chan,str);
				l = l->next;
			}
			rpt_mutex_unlock(&myrpt->lock);
		}
	}
        return RESULT_SUCCESS;
}

//## Paging function

static int rpt_do_page(int fd, int argc, char *argv[])
{
        int     i;
	char str[MAX_TEXTMSG_SIZE];
	struct rpt_tele *telem;

        if (argc < 7) return RESULT_SHOWUSAGE;

	string_toupper(argv[2]);
	string_toupper(argv[3]);
	string_toupper(argv[4]);
	string_toupper(argv[5]);
	snprintf(str,sizeof(str) - 1,"PAGE %s %s %s ",argv[3],argv[4],argv[5]);
	for(i = 6; i < argc; i++)
	{
		if (i > 5) strncat(str," ",sizeof(str) - 1);
		strncat(str,argv[i],sizeof(str) - 1);
	}	
        for(i = 0; i < nrpts; i++)
	{
                if(!strcmp(argv[2], rpt_vars[i].name))
		{
                        struct rpt *myrpt = &rpt_vars[i];
			/* ignore if not a USB channel */
			if ((strncasecmp(myrpt->rxchannel->name,"radio/", 6) == 0) &&
			    (strncasecmp(myrpt->rxchannel->name,"voter/", 6) == 0) &&
			    (strncasecmp(myrpt->rxchannel->name,"simpleusb/", 10) == 0)) return RESULT_SUCCESS;
			telem = myrpt->tele.next;
			while(telem != &myrpt->tele)
			{
				if (((telem->mode == ID) || (telem->mode == ID1) || 
					(telem->mode == IDTALKOVER)) && (!telem->killed))
				{
					if (telem->chan) ast_softhangup(telem->chan, AST_SOFTHANGUP_DEV); /* Whoosh! */
					telem->killed = 1;
					myrpt->deferid = 1;
				}
				telem = telem->next;
			}
			gettimeofday(&myrpt->paging,NULL);
			ast_sendtext(myrpt->rxchannel,str);
		}
	}
        return RESULT_SUCCESS;
}

//## Send to all nodes

static int rpt_do_sendall(int fd, int argc, char *argv[])
{
        int     i;
	struct	rpt_link *l;
	char str[MAX_TEXTMSG_SIZE];

        if (argc < 4) return RESULT_SHOWUSAGE;

	string_toupper(argv[2]);
	snprintf(str,sizeof(str) - 1,"M %s 0 ",argv[2]);
	for(i = 3; i < argc; i++)
	{
		if (i > 3) strncat(str," ",sizeof(str) - 1);
		strncat(str,argv[i],sizeof(str) - 1);
	}	
        for(i = 0; i < nrpts; i++)
	{
                if(!strcmp(argv[2], rpt_vars[i].name))
		{
                        struct rpt *myrpt = &rpt_vars[i];
			rpt_mutex_lock(&myrpt->lock);
			l = myrpt->links.next;
			/* otherwise, send it to all of em */
			while(l != &myrpt->links)
			{
				if (l->name[0] == '0') 
				{
					l = l->next;
					continue;
				}
				if (l->chan) ast_sendtext(l->chan,str);
				l = l->next;
			}
			rpt_mutex_unlock(&myrpt->lock);
		}
	}
        return RESULT_SUCCESS;
}


/*
	the convention is that macros in the data from the rpt( application
	are all at the end of the data, separated by the | and start with a *
	when put into the macro buffer, the characters have their high bit
	set so the macro processor knows they came from the application data
	and to use the alt-functions table.
	sph:
*/
static int rpt_push_alt_macro(struct rpt *myrpt, char *sptr)
{
	int	busy=0;

	rpt_mutex_lock(&myrpt->lock);
	if ((MAXMACRO - strlen(myrpt->macrobuf)) < strlen(sptr)){
		rpt_mutex_unlock(&myrpt->lock);
		busy=1;
	}
	if(!busy){
		int x;
		if (debug)ast_log(LOG_NOTICE, "rpt_push_alt_macro %s\n",sptr);
		myrpt->macrotimer = MACROTIME;
		for(x = 0; *(sptr + x); x++)
		    myrpt->macrobuf[x] = *(sptr + x) | 0x80;
		*(sptr + x) = 0;
	}
	rpt_mutex_unlock(&myrpt->lock);

	if(busy)ast_log(LOG_WARNING, "Function decoder busy on app_rpt command macro.\n");

	return busy;
}
/*
	allows us to test rpt() application data commands
*/
static int rpt_do_fun1(int fd, int argc, char *argv[])
{
	int	i;

    if (argc != 4) return RESULT_SHOWUSAGE;

	for(i = 0; i < nrpts; i++){
		if(!strcmp(argv[2], rpt_vars[i].name)){
			struct rpt *myrpt = &rpt_vars[i];
			rpt_push_alt_macro(myrpt,argv[3]);
		}
	}
	return RESULT_FAILURE;
}
/*
* send an app_rpt **command** from the CLI
*/

static int rpt_do_cmd(int fd, int argc, char *argv[])
{
	int i, l;
	int busy=0;
	int maxActions = sizeof(function_table)/sizeof(struct function_table_tag);

	int thisRpt = -1;
	int thisAction = -1;
	struct rpt *myrpt = NULL;
	if (argc != 6) return RESULT_SHOWUSAGE;
	
	for(i = 0; i < nrpts; i++)
	{
		if(!strcmp(argv[2], rpt_vars[i].name))
		{
			thisRpt = i;
			myrpt = &rpt_vars[i];
			break;
		} /* if !strcmp... */
	} /* for i */

	if (thisRpt < 0)
	{
		ast_cli(fd, "Unknown node number %s.\n", argv[2]);
		return RESULT_FAILURE;
	} /* if thisRpt < 0 */
	
	/* Look up the action */
	l = strlen(argv[3]);
	for(i = 0 ; i < maxActions; i++)
	{
		if(!strncasecmp(argv[3], function_table[i].action, l))
		{
			thisAction = i;
			break;
		} /* if !strncasecmp... */
	} /* for i */
	
	if (thisAction < 0)
	{
		ast_cli(fd, "Unknown action name %s.\n", argv[3]);
		return RESULT_FAILURE;
	} /* if thisAction < 0 */

	/* at this point, it looks like all the arguments make sense... */

	rpt_mutex_lock(&myrpt->lock);

	if (rpt_vars[thisRpt].cmdAction.state == CMD_STATE_IDLE)
	{
		rpt_vars[thisRpt].cmdAction.state = CMD_STATE_BUSY;
		rpt_vars[thisRpt].cmdAction.functionNumber = thisAction;
		snprintf(rpt_vars[thisRpt].cmdAction.param, MAXDTMF, "%s,%s",argv[4], argv[5]);
		strncpy(rpt_vars[thisRpt].cmdAction.digits, argv[5], MAXDTMF);
		rpt_vars[thisRpt].cmdAction.command_source = SOURCE_RPT;
		rpt_vars[thisRpt].cmdAction.state = CMD_STATE_READY;
	} /* if (rpt_vars[thisRpt].cmdAction.state == CMD_STATE_IDLE */
	else
	{
		busy = 1;
	} /* if (rpt_vars[thisRpt].cmdAction.state == CMD_STATE_IDLE */
	rpt_mutex_unlock(&myrpt->lock);

	return (busy ? RESULT_FAILURE : RESULT_SUCCESS);
} /* rpt_do_cmd() */

/*
* set a node's main channel variable from the command line 
*/
static int rpt_do_setvar(int fd, int argc, char *argv[])
{
	char *name, *value;
	int i,x,thisRpt = -1;

	if (argc < 4) return RESULT_SHOWUSAGE;
	for(i = 0; i < nrpts; i++)
	{
		if(!strcmp(argv[2], rpt_vars[i].name))
		{
			thisRpt = i;
			break;
		} 
	} 

	if (thisRpt < 0)
	{
		ast_cli(fd, "Unknown node number %s.\n", argv[2]);
		return RESULT_FAILURE;
	} 
	
	for (x = 3; x < argc; x++) {
		name = argv[x];
		if ((value = strchr(name, '='))) {
			*value++ = '\0';
			pbx_builtin_setvar_helper(rpt_vars[thisRpt].rxchannel, name, value);
		} else
			ast_log(LOG_WARNING, "Ignoring entry '%s' with no = \n", name);
	}
	return(0);
}

/*
* Display a node's main channel variables from the command line 
*/
static int rpt_do_showvars(int fd, int argc, char *argv[])
{
	int i,thisRpt = -1;
	struct ast_var_t *newvariable;

	if (argc != 3) return RESULT_SHOWUSAGE;
	for(i = 0; i < nrpts; i++)
	{
		if(!strcmp(argv[2], rpt_vars[i].name))
		{
			thisRpt = i;
			break;
		} 
	} 

	if (thisRpt < 0)
	{
		ast_cli(fd, "Unknown node number %s.\n", argv[2]);
		return RESULT_FAILURE;
	} 
	i = 0;
	ast_cli(fd,"Variable listing for node %s:\n",argv[2]);
	ast_channel_lock(rpt_vars[thisRpt].rxchannel);
	AST_LIST_TRAVERSE (&rpt_vars[thisRpt].rxchannel->varshead, newvariable, entries) {
		i++;
		ast_cli(fd,"   %s=%s\n", ast_var_name(newvariable), ast_var_value(newvariable));
	}
	ast_channel_unlock(rpt_vars[thisRpt].rxchannel);
	ast_cli(fd,"    -- %d variables\n", i);
	return(0);
}

/*
* Do ASL Stuff
*/


static int rpt_do_asl(int fd, int argc, char *argv[])
{
       double warpone = 75139293848.398696166028333356763;
       double warpfactor = 1.0;

       if (argc > 2) return RESULT_SHOWUSAGE;
       if ((argc > 1) && (sscanf(argv[1],"%lf",&warpfactor) != 1))
                return RESULT_SHOWUSAGE;

       ast_cli(fd, "This command doe not do anything\n");
       ast_cli(fd, "73 Steve N4IRS\n",
               warpfactor * warpfactor * warpfactor * warpone,warpfactor);
       ast_cli(fd,"Replace a command that does nothing but waste space. RIP WB6NIL\n");
       return RESULT_SUCCESS;
}


static int play_tone_pair(struct ast_channel *chan, int f1, int f2, int duration, int amplitude)
{
	int res;

        if ((res = ast_tonepair_start(chan, f1, f2, duration, amplitude)))
                return res;
                                                                                                                                            
        while(chan->generatordata) {
		if (ast_safe_sleep(chan,1)) return -1;
	}

        return 0;
}

static int play_tone(struct ast_channel *chan, int freq, int duration, int amplitude)
{
	return play_tone_pair(chan, freq, 0, duration, amplitude);
}

#ifdef	NEW_ASTERISK

/*
 *  Hooks for CLI functions
 */
 
static char *res2cli(int r)

{
	switch (r)
	{
	    case RESULT_SUCCESS:
		return(CLI_SUCCESS);
	    case RESULT_SHOWUSAGE:
		return(CLI_SHOWUSAGE);
	    default:
		return(CLI_SUCCESS);
	}
}

static char *handle_cli_debug(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt debug level";
                e->usage = debug_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_debug(a->fd,a->argc,a->argv));
}

static char *handle_cli_dump(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt dump level";
                e->usage = dump_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_dump(a->fd,a->argc,a->argv));
}


static char *handle_cli_stats(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt stats";
                e->usage = dump_stats;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_stats(a->fd,a->argc,a->argv));
}

static char *handle_cli_nodes(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt nodes";
                e->usage = dump_nodes;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_nodes(a->fd,a->argc,a->argv));
}

static char *handle_cli_xnode(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt xnode";
                e->usage = dump_xnode;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_xnode(a->fd,a->argc,a->argv));
}

static char *handle_cli_local_nodes(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt localnodes";
                e->usage = usage_local_nodes;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_local_nodes(a->fd,a->argc,a->argv));
}

static char *handle_cli_lstats(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt lstats";
                e->usage = dump_lstats;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_lstats(a->fd,a->argc,a->argv));
}

static char *handle_cli_reload(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt reload";
                e->usage = reload_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_reload(a->fd,a->argc,a->argv));
}

static char *handle_cli_restart(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt restart";
                e->usage = restart_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_restart(a->fd,a->argc,a->argv));
}

static char *handle_cli_fun(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt fun";
                e->usage = fun_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_fun(a->fd,a->argc,a->argv));
}

static char *handle_cli_playback(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt playback";
                e->usage = playback_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_playback(a->fd,a->argc,a->argv));
}

static char *handle_cli_fun1(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt fun1";
                e->usage = fun_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_fun1(a->fd,a->argc,a->argv));
}

static char *handle_cli_cmd(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt cmd";
                e->usage = cmd_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_cmd(a->fd,a->argc,a->argv));
}

static char *handle_cli_setvar(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt setvar";
                e->usage = setvar_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_setvar(a->fd,a->argc,a->argv));
}

static char *handle_cli_showvars(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt showvars";
                e->usage = showvars_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_showvars(a->fd,a->argc,a->argv));
}

static char *handle_cli_asl(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "asl";
                e->usage = asl_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_asl(a->fd,a->argc,a->argv));
}

static char *handle_cli_localplay(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt localplay";
                e->usage = localplay_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_localplay(a->fd,a->argc,a->argv));
}

static char *handle_cli_sendall(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt sendall";
                e->usage = sendall_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_sendall(a->fd,a->argc,a->argv));
}

static char *handle_cli_sendtext(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt sendtext";
                e->usage = sendtext_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_sendtext(a->fd,a->argc,a->argv));
}

static char *handle_cli_page(struct ast_cli_entry *e,
	int cmd, struct ast_cli_args *a)
{
        switch (cmd) {
        case CLI_INIT:
                e->command = "rpt page";
                e->usage = page_usage;
                return NULL;
        case CLI_GENERATE:
                return NULL;
	}
	return res2cli(rpt_do_page(a->fd,a->argc,a->argv));
}

static struct ast_cli_entry rpt_cli[] = {
	AST_CLI_DEFINE(handle_cli_debug,"Enable app_rpt debugging"),
	AST_CLI_DEFINE(handle_cli_dump,"Dump app_rpt structs for debugging"),
	AST_CLI_DEFINE(handle_cli_stats,"Dump node statistics"),
	AST_CLI_DEFINE(handle_cli_nodes,"Dump node list"),
	AST_CLI_DEFINE(handle_cli_xnode,"Dump extended node info"),
	AST_CLI_DEFINE(handle_cli_local_nodes,	"Dump list of local node numbers"),
	AST_CLI_DEFINE(handle_cli_lstats,"Dump link statistics"),
	AST_CLI_DEFINE(handle_cli_reload,"Reload app_rpt config"),
	AST_CLI_DEFINE(handle_cli_restart,"Restart app_rpt"),
	AST_CLI_DEFINE(handle_cli_playback,"Play Back an Audio File"),
	AST_CLI_DEFINE(handle_cli_fun,"Execute a DTMF function"),
	AST_CLI_DEFINE(handle_cli_fun1,"Execute a DTMF function"),
	AST_CLI_DEFINE(handle_cli_cmd,"Execute a DTMF function"),
	AST_CLI_DEFINE(handle_cli_setvar,"Set an Asterisk channel variable"),
	AST_CLI_DEFINE(handle_cli_showvars,"Display Asterisk channel variables"),
	AST_CLI_DEFINE(handle_cli_localplay,"Playback an audio file (local)"),
	AST_CLI_DEFINE(handle_cli_sendall,"Send a Text message to all connected nodes"),
	AST_CLI_DEFINE(handle_cli_sendtext,"Send a Text message to a specified nodes"),
	AST_CLI_DEFINE(handle_cli_asl,"Do ASL stuff"),
	AST_CLI_DEFINE(handle_cli_page,"Send a page to a user on a node")
};

#endif

/*
 * End of CLI hooks
 */

static int morse_cat(char *str, int freq, int duration)
{
	char *p;
	int len;

	if(!str)
		return -1;

	len = strlen(str);	
	p = str+len;

	if(len){
		*p++ = ',';
		*p = '\0';
	}

	snprintf(p, 62,"!%d/%d", freq, duration);
 
	return 0;
}


//## Convert string into morse code

static int send_morse(struct ast_channel *chan, char *string, int speed, int freq, int amplitude)
{

static struct morse_bits mbits[] = {
		{0, 0}, /* SPACE */
		{0, 0}, 
		{6, 18},/* " */
		{0, 0},
		{7, 72},/* $ */
		{0, 0},
		{0, 0},
		{6, 30},/* ' */
		{5, 13},/* ( */
		{6, 29},/* ) */
		{0, 0},
		{5, 10},/* + */
		{6, 51},/* , */
		{6, 33},/* - */
		{6, 42},/* . */
		{5, 9}, /* / */
		{5, 31},/* 0 */
		{5, 30},/* 1 */
		{5, 28},/* 2 */
		{5, 24},/* 3 */
		{5, 16},/* 4 */
		{5, 0}, /* 5 */
		{5, 1}, /* 6 */
		{5, 3}, /* 7 */
		{5, 7}, /* 8 */
		{5, 15},/* 9 */
		{6, 7}, /* : */
		{6, 21},/* ; */
		{0, 0},
		{5, 33},/* = */
		{0, 0},
		{6, 12},/* ? */
		{0, 0},
        	{2, 2}, /* A */
 		{4, 1}, /* B */
		{4, 5}, /* C */
		{3, 1}, /* D */
		{1, 0}, /* E */
		{4, 4}, /* F */
		{3, 3}, /* G */
		{4, 0}, /* H */
		{2, 0}, /* I */
		{4, 14},/* J */
		{3, 5}, /* K */
		{4, 2}, /* L */
		{2, 3}, /* M */
		{2, 1}, /* N */
		{3, 7}, /* O */
		{4, 6}, /* P */
		{4, 11},/* Q */
		{3, 2}, /* R */
		{3, 0}, /* S */
		{1, 1}, /* T */
		{3, 4}, /* U */
		{4, 8}, /* V */
		{3, 6}, /* W */
		{4, 9}, /* X */
		{4, 13},/* Y */
		{4, 3}  /* Z */
	};


	int dottime;
	int dashtime;
	int intralettertime;
	int interlettertime;
	int interwordtime;
	int len, ddcomb;
	int res;
	int c;
	char *str = NULL;
			
	res = 0;


	str = ast_malloc(12*8*strlen(string)); /* 12 chrs/element max, 8 elements/letter max */
	if(!str)
		return -1;
	str[0] = '\0';
	
	/* Approximate the dot time from the speed arg. */
	
	dottime = 900/speed;
	
	/* Establish timing releationships */
	
	dashtime = 3 * dottime;
	intralettertime = dottime;
	interlettertime = dottime * 4 ;
	interwordtime = dottime * 7;
	
	for(;(*string) && (!res); string++){
	
		c = *string;
		
		/* Convert lower case to upper case */
		
		if((c >= 'a') && (c <= 'z'))
			c -= 0x20;
		
		/* Can't deal with any char code greater than Z, skip it */
		
		if(c  > 'Z')
			continue;
		
		/* If space char, wait the inter word time */
					
		if(c == ' '){
			if(!res){
				if((res = morse_cat(str, 0, interwordtime)))
					break;
			}
			continue;
		}
		
		/* Subtract out control char offset to match our table */
		
		c -= 0x20;
		
		/* Get the character data */
		
		len = mbits[c].len;
		ddcomb = mbits[c].ddcomb;
		
		/* Send the character */
		
		for(; len ; len--){
			if(!res)
				res = morse_cat(str, freq, (ddcomb & 1) ? dashtime : dottime);
			if(!res)
				res = morse_cat(str, 0, intralettertime);
			ddcomb >>= 1;
		}
		
		/* Wait the interletter time */
		
		if(!res)
			res = morse_cat(str, 0, interlettertime - intralettertime);

	}
	
	/* Wait for all the characters to be sent */

	if(!res){
		if(debug > 4)
			ast_log(LOG_NOTICE,"Morse string: %s\n", str);
		ast_safe_sleep(chan,100);
		ast_playtones_start(chan, amplitude, str, 0);
		while(chan->generatordata){
			if(ast_safe_sleep(chan, 20)){
				res = -1;
				break;
			}
		}		
				 
	}
	if(str)
		ast_free(str);
	return res;
}

//# Send telemetry tones

static int send_tone_telemetry(struct ast_channel *chan, char *tonestring)
{
	char *p,*stringp;
	char *tonesubset;
	int f1,f2;
	int duration;
	int amplitude;
	int res;
	int i;
	int flags;
	
	res = 0;

	if(!tonestring)
		return res;
	
	p = stringp = ast_strdup(tonestring);

	for(;tonestring;){
		tonesubset = strsep(&stringp,")");
		if(!tonesubset)
			break;
		if(sscanf(tonesubset,"(%d,%d,%d,%d", &f1, &f2, &duration, &amplitude) != 4)
			break;
		res = play_tone_pair(chan, f1, f2, duration, amplitude);
		if(res)
			break;
	}
	if(p)
		ast_free(p);
	if(!res)
		res = play_tone_pair(chan, 0, 0, 100, 0); /* This is needed to ensure the last tone segment is timed correctly */
	
	if (!res) 
		res = ast_waitstream(chan, "");

	ast_stopstream(chan);

	/*
	* Wait for the zaptel driver to physically write the tone blocks to the hardware
	*/

	for(i = 0; i < 20 ; i++){
		flags =  DAHDI_IOMUX_WRITEEMPTY | DAHDI_IOMUX_NOWAIT; 
		res = ioctl(chan->fds[0], DAHDI_IOMUX, &flags);
		if(flags & DAHDI_IOMUX_WRITEEMPTY)
			break;
		if( ast_safe_sleep(chan, 50)){
			res = -1;
			break;
		}
	}
		
	return res;
		
}

//# Say a file - streams file to output channel

static int sayfile(struct ast_channel *mychannel,char *fname)
{
int	res;

	res = ast_streamfile(mychannel, fname, mychannel->language);
	if (!res) 
		res = ast_waitstream(mychannel, "");
	else
		 ast_log(LOG_WARNING, "ast_streamfile %s failed on %s\n", fname, mychannel->name);
	ast_stopstream(mychannel);
	return res;
}

static int saycharstr(struct ast_channel *mychannel,char *str)
{
int	res;

	res = ast_say_character_str(mychannel,str,NULL,mychannel->language);
	if (!res) 
		res = ast_waitstream(mychannel, "");
	else
		 ast_log(LOG_WARNING, "ast_streamfile failed on %s\n", mychannel->name);
	ast_stopstream(mychannel);
	return res;
}

//# Say a number -- streams corresponding sound file

static int saynum(struct ast_channel *mychannel, int num)
{
	int res;
	res = ast_say_number(mychannel, num, NULL, mychannel->language, NULL);
	if(!res)
		res = ast_waitstream(mychannel, "");
	else
		ast_log(LOG_WARNING, "ast_streamfile failed on %s\n", mychannel->name);
	ast_stopstream(mychannel);
	return res;
}

//# Say a phonetic words -- streams corresponding sound file

static int sayphoneticstr(struct ast_channel *mychannel,char *str)
{
int	res;

	res = ast_say_phonetic_str(mychannel,str,NULL,mychannel->language);
	if (!res) 
		res = ast_waitstream(mychannel, "");
	else
		 ast_log(LOG_WARNING, "ast_streamfile failed on %s\n", mychannel->name);
	ast_stopstream(mychannel);
	return res;
}

/* say a node and nodename. Try to look in dir referred to by nodenames in
config, and see if there's a custom node file to play, and if so, play it */

static int saynode(struct rpt *myrpt, struct ast_channel *mychannel, char *name)
{
int	res = 0,tgn;
char	*val,fname[300],str[100];

	if (strlen(name) < 1) return(0);
	tgn = tlb_node_get(name,'n',NULL,str,NULL,NULL);
	if (((name[0] != '3') && (tgn != 1)) || ((name[0] == '3') && (myrpt->p.eannmode != 2)) ||
		((tgn == 1) && (myrpt->p.tannmode != 2)))
	{
		val = (char *) ast_variable_retrieve(myrpt->cfg, myrpt->name, "nodenames");
		if (!val) val = NODENAMES;
		snprintf(fname,sizeof(fname) - 1,"%s/%s",val,name);
		if (ast_fileexists(fname,NULL,mychannel->language) > 0)
			return(sayfile(mychannel,fname));
		res = sayfile(mychannel,"rpt/node");
		if (!res) 
			res = ast_say_character_str(mychannel,name,NULL,mychannel->language);
	}
	if (tgn == 1)
	{
		if (myrpt->p.tannmode < 2) return res;
		return(sayphoneticstr(mychannel,str));
	}
	if (name[0] != '3') return res;
	if (myrpt->p.eannmode < 2) return res;
	sprintf(str,"%d",atoi(name + 1));	
	if (elink_db_get(str,'n',NULL,fname,NULL) < 1) return res;
	res = sayphoneticstr(mychannel,fname);
	return res;
}

static int telem_any(struct rpt *myrpt,struct ast_channel *chan, char *entry)
{
	int res;
	char c;
	
	int morsespeed;
	int morsefreq;
	int morseampl;
	int morseidfreq;
	int morseidampl;
	
	res = 0;
	
	morsespeed = retrieve_astcfgint(myrpt, myrpt->p.morse, "speed", 5, 20, 20);
       	morsefreq = retrieve_astcfgint(myrpt, myrpt->p.morse, "frequency", 300, 3000, 800);
       	morseampl = retrieve_astcfgint(myrpt, myrpt->p.morse, "amplitude", 200, 8192, 4096);
	morseidampl = retrieve_astcfgint(myrpt, myrpt->p.morse, "idamplitude", 200, 8192, 2048);
	morseidfreq = retrieve_astcfgint(myrpt, myrpt->p.morse, "idfrequency", 300, 3000, 330);	
	
	/* Is it a file, or a tone sequence? */
			
	if(entry[0] == '|'){
		c = entry[1];
		if((c >= 'a')&&(c <= 'z'))
			c -= 0x20;
	
		switch(c){
			case 'I': /* Morse ID */
				res = send_morse(chan, entry + 2, morsespeed, morseidfreq, morseidampl);
				break;
			
			case 'M': /* Morse Message */
				res = send_morse(chan, entry + 2, morsespeed, morsefreq, morseampl);
				break;
			
			case 'T': /* Tone sequence */
				res = send_tone_telemetry(chan, entry + 2);
				break;
			default:
				res = -1;
		}
	}
	else
		res = sayfile(chan, entry); /* File */
	return res;
}

/*
* This function looks up a telemetry name in the config file, and does a telemetry response as configured.
*
* 4 types of telemtry are handled: Morse ID, Morse Message, Tone Sequence, and a File containing a recording.
*/

static int telem_lookup(struct rpt *myrpt,struct ast_channel *chan, char *node, char *name)
{
	
	int res;
	int i;
	char *entry;

	res = 0;
	entry = NULL;

	entry = (char *) ast_variable_retrieve(myrpt->cfg, myrpt->p.telemetry, name);
	
	/* Try to look up the telemetry name */	

	if(!entry){
		/* Telemetry name wasn't found in the config file, use the default */
		for(i = 0; i < sizeof(tele_defs)/sizeof(struct telem_defaults) ; i++){
			if(!strcasecmp(tele_defs[i].name, name))
				entry = tele_defs[i].value;
		}
	}
	if(entry){	
		if(strlen(entry))
			if (chan) telem_any(myrpt,chan, entry);
	}
	else{
		res = -1;
	}
	return res;
}

/*
* Retrieve a wait interval
*/

static int get_wait_interval(struct rpt *myrpt, int type)
{
        int interval;
        char *wait_times;
        char *wait_times_save;
                                                                                                                  
        wait_times_save = NULL;
        wait_times = (char *) ast_variable_retrieve(myrpt->cfg, myrpt->name, "wait_times");
                                                                                                                  
        if(wait_times){
                wait_times_save = ast_strdup(wait_times);
                if(!wait_times_save)
			return 0;
                
        }
                                                                                                                  
        switch(type){
                case DLY_TELEM:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "telemwait", 500, 5000, 1000);
                        else
                                interval = 1000;
                        break;
                                                                                                                  
                case DLY_ID:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "idwait",250,5000,500);
                        else
                                interval = 500;
                        break;
                                                                                                                  
                case DLY_UNKEY:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "unkeywait",50,5000,1000);
                        else
                                interval = 1000;
                        break;
                                                                                                                  
                case DLY_LINKUNKEY:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "linkunkeywait",500,5000,1000);
                        else
                                interval = 1000;
                        break;
                                                                                                                  
                case DLY_CALLTERM:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "calltermwait",500,5000,1500);
                        else
                                interval = 1500;
                        break;
                                                                                                                  
                case DLY_COMP:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "compwait",500,5000,200);
                        else
                                interval = 200;
                        break;
                                                                                                                  
                case DLY_PARROT:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "parrotwait",500,5000,200);
                        else
                                interval = 200;
                        break;
                case DLY_MDC1200:
                        if(wait_times)
                                interval = retrieve_astcfgint(myrpt,wait_times_save, "mdc1200wait",500,5000,200);
                        else
                                interval = 350;
                        break;
                default:
			interval = 0;
			break;
        }
	if(wait_times_save)
       		ast_free(wait_times_save);
	return interval;
}                                                                                                                  


/*
* Wait a configurable interval of time 
*/
static int wait_interval(struct rpt *myrpt, int type, struct ast_channel *chan)
{
	int interval;

	do {
		while (myrpt->p.holdofftelem && 
			(myrpt->keyed || (myrpt->remrx && (type != DLY_ID))))
		{
			if (ast_safe_sleep(chan,100) < 0) return -1;
		}

		interval = get_wait_interval(myrpt, type);
		if(debug)
			ast_log(LOG_NOTICE,"Delay interval = %d\n", interval);
		if(interval)
			if (ast_safe_sleep(chan,interval) < 0) return -1;
		if(debug)
			ast_log(LOG_NOTICE,"Delay complete\n");
	}
	while (myrpt->p.holdofftelem && 
		(myrpt->keyed || (myrpt->remrx && (type != DLY_ID))));
	return 0;
}

static int split_freq(char *mhz, char *decimals, char *freq);


//### BEGIN TELEMETRY CODE SECTION
/*
 * Routine to process various telemetry commands that are in the myrpt structure
 * Used extensively when links and build/torn down and other events are processed by the 
 * rpt_master threads. 
 */
 
 /*
  *
  * WARNING:  YOU ARE NOW HEADED INTO ONE GIANT MAZE OF SWITCH STATEMENTS THAT DO MOST OF THE WORK FOR
  *           APP_RPT.  THE MAJORITY OF THIS IS VERY UNDOCUMENTED CODE AND CAN BE VERY HARD TO READ. 
  *           IT IS ALSO PROBABLY THE MOST ERROR PRONE PART OF THE CODE, ESPECIALLY THE PORTIONS
  *           RELATED TO THREADED OPERATIONS.
  */
 
static void handle_varcmd_tele(struct rpt *myrpt,struct ast_channel *mychannel,char *varcmd)
{
char	*strs[100],*p,buf[100],c;
int	i,j,k,n,res,vmajor,vminor;
float	f;
time_t	t;
unsigned int t1;
#ifdef	NEW_ASTERISK
struct	ast_tm localtm;
#else
struct	tm localtm;
#endif

	n = finddelim(varcmd,strs,100);
	if (n < 1) return;
	if (!strcasecmp(strs[0],"REMGO"))
	{
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			sayfile(mychannel, "rpt/remote_go");
		return;
	}
	if (!strcasecmp(strs[0],"REMALREADY"))
	{
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			sayfile(mychannel, "rpt/remote_already");
		return;
	}
	if (!strcasecmp(strs[0],"REMNOTFOUND"))
	{
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			sayfile(mychannel, "rpt/remote_notfound");
		return;
	}
	if (!strcasecmp(strs[0],"COMPLETE"))
	{
		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		res = telem_lookup(myrpt,mychannel, myrpt->name, "functcomplete");
		if (!res) 
			res = ast_waitstream(mychannel, "");
		else
			 ast_log(LOG_WARNING, "ast_streamfile failed on %s\n", mychannel->name);
		ast_stopstream(mychannel);
		return;
	}
	if (!strcasecmp(strs[0],"PROC"))
	{
		wait_interval(myrpt, DLY_TELEM, mychannel);
		res = telem_lookup(myrpt, mychannel, myrpt->name, "patchup");
		if(res < 0){ /* Then default message */
			sayfile(mychannel, "rpt/callproceeding");
		}
		return;
	}
	if (!strcasecmp(strs[0],"TERM"))
	{
		/* wait a little bit longer */
		if (wait_interval(myrpt, DLY_CALLTERM, mychannel) == -1) return;
		res = telem_lookup(myrpt, mychannel, myrpt->name, "patchdown");
		if(res < 0){ /* Then default message */
			sayfile(mychannel, "rpt/callterminated");
		}
		return;
	}
	if (!strcasecmp(strs[0],"MACRO_NOTFOUND"))
	{
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			sayfile(mychannel, "rpt/macro_notfound");
		return;
	}
	if (!strcasecmp(strs[0],"MACRO_BUSY"))
	{
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			sayfile(mychannel, "rpt/macro_busy");
		return;
	}
	if (!strcasecmp(strs[0],"CONNECTED"))
	{

		if (n < 3) return;
		if (wait_interval(myrpt, DLY_TELEM,  mychannel) == -1) return;
		res = saynode(myrpt,mychannel,strs[2]);
		if (!res)
		    res = ast_streamfile(mychannel, "rpt/connected", mychannel->language);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		else
			 ast_log(LOG_WARNING, "ast_streamfile failed on %s\n", mychannel->name);
		ast_stopstream(mychannel);
		res = ast_streamfile(mychannel, "digits/2", mychannel->language);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		else
			 ast_log(LOG_WARNING, "ast_streamfile failed on %s\n", mychannel->name);
		ast_stopstream(mychannel);
		saynode(myrpt,mychannel,strs[1]);
		return;
	}
	if (!strcasecmp(strs[0],"CONNFAIL"))
	{

		if (n < 2) return;			
		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		res = saynode(myrpt,mychannel,strs[1]);
		if (!res) 
		   sayfile(mychannel, "rpt/connection_failed");
		return;
	}
	if (!strcasecmp(strs[0],"REMDISC"))
	{

		if (n < 2) return;			
		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		res = saynode(myrpt,mychannel,strs[1]);
		if (!res) 
		   sayfile(mychannel, "rpt/remote_disc");
		return;
	}
	if (!strcasecmp(strs[0],"STATS_TIME"))
	{
		if (n < 2) return;
		if (sscanf(strs[1],"%u",&t1) != 1) return;
		t = t1;
	    	if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		rpt_localtime(&t, &localtm, myrpt->p.timezone);
		t1 = rpt_mktime(&localtm,NULL);
		/* Say the phase of the day is before the time */
		if((localtm.tm_hour >= 0) && (localtm.tm_hour < 12))
			p = "rpt/goodmorning";
		else if((localtm.tm_hour >= 12) && (localtm.tm_hour < 18))
			p = "rpt/goodafternoon";
		else
			p = "rpt/goodevening";
		if (sayfile(mychannel,p) == -1) return;
		/* Say the time is ... */		
		if (sayfile(mychannel,"rpt/thetimeis") == -1) return;
		/* Say the time */				
	    	res = ast_say_time(mychannel, t1, "", mychannel->language);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);		
		return;
	}		
	if (!strcasecmp(strs[0],"STATS_VERSION"))
	{
		if (n < 2) return;
		if(sscanf(strs[1], "%d.%d", &vmajor, &vminor) != 2) return;
    		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		/* Say "version" */
		if (sayfile(mychannel,"rpt/version") == -1) return;
		res = ast_say_number(mychannel, vmajor, "", mychannel->language, (char *) NULL);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);	
		if (saycharstr(mychannel,".") == -1) return;
		if(!res) /* Say "Y" */
			ast_say_number(mychannel, vminor, "", mychannel->language, (char *) NULL);
		if (!res){
			res = ast_waitstream(mychannel, "");
			ast_stopstream(mychannel);
		}	
		else
			 ast_log(LOG_WARNING, "ast_streamfile failed on %s\n", mychannel->name);
		return;
	}		
	if (!strcasecmp(strs[0],"STATS_GPS"))
	{
		if (n < 5) return;
	    	if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		if (saynode(myrpt,mychannel,strs[1]) == -1) return;
		if (sayfile(mychannel,"location") == -1) return;
		c = *(strs[2] + strlen(strs[2]) - 1);
		*(strs[2] + strlen(strs[2]) - 1) = 0;
		if (sscanf(strs[2],"%2d%d.%d",&i,&j,&k) != 3) return;
		res = ast_say_number(mychannel, i, "", mychannel->language, (char *) NULL);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);	
		if (sayfile(mychannel,"degrees") == -1) return;
		res = ast_say_number(mychannel, j, "", mychannel->language, (char *) NULL);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);	
		if (saycharstr(mychannel,strs[2] + 4) == -1) return;
		if (sayfile(mychannel,"minutes") == -1) return;
		if (sayfile(mychannel,(c == 'N') ? "north" : "south") == -1) return;
		if (sayfile(mychannel,"rpt/latitude") == -1) return;
		c = *(strs[3] + strlen(strs[3]) - 1);
		*(strs[3] + strlen(strs[3]) - 1) = 0;
		if (sscanf(strs[3],"%3d%d.%d",&i,&j,&k) != 3) return;
		res = ast_say_number(mychannel, i, "", mychannel->language, (char *) NULL);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);	
		if (sayfile(mychannel,"degrees") == -1) return;
		res = ast_say_number(mychannel, j, "", mychannel->language, (char *) NULL);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);	
		if (saycharstr(mychannel,strs[3] + 5) == -1) return;
		if (sayfile(mychannel,"minutes") == -1) return;
		if (sayfile(mychannel,(c == 'E') ? "east" : "west") == -1) return;
		if (sayfile(mychannel,"rpt/longitude") == -1) return;
		if (!*strs[4]) return;
		c = *(strs[4] + strlen(strs[4]) - 1);
		*(strs[4] + strlen(strs[4]) - 1) = 0;
		if (sscanf(strs[4],"%f",&f) != 1) return;
		if (myrpt->p.gpsfeet)
		{
			if (c == 'M') f *= 3.2808399;
		}
		else
		{
			if (c != 'M') f /= 3.2808399;
		}			
		sprintf(buf,"%0.1f",f);
		if (sscanf(buf,"%d.%d",&i,&j) != 2) return;
		res = ast_say_number(mychannel, i, "", mychannel->language, (char *) NULL);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);	
		if (saycharstr(mychannel,".") == -1) return;
		res = ast_say_number(mychannel, j, "", mychannel->language, (char *) NULL);
		if (!res) 
			res = ast_waitstream(mychannel, "");
		ast_stopstream(mychannel);	
		if (sayfile(mychannel,(myrpt->p.gpsfeet) ? "feet" : "meters") == -1) return;
		if (saycharstr(mychannel,"AMSL") == -1) return;
		ast_stopstream(mychannel);	
		return;
	}
	if (!strcasecmp(strs[0],"ARB_ALPHA"))
	{
		if (n < 2) return;
	    	if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
	    	saycharstr(mychannel, strs[1]);
		return;
	}		
	if (!strcasecmp(strs[0],"REV_PATCH"))
	{
		/* Parts of this section taken from app_parkandannounce */
		char *tpl_working, *tpl_current, *tpl_copy;
		char *tmp[100], *myparm;
		int looptemp=0,i=0, dres = 0;

		if (n < 3) return;
	    	if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
	

		tpl_working = ast_strdup(strs[2]);
		tpl_copy = tpl_working;
		myparm = strsep(&tpl_working,"^");
		tpl_current=strsep(&tpl_working, ":");
		while(tpl_current && looptemp < sizeof(tmp)) {
			tmp[looptemp]=tpl_current;
			looptemp++;
			tpl_current=strsep(&tpl_working,":");
		}
		for(i=0; i<looptemp; i++) {
			if(!strcmp(tmp[i], "PARKED")) {
				ast_say_digits(mychannel, atoi(myparm), "", mychannel->language);
			} else if(!strcmp(tmp[i], "NODE")) {
				ast_say_digits(mychannel, atoi(strs[1]), "", mychannel->language);
			} else {
				dres = ast_streamfile(mychannel, tmp[i], mychannel->language);
				if(!dres) {
					dres = ast_waitstream(mychannel, "");
				} else {
					ast_log(LOG_WARNING, "ast_streamfile of %s failed on %s\n", tmp[i], mychannel->name);
					dres = 0;
				}
			}
		}
		ast_free(tpl_copy);
		return;
	}
	if (!strcasecmp(strs[0],"LASTNODEKEY"))
	{
		if (n < 2) return;
		if (!atoi(strs[1])) return;
		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		saynode(myrpt,mychannel,strs[1]);
		return;
	}
	if (!strcasecmp(strs[0],"LASTUSER"))
	{
		if (n < 2) return;
		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		sayphoneticstr(mychannel,strs[1]);
		if (n < 3) return;
		sayfile(mychannel,"and");
		sayphoneticstr(mychannel,strs[2]);
		return;
	}
	if (!strcasecmp(strs[0],"STATUS"))
	{
		if (n < 3) return;
		if (wait_interval(myrpt, DLY_TELEM, mychannel) == -1) return;
		saynode(myrpt,mychannel,strs[1]);
		if (atoi(strs[2]) > 0) sayfile(mychannel, "rpt/autopatch_on");
		else if (n == 3)
		{
			sayfile(mychannel,"rpt/repeat_only");
			return;
		}
		for(i = 3; i < n; i++)
		{
			saynode(myrpt,mychannel,strs[i] + 1);
			if (*strs[i] == 'T') sayfile(mychannel,"rpt/tranceive");
			else if (*strs[i] == 'R') sayfile(mychannel,"rpt/monitor");
			else if (*strs[i] == 'L') sayfile(mychannel,"rpt/localmonitor");
			else sayfile(mychannel,"rpt/connecting");
		}
		return;
	}
	ast_log(LOG_WARNING,"Got unknown link telemetry command: %s\n",strs[0]);
	return;
}

/*
 *  Threaded telemetry handling routines - goes hand in hand with the previous routine (see above)
 *  This routine does a lot of processing of what you "hear" when app_rpt is running.
 *  Note that this routine could probably benefit from an overhaul to make it easier to read/debug. 
 *  Many of the items here seem to have been bolted onto this routine as it app_rpt has evolved.
 */
 
static void *rpt_tele_thread(void *this)
{
struct dahdi_confinfo ci;  /* conference info */
int	res = 0,haslink,hastx,hasremote,imdone = 0, unkeys_queued, x;
struct	rpt_tele *mytele = (struct rpt_tele *)this;
struct  rpt_tele *tlist;
struct	rpt *myrpt;
struct	rpt_link *l,*l1,linkbase;
struct	ast_channel *mychannel;
int id_malloc, vmajor, vminor, m;
char *p,*ct,*ct_copy,*ident, *nodename,*cp;
time_t t,t1,was;
#ifdef	NEW_ASTERISK
struct ast_tm localtm;
#else
struct tm localtm;
#endif
char lbuf[MAXLINKLIST],*strs[MAXLINKLIST];
int	i,j,k,ns,rbimode;
unsigned int u;
char mhz[MAXREMSTR],decimals[MAXREMSTR],mystr[200];
char	lat[100],lon[100],elev[100],c;
FILE	*fp;
float	f;
struct stat mystat;
struct dahdi_params par;
#ifdef	_MDC_ENCODE_H_
struct	mdcparams *mdcp;
#endif

	/* get a pointer to myrpt */
	myrpt = mytele->rpt;

	/* Snag copies of a few key myrpt variables */
	rpt_mutex_lock(&myrpt->lock);
	nodename = ast_strdup(myrpt->name);
	if(!nodename)
	{
	    fprintf(stderr,"rpt:Sorry unable strdup nodename\n");
	    rpt_mutex_lock(&myrpt->lock);
	    remque((struct qelem *)mytele);
	    ast_log(LOG_NOTICE,"Telemetry thread aborted at line %d, mode: %d\n",__LINE__, mytele->mode); /*@@@@@@@@@@@*/
	    rpt_mutex_unlock(&myrpt->lock);
	    ast_free(mytele);
	    pthread_exit(NULL);
	}

	if (myrpt->p.ident){
		ident = ast_strdup(myrpt->p.ident);
        	if(!ident)
		{
        	        fprintf(stderr,"rpt:Sorry unable strdup ident\n");
			rpt_mutex_lock(&myrpt->lock);
                	remque((struct qelem *)mytele);
                	ast_log(LOG_NOTICE,"Telemetry thread aborted at line %d, mode: %d\n",
			__LINE__, mytele->mode); /*@@@@@@@@@@@*/
                	rpt_mutex_unlock(&myrpt->lock);
			ast_free(nodename);
                	ast_free(mytele);
                	pthread_exit(NULL);
        	}
		else{
			id_malloc = 1;
		}
	}
	else
	{
		ident = "";
		id_malloc = 0;
	}
	rpt_mutex_unlock(&myrpt->lock);
		


	/* allocate a pseudo-channel thru asterisk */
	mychannel = ast_request(DAHDI_CHANNEL_NAME,AST_FORMAT_SLINEAR,"pseudo",NULL);
	if (!mychannel)
	{
		fprintf(stderr,"rpt:Sorry unable to obtain pseudo channel\n");
		rpt_mutex_lock(&myrpt->lock);
		remque((struct qelem *)mytele);
		ast_log(LOG_NOTICE,"Telemetry thread aborted at line %d, mode: %d\n",__LINE__, mytele->mode); /*@@@@@@@@@@@*/
		rpt_mutex_unlock(&myrpt->lock);
		ast_free(nodename);
		if(id_malloc)
			ast_free(ident);
		ast_free(mytele);		
		pthread_exit(NULL);
	}
#ifdef	AST_CDR_FLAG_POST_DISABLED
	if (mychannel->cdr) 
		ast_set_flag(mychannel->cdr,AST_CDR_FLAG_POST_DISABLED);
#endif
	ast_answer(mychannel);
	rpt_mutex_lock(&myrpt->lock);
	mytele->chan = mychannel;
	while (myrpt->active_telem && 
	    ((myrpt->active_telem->mode == PAGE) || (
		myrpt->active_telem->mode == MDC1200)))
	{
                rpt_mutex_unlock(&myrpt->lock);
		usleep(100000);
		rpt_mutex_lock(&myrpt->lock);
	}
	rpt_mutex_unlock(&myrpt->lock);
	while((mytele->mode != SETREMOTE) && (mytele->mode != UNKEY) &&
	    (mytele->mode != LINKUNKEY) && (mytele->mode != LOCUNKEY) &&
		(mytele->mode != COMPLETE) && (mytele->mode != REMGO) && 
		    (mytele->mode != REMCOMPLETE))
	{	
                rpt_mutex_lock(&myrpt->lock);
		if ((!myrpt->active_telem) &&
			(myrpt->tele.prev == mytele))
		{
			myrpt->active_telem = mytele;
	                rpt_mutex_unlock(&myrpt->lock);
			break;
		}
                rpt_mutex_unlock(&myrpt->lock);
		usleep(100000);
	}

	/* make a conference for the tx */
	ci.chan = 0;
	/* If the telemetry is only intended for a local audience, */
	/* only connect the ID audio to the local tx conference so */
	/* linked systems can't hear it */
	ci.confno = (((mytele->mode == ID1) || (mytele->mode == PLAYBACK) ||
	    (mytele->mode == TEST_TONE) || (mytele->mode == STATS_GPS_LEGACY)) ? 
		myrpt->conf : myrpt->teleconf);
		myrpt->conf : myrpt->txconf);
	ci.confmode = DAHDI_CONF_CONFANN;
	/* first put the channel on the conference in announce mode */
	if (ioctl(mychannel->fds[0],DAHDI_SETCONF,&ci) == -1)
	{
		ast_log(LOG_WARNING, "Unable to set conference mode to Announce\n");
		rpt_mutex_lock(&myrpt->lock);
		myrpt->active_telem = NULL;
		remque((struct qelem *)mytele);
		rpt_mutex_unlock(&myrpt->lock);
		ast_log(LOG_NOTICE,"Telemetry thread aborted at line %d, mode: %d\n",__LINE__, mytele->mode); /*@@@@@@@@@@@*/
		ast_free(nodename);
		if(id_malloc)
			ast_free(ident);
		ast_free(mytele);		
		ast_hangup(mychannel);
		pthread_exit(NULL);
	}
	ast_stopstream(mychannel);
	res = 0;
	switch(mytele->mode)
	{
	    case USEROUT:
		handle_userout_tele(myrpt, mychannel, mytele->param);
		imdone = 1;
		break;

	    case METER:
		handle_meter_tele(myrpt, mychannel, mytele->param);
		imdone = 1;
		break;

	    case VARCMD:
		handle_varcmd_tele(myrpt,mychannel,mytele->param);
		imdone = 1;
		break;
	    case ID:
	    case ID1:
		if (*ident)
		{
			/* wait a bit */
			if (!wait_interval(myrpt, (mytele->mode == ID) ? DLY_ID : DLY_TELEM,mychannel))
				res = telem_any(myrpt,mychannel, ident); 
		}
		imdone=1;	
		break;
		
	    case TAILMSG:
		/* wait a little bit longer */
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			res = ast_streamfile(mychannel, myrpt->p.tailmessages[myrpt->tailmessagen], mychannel->language); 
		break;

	    case IDTALKOVER:
		if(debug >= 6)
			ast_log(LOG_NOTICE,"Tracepoint IDTALKOVER: in rpt_tele_thread()\n");
	    	p = (char *) ast_variable_retrieve(myrpt->cfg, nodename, "idtalkover");
	    	if(p)
			res = telem_any(myrpt,mychannel, p); 
		imdone=1;	
	    	break;
	    		
	    case PROC:
		/* wait a little bit longer */
		if (wait_interval(myrpt, DLY_TELEM, mychannel))
			res = telem_lookup(myrpt, mychannel, myrpt->name, "patchup");
		if(res < 0){ /* Then default message */
			res = ast_streamfile(mychannel, "rpt/callproceeding", mychannel->language);
		}
		break;
	    case TERM:
		/* wait a little bit longer */
		if (!wait_interval(myrpt, DLY_CALLTERM, mychannel))
			res = telem_lookup(myrpt, mychannel, myrpt->name, "patchdown");
		if(res < 0){ /* Then default message */
			res = ast_streamfile(mychannel, "rpt/callterminated", mychannel->language);
		}
		break;
	    case COMPLETE:
		/* wait a little bit */
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			res = telem_lookup(myrpt,mychannel, myrpt->name, "functcomplete");
		break;
	    case REMCOMPLETE:
		/* wait a little bit */
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			res = telem_lookup(myrpt,mychannel, myrpt->name, "remcomplete");
		break;
	    case MACRO_NOTFOUND:
		/* wait a little bit */
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			res = ast_streamfile(mychannel, "rpt/macro_notfound", mychannel->language);
		break;
	    case MACRO_BUSY:
		/* wait a little bit */
		if (!wait_interval(myrpt, DLY_TELEM, mychannel))
			res = ast_streamfile(mychannel, "rpt/macro_busy", mychannel->language);
		break;
	    case PAGE:
		if (!wait_interval(myrpt, DLY_TELEM,  mychannel))
		{
			res = -1;
			if (mytele->submode.p)
			{
			myrpt->noduck=1;
				res = ast_playtones_start(myrpt->txchannel,0,
					(char *) mytele->submode.p,0);
				while(myrpt->txchannel->generatordata)
				{
					if(ast_safe_sleep(myrpt->txchannel, 50))
					{
						res = -1;
						break;
					}
				}
				free((char *)mytele->submode.p);
			}
		}
		imdone = 1;
		break;
#ifdef	_MDC_ENCODE_H_
	    case MDC1200:
		mdcp = (struct mdcparams *)mytele->submode.p;
		if (mdcp)
		{
			if (mdcp->type[0] != 'A')
			{
				if (wait_interval(myrpt, DLY_TELEM,  mychannel) == -1)
				{
					res = -1;
					imdone = 1;
					break;
				}
			}
			else
			{
				if (wait_interval(myrpt, DLY_MDC1200,  mychannel) == -1)
				{
					res = -1;
					imdone = 1;
					break;
				}
			}
			res = mdc1200gen_start(myrpt->txchannel,mdcp->type,mdcp->UnitID,mdcp->DestID,mdcp->subcode);
			ast_free(mdcp);
			while(myrpt->txchannel->generatordata)
			{
				if(ast_safe_sleep(myrpt->txchannel, 50))
				{
					res = -1;
					break;
				}
			}
		}
		imdone = 1;
		break;
#endif
	    case UNKEY:
	    case LOCUNKEY:
		if(myrpt->patchnoct && myrpt->callmode){ /* If no CT during patch configured, then don't send one */
			imdone = 1;
			break;
		}
			
		/*
		* Reset the Unkey to CT timer
		*/

		x = get_wait_interval(myrpt, DLY_UNKEY);
		rpt_mutex_lock(&myrpt->lock);
		myrpt->unkeytocttimer = x; /* Must be protected as it is changed below */
		rpt_mutex_unlock(&myrpt->lock);

		/*
		* If there's one already queued, don't do another
		*/

		tlist = myrpt->tele.next;
		unkeys_queued = 0;
                if (tlist != &myrpt->tele)
                {
                        rpt_mutex_lock(&myrpt->lock);
                        while(tlist != &myrpt->tele){
                                if ((tlist->mode == UNKEY) || 
				    (tlist->mode == LOCUNKEY)) unkeys_queued++;
                                tlist = tlist->next;
                        }
                        rpt_mutex_unlock(&myrpt->lock);
		}
		if( unkeys_queued > 1){
			imdone = 1;
			break;
		}

		/* Wait for the telemetry timer to expire */
		/* Periodically check the timer since it can be re-initialized above */
		while(myrpt->unkeytocttimer)
		{
			int ctint;
			if(myrpt->unkeytocttimer > 100)
				ctint = 100;
			else
				ctint = myrpt->unkeytocttimer;
			ast_safe_sleep(mychannel, ctint);
			rpt_mutex_lock(&myrpt->lock);
			if(myrpt->unkeytocttimer < ctint)
				myrpt->unkeytocttimer = 0;
			else
				myrpt->unkeytocttimer -= ctint;
			rpt_mutex_unlock(&myrpt->lock);
		}
	
		/*
		* Now, the carrier on the rptr rx should be gone. 
		* If it re-appeared, then forget about sending the CT
		*/
		if(myrpt->keyed){
			imdone = 1;
			break;
		}
		
		rpt_mutex_lock(&myrpt->lock); /* Update the kerchunk counters */
		myrpt->dailykerchunks++;
		myrpt->totalkerchunks++;
		rpt_mutex_unlock(&myrpt->lock);
	
treataslocal:

		rpt_mutex_lock(&myrpt->lock);
		/* get all the nodes */
		__mklinklist(myrpt,NULL,lbuf,0);
		rpt_mutex_unlock(&myrpt->lock);
		/* parse em */
		ns = finddelim(lbuf,strs,MAXLINKLIST);
		haslink = 0;
		for(i = 0; i < ns; i++)
		{
			char *cpr = strs[i] + 1;
			if (!strcmp(cpr,myrpt->name)) continue;
			if (ISRANGER(cpr)) haslink = 1;
		}

		/* if has a RANGER node connected to it, use special telemetry for RANGER mode */
		if (haslink)
		{
			res = telem_lookup(myrpt,mychannel, myrpt->name, "ranger");
			if(res)
				ast_log(LOG_WARNING, "telem_lookup:ranger failed on %s\n", mychannel->name);
		}

		if ((mytele->mode == LOCUNKEY) &&
		    ((ct = (char *) ast_variable_retrieve(myrpt->cfg, nodename, "localct")))) { /* Local override ct */
			ct_copy = ast_strdup(ct);
			if(ct_copy)
			{
			    myrpt->noduck=1;
				res = telem_lookup(myrpt,mychannel, myrpt->name, ct_copy);
				ast_free(ct_copy);
			}
			else
				res = -1;
			if(res)
			 	ast_log(LOG_WARNING, "telem_lookup:ctx failed on %s\n", mychannel->name);		
		}
		haslink = 0;
		hastx = 0;
		hasremote = 0;		
		l = myrpt->links.next;
		if (l != &myrpt->links)
		{
			rpt_mutex_lock(&myrpt->lock);
			while(l != &myrpt->links)
			{
				int v,w;

				if (l->name[0] == '0')
				{
					l = l->next;
					continue;
				}
				w = 1;
				if (myrpt->p.nolocallinkct)
				{

					for(v = 0; v < nrpts; v++)
					{
						if (&rpt_vars[v] == myrpt) continue;
						if (rpt_vars[v].remote) continue;
						if (strcmp(rpt_vars[v].name,l->name)) continue;
						w = 0;
						break;
					}
				} 
				if (myrpt->p.locallinknodesn)
				{
					for(v = 0; v < myrpt->p.locallinknodesn; v++)
					{
						if (strcmp(l->name,myrpt->p.locallinknodes[v])) continue;
						w = 0;
						break;
					}
				}
				if (w) haslink = 1;
				if (l->mode == 1) {
					hastx++;
					if (l->isremote) hasremote++;
				}
				l = l->next;
			}
			rpt_mutex_unlock(&myrpt->lock);
		}
		if (haslink)
		{
			myrpt->noduck=1;

			res = telem_lookup(myrpt,mychannel, myrpt->name, (!hastx) ? "remotemon" : "remotetx");
			if(res)
				ast_log(LOG_WARNING, "telem_lookup:remotexx failed on %s\n", mychannel->name);
			
		
			/* if in remote cmd mode, indicate it */
			if (myrpt->cmdnode[0] && strcmp(myrpt->cmdnode,"aprstt"))
			{
				ast_safe_sleep(mychannel,200);
				res = telem_lookup(myrpt,mychannel, myrpt->name, "cmdmode");
				if(res)
				 	ast_log(LOG_WARNING, "telem_lookup:cmdmode failed on %s\n", mychannel->name);
				ast_stopstream(mychannel);
			}
		}
		else if((ct = (char *) ast_variable_retrieve(myrpt->cfg, nodename, "unlinkedct"))){ /* Unlinked Courtesy Tone */
			ct_copy = ast_strdup(ct);
			if(ct_copy)
			{
