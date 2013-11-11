// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Definitions of commdb tables and associated columns.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef GLOBALDEF_H
#define GLOBALDEF_H

#include <e32base.h>
#include <e32std.h>
#include <D32Comm.h>
#include <d32dbms.h>
#include <nifvar.h>
#include <etelMm.h>

#include "t_tabdef.h"
#include "Metadatabase.h"
//#include "commsdat.h"
#include "CommsDatTypesV1_1.h"
//#include "cedtablebase.h"
//#include <centralrepository.h>

using namespace CommsDat;

// Repository for comms data
const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };

#define MAX_COL_NAME_LEN		64



/*Some definitions related to the Config File*/

/**
Macro for build independent literal ADD_SECTION
*/
#define BEGIN_ADD			_L("ADD_SECTION")

/**
Macro for build independent literal ADD_SECTION
*/
#define BEGIN_TEMPLATE		_L("ADD_TEMPLATE")

/**
Macro for build independent literal ADD_ATTRIBUTE
*/
#define BEGIN_ATTRIB		_L("ADD_ATTRIBUTES")

/**
Macro for build independent literal ADD_SECTION
*/
#define BEGIN_SET			_L("SET_SECTION")

/**
Macro for build independent literal END_ADD
*/
#define END_ADD				_L("END_ADD")

/**
Macro for build independent literal END_TEMPLATE
*/
#define END_TEMPLATE		_L("END_TEMPLATE")

/**
Macro for build independent literal END_SET
*/
#define END_SET				_L("END_SET")

/**
Macro for build independent literal END_ATTRIB
*/
#define END_ATTRIB			_L("END_ATTRIBUTES")

/**
Macro for build independent literal SET_CONDITION
*/
#define CONDITION			_L("SET_CONDITION")

/**
Macro for build independent literal _TABLE
*/
#define TABLE			_L("_TABLE{")

/**
Macro for build independent literal _TABLE
*/
#define TEMPLATE		_L("_TEMPLATE{")

/**
Macro for build independent literal _TABLE
*/
#define RECORD			_L("_RECORD{")

/**
Macro for build independent literal FIELD_COUNT
*/
#define FIELD_COUNT			_L("FIELD_COUNT")

#define E_UNKNOWN			0

#define E_ADD				1

#define E_SET				2

#define E_TEMPLATE			3

#define E_ATTRIB			4

#define E_TABLE				5

#define E_TPLATE			6

#define E_RECORD			7

#define E_DATA				8

/**/




/**
access to local error strings
@internalComponent
*/
extern TPtrC errorCode[];

/**
associated default working files
*/
#define DATABASE_TARGET			_S("c:\\system\\data\\cdbv3.dat")	// physical database

/**
end of array
*/
#define NO_MORE_RECORDS			_S("NO_MORE_RECORDS")

/**
all records in table
*/
#define ALL_RECORDS				_S("ALL_RECORDS")

/**
TRUE enum
*/
#define TRUE_VAL				_S("TRUE")

/**
FALSE enum
*/
#define FALSE_VAL				_S("FALSE")

/**
empty enum
*/
#define NOTHING					_S("")

/**
max amount of column updates per table we can handle
*/
#define MAX_DB_COLUMNS			100

/**
max length for column name
*/
#define MAX_COL_NAME_LEN		64

/**
max length of column contents
*/
#define MAX_COL_VAL_LEN			96

/**
max length of long text column contents
*/
#define MAX_COL_LONG_VAL_LEN	1024

/**
max length of queries
*/
#define MAX_QUERY_LEN			256

/**
initial value for bad text that must be replaced
*/
#define BADTEXTVAL				_S("NOTHING")

/**
initial value for bad number that must be replaced
*/
#define BADNUMVAL				(unsigned long)999999

/**
local CED defines, for values not already defined by CommDB
*/

/**
list of local error codes
@internalComponent
*/
#define E_NOTENOUGHDATA		0

#define E_INVALIDCOMMAND	1
#define E_INCORRECTTYPE		2
#define E_INCORRECTCOLNAME	3
#define E_ALREADYEXISTS		4
#define E_INCORRECTENUM		5
#define E_INCORRECTAGENT	6
#define E_BADFIELDCOUNT		7
#define E_NOFIELDS			8
#define E_NOTSUPPORTED		9
#define E_INCORRECTGLOBAL	10
#define E_BADESCAPEDCHAR	11
#define E_BADHEXADECIMAL	12

/**
These names are maintained for compatibility they are deprecated
and should be replaced in code by CDB_LIT declarations
@internalComponent
*/
#define SERVER_NAME					(const TText*)_SERVER_NAME
#define GLOBAL_SETTINGS				(const TText*)_GLOBAL_SETTINGS
#define CONNECTION_PREFERENCES		(const TText*)_CONNECTION_PREFERENCES
//#define AGENT						(const TText*)_AGENT

/**
Misc table names that don't need to be public
@internalComponent
*/
#define _GLOBAL_SETTINGS			_I("GlobalSettings")
#define _CONNECTION_PREFERENCES		_I("ConnectionPreferences")
#define _ACCESS_TABLE				_I("AccessTypeTable")


/**
Columns for Access Type table
@internalComponent
*/
#define ACCESS_TYPE_TABLE		_S("AccessTypeTable")
#define TABLE_ACC_NAME			_S("TableName")
#define TABLE_ACC_TYPE			_S("AccessType")

//
// Columns for Connection Preferences table
//
/** Ranking of this connection preference.
This information is used to specify the order in which attempts at a connection should be
carried out. The highest ranking is 1 and is always attempted. If this fails, a preference
with rank 2 is then attempted, and so on. Ranking of 0 indicates that a connection
preference is only being stored and is not used for connections.

For incoming connections, CommDb limits the ranking to 0 or 1. For outgoing connection
preferences, the maximum ranking is the value in CONNECTION_ATTEMPTS field in the Global
table.

Multiple records with the same ranking (except for 0) and the same direction are not allowed.

Field type: TUint32. This field may not be left as NULL.
@internalComponent
*/
#define _CONNECT_PREF_RANKING		_I("Ranking")

/**
The direction of the connection. The directions are defined by the enum 
TCommDbConnectionDirection.
Field type: TUint32. This field may not be left as NULL.
*/
#define _CONNECT_PREF_DIRECTION		_I("Direction")
/**
The bearers that can be used for this connection. 

The bearers are defined by the enum TCommDbBearer. Values can be OR'ed together to indicate 
that either of the bearers could be used.
Field type: TUint32. This field may not be left as NULL.
*/
#define _CONNECT_PREF_BEARER_SET	_I("BearerSet")

/**
Determines whether or not the user should be prompted or warned with a dialog 
that a connection is going to be made.

The options are defined by the enum TCommDbDialogPref.

Field type: TUint32. This field may not be left as NULL.

@see TCommDbDialogPref.
*/
#define _CONNECT_PREF_DIALOG_PREF	_I("DialogPref")

/**
Identifier of a preferred IAP for this connection, if this is an IAP type database.

For ISP type databases, see the subsequent fields..

Field type: TUint32. This field may be left as NULL.
*/
#define _CONNECT_PREF_IAP			_I("IAP")

/**
Ranking of this connection preference.

This information is used to specify the order in which attempts at a connection 
should be carried out. The highest ranking is 1 and is always attempted. If 
this fails, a preference with rank 2 is then attempted, and so on. A ranking 
of 0 indicates that a connection preference is only being stored and is not 
used for connections.

For incoming connections, CommDb limits the ranking to 0 or 1. For outgoing 
connection preferences, the maximum ranking is the value in CONNECTION_ATTEMPTS 
field in the Global table.

Multiple records with the same ranking (except for 0) and the same direction 
are not allowed.

Type: TUint32. Null: no

@see CONNECTION_ATTEMPTS 
*/
#define CONNECT_PREF_RANKING		(const TText*)_CONNECT_PREF_RANKING

/** 
The direction of the connection. The directions are defined by the enum 
TCommDbConnectionDirection.

Type: TUint32. Null: no 

@see TCommDbConnectionDirection
*/
#define CONNECT_PREF_DIRECTION		(const TText*)_CONNECT_PREF_DIRECTION

/** 
The bearers that can be used for this connection.

The bearers are defined by the enum TCommDbBearer. Values can be ORed together 
to indicate that either of the bearers could be used.

Type: TUint32. Null: no

@see TCommDbBearer 
*/
#define CONNECT_PREF_BEARER_SET		(const TText*)_CONNECT_PREF_BEARER_SET

/** 
Determines whether or not the user should be should be prompted or warned with 
a dialog that a connection is going to be made. 

The options are defined by the enum TCommDbDialogPref.

Type: TUint32. Null: no

@see TCommDbDialogPref 
*/
#define CONNECT_PREF_DIALOG_PREF	(const TText*)_CONNECT_PREF_DIALOG_PREF

/** 
Identifier of the preferred IAP for this connection, if this is an IAP type database.

For ISP type databases, see the subsequent fields.

Type: TUint32. Null: yes 
*/
#define CONNECT_PREF_IAP			(const TText*)_CONNECT_PREF_IAP

/**
types of commdb table access we can have.  This is because CommDB does not have a uniform structure to it's
tables and some tables must be accessed using specialist functions(!) so we must determine this during
the processing

@internalComponent
*/
typedef enum accessType
	{
	DBUnknown = 0,
	DBPrefIn = 1,
	DBPrefOut = 2,
	DBTable = 4,
	DBInsert = 8,
	DBUpdate = 16,
	DBAgent = 32,
	DBGlobal = 64,
	DBTemplate = 128
#ifdef __COMMDB_ROHID_SUPPORT__
	,DBExtendedProcessing = 256
#endif
	} ACCESSTYPE;


/**
holds information on a particular column of a table
@internalComponent
*/
typedef struct ColAttributes
	{
	TPtrC						Name;
	TBuf<MAX_COL_LONG_VAL_LEN>	Value;
	TDbColType					Type;
	} COLATTRIBUTES;


//
// ENUMERATION ARRAYS
//
#define PROTECTEDWRITE	_S("ProtectedWrite")
#define PRIVATE			_S("Private")
#define HIDDEN			_S("Hidden")
#define PROTECTEDREAD	_S("ProtectedRead")

/**
list of values in Attributes column
@internalComponent
*/
static const TText* const ATTRIBArray[] =
	{
	HIDDEN,
	PRIVATE,
	PROTECTEDWRITE,
	PROTECTEDREAD,
	NO_MORE_RECORDS
	};

//
// COLUMN ARRAYS
// These arrays define the column names in a particular table - not in any
// particular order, but only the ones we are allowed to modify
//

/**
list of columns in IAP table
@internalComponent
*/
const TText* const IAPcolumnArray[] =
	{
	COMMDB_NAME,
	IAP_SERVICE, IAP_SERVICE_TYPE, IAP_BEARER, IAP_BEARER_TYPE,
	IAP_NETWORK, IAP_NETWORK_WEIGHTING, IAP_LOCATION,
#ifdef SYMBIAN_NETWORKING_UMTSR5
	IAP_APPSID,
#endif
	NO_MORE_RECORDS
	};

/**
list of columns in ISP OUT table
@internalComponent
*/
const TText* const ISPOUTcolumnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif
	COMMDB_NAME,	
	ISP_DESCRIPTION, ISP_TYPE, ISP_DEFAULT_TEL_NUM, ISP_DIAL_RESOLUTION,
	ISP_USE_LOGIN_SCRIPT, ISP_LOGIN_SCRIPT, ISP_PROMPT_FOR_LOGIN,
	ISP_LOGIN_NAME, ISP_LOGIN_PASS, ISP_CHARGECARD, ISP_DISPLAY_PCT,
	ISP_IF_PARAMS, ISP_IF_NETWORKS, ISP_IF_PROMPT_FOR_AUTH, ISP_IF_AUTH_NAME,
	ISP_IF_AUTH_PASS, ISP_IF_AUTH_RETRIES, ISP_IF_CALLBACK_ENABLED,
	ISP_IF_CALLBACK_TYPE, ISP_IF_CALLBACK_INFO, ISP_CALLBACK_TIMEOUT,
	ISP_IP_ADDR_FROM_SERVER, ISP_IP_ADDR, ISP_IP_NETMASK, ISP_IP_GATEWAY,
	ISP_IP_DNS_ADDR_FROM_SERVER, ISP_IP_NAME_SERVER1, ISP_IP_NAME_SERVER2,
	ISP_IP6_DNS_ADDR_FROM_SERVER, ISP_IP6_NAME_SERVER1, ISP_IP6_NAME_SERVER2,
	ISP_IP_ADDR_LEASE_VALID_FROM, ISP_IP_ADDR_LEASE_VALID_TO,
	ISP_CONFIG_DAEMON_MANAGER_NAME, ISP_CONFIG_DAEMON_NAME, ISP_ENABLE_IP_HEADER_COMP, 
	ISP_ENABLE_LCP_EXTENSIONS, ISP_DISABLE_PLAIN_TEXT_AUTH,
	ISP_ENABLE_SW_COMP, ISP_BEARER_NAME, ISP_BEARER_SPEED, ISP_BEARER_CE,
	ISP_INIT_STRING, ISP_BEARER_TYPE, ISP_CHANNEL_CODING, ISP_AIUR,
	ISP_REQUESTED_TIME_SLOTS, ISP_MAXIMUM_TIME_SLOTS,
	//new column
	ISP_BEARER_SERVICE, ISP_BEARER_PROTOCOL, ISP_RLP_VERSION, ISP_IWF_TO_MS, ISP_MS_TO_IWF,
	ISP_ACK_TIMER, ISP_RETRANSMISSION_ATTEMPTS, ISP_RESEQUENCE_PERIOD, ISP_V42_COMPRESSION,
	ISP_V42_CODEWORDS, ISP_V42_MAX_LENGTH, ISP_ASYMMETRY, ISP_USER_INIT_UPGRADE, ISP_USE_EDGE,
	ISP_IF_SERVER_MODE,
	ISP_SERVICE_ENABLE_LLMNR,
	NO_MORE_RECORDS
	};

/**
list of columns in ISP IN table
@internalComponent
*/
const TText* const ISPINcolumnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif
	COMMDB_NAME,	
	ISP_DESCRIPTION, ISP_TYPE,
	ISP_USE_LOGIN_SCRIPT, ISP_LOGIN_SCRIPT, ISP_AUTHENTICATION,
	ISP_IF_AUTH_RETRIES, ISP_IF_PARAMS, ISP_IF_NETWORKS,
	ISP_IP_ADDR_FROM_SERVER, ISP_IP_ADDR, ISP_IP_NETMASK, ISP_IP_GATEWAY,
	ISP_IP_DNS_ADDR_FROM_SERVER, ISP_IP_NAME_SERVER1, ISP_IP_NAME_SERVER2,
	ISP_IP6_DNS_ADDR_FROM_SERVER, ISP_IP6_NAME_SERVER1, ISP_IP6_NAME_SERVER2,
	ISP_ENABLE_IP_HEADER_COMP, ISP_ENABLE_LCP_EXTENSIONS, ISP_DISABLE_PLAIN_TEXT_AUTH,
	ISP_ENABLE_SW_COMP, ISP_BEARER_NAME, ISP_BEARER_SPEED, ISP_BEARER_CE,
	ISP_INIT_STRING, ISP_USE_EDGE,
	NO_MORE_RECORDS
	};

/**
list of columns in GPRS IN and OUT table
@internalComponent
*/
const TText* const GPRScolumnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif
	COMMDB_NAME,
	GPRS_APN, GPRS_PDP_TYPE, GPRS_REQ_PRECEDENCE,
	GPRS_REQ_DELAY, GPRS_REQ_RELIABILITY, GPRS_REQ_PEAK_THROUGHPUT,
	GPRS_REQ_MEAN_THROUGHPUT, GPRS_MIN_PRECEDENCE, GPRS_MIN_DELAY,
	GPRS_MIN_RELIABILITY, GPRS_MIN_PEAK_THROUGHPUT, GPRS_MIN_MEAN_THROUGHPUT,
	GPRS_DATA_COMPRESSION, GPRS_HEADER_COMPRESSION, GPRS_USE_EDGE, GPRS_ANONYMOUS_ACCESS,
	GPRS_IF_PARAMS, GPRS_IF_NETWORKS, GPRS_IF_PROMPT_FOR_AUTH,
	GPRS_IF_AUTH_NAME, GPRS_IF_AUTH_PASS, GPRS_IF_AUTH_RETRIES,
	GPRS_IP_NETMASK, GPRS_IP_GATEWAY, GPRS_IP_DNS_ADDR_FROM_SERVER,
	GPRS_IP_NAME_SERVER1, GPRS_IP_NAME_SERVER2, GPRS_ENABLE_LCP_EXTENSIONS,
	GPRS_DISABLE_PLAIN_TEXT_AUTH, GPRS_PDP_ADDRESS,
	GPRS_IP_ADDR, GPRS_IP_ADDR_FROM_SERVER,
	GPRS_IP6_DNS_ADDR_FROM_SERVER, GPRS_IP6_NAME_SERVER1, GPRS_IP6_NAME_SERVER2,
	GPRS_IP_ADDR_LEASE_VALID_FROM, GPRS_IP_ADDR_LEASE_VALID_TO, GPRS_CONFIG_DAEMON_MANAGER_NAME, GPRS_CONFIG_DAEMON_NAME,
	//new column
	GPRS_AP_TYPE, GPRS_QOS_WARNING_TIMEOUT,
	GPRS_SERVICE_ENABLE_LLMNR,
	GPRS_QOS_REQ_TRAFFIC_CLASS, GPRS_QOS_MIN_TRAFFIC_CLASS, GPRS_QOS_REQ_DELIVERY_ORDER	,
	GPRS_QOS_MIN_DELIVERY_ORDER, GPRS_QOS_REQ_DELIVER_ERRONEOUS_SDU,GPRS_QOS_MIN_DELIVER_ERRONEOUS_SDU,	
	GPRS_QOS_REQ_MAX_SDUSIZE, GPRS_QOS_MIN_ACCEPTABLE_MAX_SDU_SIZE, GPRS_QOS_REQ_MAX_UPLINK_RATE,
	GPRS_QOS_REQ_MIN_UPLINK_RATE, GPRS_QOS_REQ_MAX_DOWNLINK_RATE,GPRS_QOS_REQ_MIN_DOWNLINK_RATE,		
	GPRS_QOS_REQ_BER, GPRS_QOS_MAX_BER, GPRS_QOS_REQ_SDU_ERROR_RATIO, GPRS_QOS_MAX_SDU_ERROR_RATIO,		
	GPRS_QOS_REQ_TRAFFIC_HANDLING_PRIORITY, GPRS_QOS_MIN_TRAFFIC_HANDLING_PRIORITY,
	GPRS_QOS_REQ_TRANSFER_DELAY, GPRS_QOS_MAX_TRANSFER_DELAY, GPRS_QOS_REQ_GUARANTEED_UPLINK_RATE,	
	GPRS_QOS_MIN_GUARANTEED_UPLINK_RATE, GPRS_QOS_REQ_GUARANTEED_DOWNLINK_RATE,
	GPRS_QOS_MIN_GUARANTEED_DOWNLINK_RATE, GPRS_QOS_SIGNALLING_INDICATION, GPRS_QOS_IM_CN_SIGNALLING_INDICATOR,
	GPRS_QOS_SOURCE_STATISTICS_DESCRIPTOR,
	NO_MORE_RECORDS
	};

/**
list of columns in Default GPRS table
@internalComponent
*/
const TText* const DEFGPRScolumnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif
	COMMDB_NAME,
	DEFAULT_GPRS_USAGE, DEFAULT_GPRS_APN, DEFAULT_GPRS_PDP_TYPE, DEFAULT_GPRS_PDP_ADDRESS,
	DEFAULT_GPRS_PRECEDENCE, DEFAULT_GPRS_DELAY, DEFAULT_GPRS_RELIABILITY,
	DEFAULT_GPRS_PEAK_THROUGHPUT, DEFAULT_GPRS_MEAN_THROUGHPUT, DEFAULT_GPRS_MIN_PRECEDENCE,
	DEFAULT_GPRS_MIN_DELAY, DEFAULT_GPRS_MIN_RELIABILITY, DEFAULT_GPRS_MIN_PEAK_THROUGHPUT,
	DEFAULT_GPRS_MIN_MEAN_THROUGHPUT, DEFAULT_GPRS_DATA_COMPRESSION,
	DEFAULT_GPRS_HEADER_COMPRESSION, DEFAULT_GPRS_USE_EDGE, DEFAULT_GPRS_ANONYMOUS_ACCESS,
	NO_MORE_RECORDS
	};

/**
list of columns in Modem table
@internalComponent
*/
const TText* const MODEMBEARERcolumnArray[] =
	{	
	COMMDB_NAME,
	MODEM_AGENT, MODEM_NIF_NAME, MODEM_PORT_NAME, MODEM_CONTROL_CHANNEL_PORT_NAME, MODEM_TSY_NAME, MODEM_CSY_NAME,
	MODEM_DATA_BITS, MODEM_STOP_BITS, MODEM_PARITY, MODEM_RATE, MODEM_HANDSHAKING,
	MODEM_SPECIAL_RATE, MODEM_XON_CHAR, MODEM_XOFF_CHAR, MODEM_FAX_CLASS_PREF,
	MODEM_SPEAKER_PREF, MODEM_SPEAKER_VOL_PREF, MODEM_MODEM_INIT_STRING,
	MODEM_DATA_INIT_STRING, MODEM_FAX_INIT_STRING, MODEM_ISP_INIT_STRING,
	MODEM_DIAL_PAUSE_LENGTH, MODEM_CARRIER_TIMEOUT, MODEM_AUTO_ANSWER_RING_COUNT,
	MODEM_SPEAKER_VOL_CONTROL_LOW, MODEM_SPEAKER_VOL_CONTROL_MEDIUM,
	MODEM_SPEAKER_VOL_CONTROL_HIGH, MODEM_SPEAKER_ALWAYS_OFF,
	MODEM_SPEAKER_ON_UNTIL_CARRIER, MODEM_SPEAKER_ALWAYS_ON,
	MODEM_SPEAKER_ON_AFTER_DIAL_UNTIL_CARRIER, MODEM_DIAL_TONE_WAIT_MODIFIER,
	MODEM_CALL_PROGRESS_1, MODEM_CALL_PROGRESS_2, MODEM_CALL_PROGRESS_3,
	MODEM_CALL_PROGRESS_4, MODEM_ECHO_OFF, MODEM_VERBOSE_TEXT, MODEM_QUIET_ON,
	MODEM_QUIET_OFF, MODEM_DIAL_COMMAND_STATE_MODIFIER, MODEM_ON_LINE,
	MODEM_RESET_CONFIGURATION, MODEM_RETURN_TO_FACTORY_DEFS,
	MODEM_DCD_ON_DURING_LINK, MODEM_DTR_HANG_UP, MODEM_DSR_ALWAYS_ON,
	MODEM_RTS_CTS_HANDSHAKE, MODEM_XON_XOFF_HANDSHAKE, MODEM_ESCAPE_CHARACTER,
	MODEM_ESCAPE_GUARD_PERIOD, MODEM_FAX_CLASS_INTERROGATE, MODEM_FAX_CLASS,
	MODEM_NO_DIAL_TONE, MODEM_BUSY, MODEM_NO_ANSWER, MODEM_CARRIER, MODEM_CONNECT,
	MODEM_COMPRESSION_CLASS_5, MODEM_COMPRESSION_V42BIS, MODEM_COMPRESSION_NONE,
	MODEM_PROTOCOL_LAPD, MODEM_PROTOCOL_ALT, MODEM_PROTOCOL_ALTCELLULAR,
	MODEM_PROTOCOL_NONE, MODEM_MESSAGE_CENTRE_NUMBER, MODEM_MESSAGE_VALIDITY_PERIOD,
	MODEM_MESSAGE_DELIVERY_REPORT, MODEM_MIN_SIGNAL_LEVEL,
	LAST_SOCKET_ACTIVITY_TIMEOUT, LAST_SESSION_CLOSED_TIMEOUT, LAST_SOCKET_CLOSED_TIMEOUT,
	MODEM_SIR_SETTINGS, MODEM_COMM_ROLE, NO_MORE_RECORDS
	};

/**
list of columns in LANBEARER table
@internalComponent
*/
const TText* const LANBEARERcolumnArray[] =
	{	
	COMMDB_NAME,
	LAN_BEARER_AGENT, LAN_BEARER_NIF_NAME, LAN_BEARER_LDD_FILENAME, LAN_BEARER_LDD_NAME,
	LAN_BEARER_PDD_FILENAME, LAN_BEARER_PDD_NAME, LAN_BEARER_PACKET_DRIVER_NAME,
	LAST_SOCKET_ACTIVITY_TIMEOUT, LAST_SESSION_CLOSED_TIMEOUT, LAST_SOCKET_CLOSED_TIMEOUT,
	NO_MORE_RECORDS
	};

/**
list of columns in Location table
@internalComponent
*/
const TText* const LOCATIONcolumnArray[] =
	{	
	COMMDB_NAME,
	LOCATION_INTL_PREFIX_CODE, LOCATION_NAT_PREFIX_CODE, LOCATION_NAT_CODE,
	LOCATION_AREA_CODE, LOCATION_DIAL_OUT_CODE, LOCATION_DISABLE_CALL_WAITING_CODE,
	LOCATION_MOBILE, LOCATION_USE_PULSE_DIAL, LOCATION_WAIT_FOR_DIAL_TONE,
	LOCATION_PAUSE_AFTER_DIAL_OUT,
	NO_MORE_RECORDS
	};

/**
list of columns in Charge Card table
@internalComponent
*/
const TText* const CHARGECARDcolumnArray[] =
	{	
	COMMDB_NAME,
	CHARGECARD_ACCOUNT_NUMBER, CHARGECARD_PIN, CHARGECARD_LOCAL_RULE,
	CHARGECARD_NAT_RULE, CHARGECARD_INTL_RULE,
	NO_MORE_RECORDS
	};

/**
list of columns in Proxy table
@internalComponent
*/
const TText* const PROXIEcolumnArray[] =
	{	
    PROXY_ISP, PROXY_SERVICE_TYPE, PROXY_USE_PROXY_SERVER, PROXY_SERVER_NAME,
	PROXY_PROTOCOL_NAME, PROXY_PORT_NUMBER, PROXY_EXCEPTIONS,
	NO_MORE_RECORDS
	};

//DEPRECATED : Agent table has been removed/*

/**
list of columns in AgentLookup table
@internalComponent
*/
const TText* const AGENTLOOKUPcolumnArray[] =
	{
    AGENT_FRIENDLY_NAME, AGENT_FILENAME,
	NO_MORE_RECORDS
	};

/**
list of columns in WAP Access Point Table
@internalComponent
*/
const TText* const WAPACCESSPOINTcolumnArray[] =
	{	
	COMMDB_NAME,
	WAP_CURRENT_BEARER, WAP_START_PAGE,
	NO_MORE_RECORDS
	};

/**
list of columns in WAP IP Bearer Table
@internalComponent
*/
const TText* const WAPIPBEARERcolumnArray[] =
	{	
	WAP_ACCESS_POINT_ID, WAP_GATEWAY_ADDRESS, WAP_IAP,
	WAP_WSP_OPTION, WAP_SECURITY,
	//new column
	WAP_PROXY_PORT, WAP_PROXY_LOGIN_NAME, WAP_PROXY_LOGIN_PASS,
	NO_MORE_RECORDS
	};

/**
list of columns in WAP SMS Bearer Table
@internalComponent
*/
const TText* const WAPSMSBEARERcolumnArray[] =
	{	
	WAP_ACCESS_POINT_ID, WAP_GATEWAY_ADDRESS, WAP_SERVICE_CENTRE_ADDRESS,
	WAP_WSP_OPTION, WAP_SECURITY,
	NO_MORE_RECORDS
	};

/**
list of columns in Bluetooth table
@internalComponent
*/
const TText* const BLUETOOTHcolumnArray[] =
	{	
	BT_DEV_ADD, BT_DEV_DFLT_NAME, BT_DEV_FRIENDLY_NAME, BT_DEVICE_CLASS, BT_LINKKEY,
	BT_DEVICE_GLOBAL_SECURITY, BT_DEV_STATE,
	BT_PAGE_SCAN_REP_MODE, BT_PAGE_SCAN_PERIOD_MODE, BT_PAGE_SCAN_MODE,
	BT_SPARE, BT_CLK_OFFSET, BT_COD,
	NO_MORE_RECORDS
	};

/**
list of columns in Bluetooth default table
@internalComponent
*/
const TText* const BLUETOOTHDEFcolumnArray[] =
	{	
	BT_DFLT_SER_NAME, BT_DFLT_SER_UUID, BT_DFLT_VIRTUAL_PORT, BT_DFLT_DEV_ADD,
	BT_DFLT_SECURITY_LEVEL,
	NO_MORE_RECORDS
	};

/**
list of columns in Bluetooth security table
@internalComponent
*/
const TText* const BLUETOOTHSECURITYcolumnArray[] =
	{	
	BT_DEV_ADD, BT_SEC_DEVICE_SECURITY, BT_SEC_UID,
	NO_MORE_RECORDS
	};

/**
list of columns in BT Persistance table
@internalComponent
*/
const TText* const BLUETOOTHPERSISTcolumnArray[] =
	{
	BT_PERSIST_COD, BT_PERSIST_PAGE_TIMEOUT, BT_PERSIST_PAGE_SCAN_INTERVAL,
	BT_PERSIST_PAGE_SCAN_WINDOW,BT_PERSIST_INQUIRY_SCAN_INTERVAL,
	BT_PERSIST_INQUIRY_SCAN_WINDOW, BT_PERSIST_HOLD_MODE_ACTIVITY,
	BT_PERSIST_SCO_FLOW_CONTROL, BT_PERSIST_PAGE_SCAN_PERIOD,
	BT_PERSIST_PAGE_SCAN_MODE, BT_PERSIST_POWER_SETTINGS,
	BT_PERSIST_SCAN, BT_PERSIST_LOCAL_NAME,
	BT_PERSIST_IAC_LAP, BT_PERSIST_STATE, NO_MORE_RECORDS
	};

/**
list of columns in Connection Preferences table
@internalComponent
*/
/*const TText* const CONNECTPREFcolumnArray[] =
{
CONNECT_PREF_RANKING, CONNECT_PREF_DIRECTION, CONNECT_PREF_BEARER_SET,
CONNECT_PREF_DIALOG_PREF, CONNECT_PREF_IAP,
NO_MORE_RECORDS
};*/

/**
list of columns in Global Settings table
@internalComponent
*/
const TText* const GLOBALcolumnArray[] =
	{	COMMDB_NAME,
	WAP_ACCESS_POINT, REDIAL_ATTEMPTS, SMS_BEARER, SMS_RECEIVE_MODE, GPRS_ATTACH_MODE,
	ACCEPT_INCOMING_GPRS, GPRS_CLASS_C_BEARER,
	CONNECTION_ATTEMPTS, MODEM_DATA_FAX, MODEM_PHONE_SERVICES_SMS,
	LOCATION_DATA_FAX, LOCATION_PHONE_SERVICES_SMS,
	MAXMBUF_HEAP, DEFAULT_AGENT, DEFAULT_NETWORK, BEARER_AVAILABILITY_CHECK_TSY,
	// CDMA SimpleIP/MobileIP fields
	CDMA_OP_CAPABILITY, CDMA_SIMIP_MAX_NUM_NAI, CDMA_SIMIP_MAX_NAI_LENGTH,
	CDMA_SIMIP_MAX_SS_LENGTH, CDMA_SIMIP_AUTH_SUPPORTED, CDMA_MIP_MAX_NUM_NAI,
	CDMA_MIP_MAX_NAI_LENGTH, CDMA_MIP_MAX_MN_AAA_SS_LENGTH, CDMA_MIP_MN_AAA_AUTH_ALGORITHM,
	CDMA_MIP_MAX_MN_HA_SS_LENGTH, CDMA_MIP_MN_HA_AUTH_ALGORITHM,
	DEFAULT_SNAP,
	DEFAULT_TIER,
	WMM_ENABLED,
	NO_MORE_RECORDS
	};


/**
list of columns in Table Access table
@internalComponent
*/
const TText* const TABLEACCcolumnArray[] =
	{
	TABLE_ACC_NAME, 
	TABLE_ACC_TYPE,
	NO_MORE_RECORDS
	};

/**
list of columns in Security Socket table
@internalComponent
*/
const TText* const SScolumnArray[] =
	{
	SS_PROTO_NAME, SS_PROTO_LIBRARY,
	NO_MORE_RECORDS
	};

/**
list of columns in LAN SERVICE table
@internalComponent
*/
const TText* const LANSERVICEcolumnArray[] =
	{	
	COMMDB_NAME,
	LAN_IF_NETWORKS, LAN_IP_NETMASK,
	LAN_IP_GATEWAY, LAN_IP_ADDR_FROM_SERVER, LAN_IP_ADDR,
	LAN_IP_DNS_ADDR_FROM_SERVER, LAN_IP_NAME_SERVER1, LAN_IP_NAME_SERVER2,
	LAN_IP6_DNS_ADDR_FROM_SERVER, LAN_IP6_NAME_SERVER1, LAN_IP6_NAME_SERVER2,
	LAN_IP_ADDR_LEASE_VALID_FROM, LAN_IP_ADDR_LEASE_VALID_TO, LAN_CONFIG_DAEMON_MANAGER_NAME, LAN_CONFIG_DAEMON_NAME,
	LAN_SERVICE_EXTENSION_TABLE_NAME, LAN_SERVICE_EXTENSION_TABLE_RECORD_ID,
	LAN_SERVICE_ENABLE_LLMNR,
	NO_MORE_RECORDS
	};

/**
list of columns in NETWORK table
@internalComponent
*/
const TText* const NETWORKcolumnArray[] =
	{    
	COMMDB_NAME,
	HOST_NAME,
	NO_MORE_RECORDS
	};

/**
list of columns in VIRTUAL_BEARER table
@internalComponent
*/
const TText* const VIRTUAL_BEARERcolumnArray[] =
	{
	COMMDB_NAME,
	AGENT_NAME, IF_NAME,
	LAST_SOCKET_ACTIVITY_TIMEOUT, LAST_SESSION_CLOSED_TIMEOUT, LAST_SOCKET_CLOSED_TIMEOUT,
	NO_MORE_RECORDS
	};

/**
list of columns in VPN_SERVICE table
@internalComponent
*/
const TText* const VPN_SERVICEcolumnArray[] =
	{
	COMMDB_NAME,
	VPN_SERVICE_POLICY,VPN_SERVICE_IAP,VPN_SERVICE_NETWORKID,
	VPN_SERVICE_ENABLE_LLMNR,
	NO_MORE_RECORDS
	};

/**
list of columns in WLANEXTENSIONSERVICE table
@internalComponent
*/
const TText* const WLAN_SERVICEEXTENSIONcolumnArray[] =
	{
	COMMDB_NAME,
	WLAN_PROFILE,WLAN_SSID,WLAN_ENCRYPTION_STATUS,
	WLAN_ENCRYPTION_TYPE,WLAN_DEFAULT_WEP_KEY,
	WLAN_ENCRYPT_KEY1_VALID,WLAN_WEP_Key1,
	WLAN_ENCRYPT_KEY2_VALID,WLAN_WEP_KEY2,
	WLAN_ENCRYPT_KEY3_VALID,WLAN_WEP_KEY3,
	WLAN_ENCRYPT_KEY4_VALID,WLAN_WEP_KEY4,
	WLANRANKING,WLAN_DIALOG_PREF,WLAN_NETWORK_TYPE,
	WLAN_FRAG_THRES,WLAN_RTS_THRES,WLAN_DES_TRANS_RATE,
	WLAN_BEACON_INT,WLAN_CHANNEL_ID,WLAN_AUTHENTICATION_MODE,
	WLAN_POWERSAVE_MODE,WLAN_ALLOW_UNENCRYPT_MODE,WLAN_ALLOW_SHORT_PRE,
	WLAN_PREAMBLE_TYPE,WLAN_TX_POWER_LEVEL,WLAN_EAP_PRESHARED_KEY,
	WLAN_REG_DOMAIN,WLAN_SEC_DATA,
	NO_MORE_RECORDS
	};


/**
List of all tables in commdb, and the arrays of columns we are
able to modify that match to each table.  The order of both tables
must be identical!
@internalComponent
*/
const TText* const tableArray[] =
	{
	NETWORK,
	IAP,
	NO_MORE_RECORDS   
	};                
	/*const TText* const tableArray[] =
	{
	NETWORK,
	MODEM_BEARER,
	LAN_BEARER,
	LOCATION,
	CHARGECARD,
	GLOBAL_SETTINGS,
	DIAL_OUT_ISP,
	DIAL_IN_ISP,
	OUTGOING_GPRS,
	INCOMING_GPRS,
	DEFAULT_GPRS,
	LAN_SERVICE,
	IAP,
	CONNECTION_PREFERENCES,
	BT_DEVICE_TABLE,
	BT_SECURITY_TABLE,
	BT_DEFAULT_TABLE,
	PROXIES,
	AGENT_LOOKUP,
	WAP_ACCESS_POINT,
	WAP_IP_BEARER,
	WAP_SMS_BEARER,
	SS_PROTO_TABLE,
	BT_PERSIST_TABLE,
	VIRTUAL_BEARER,
	VPN_SERVICE,
	WLAN_SERVICE_EXTENSION_TABLE,	
	ACCESS_TYPE_TABLE,  // This must be the last entry in the table. Add new entries before it. 
	NO_MORE_RECORDS   
	};                    
*/

/**
List of all tables in CommDB as they appear in the XML
configuration file
*/

const TText* const xmlTableArray[] =
	{
	_S("Network"),
	_S("ModemBearer"),
	_S("LANBearer"),
	_S("Location"),
	_S("Chargecard"),
	_S("GlobalSettings"),
	_S("DialOutISP"),
	_S("DialInISP"),
	_S("OutgoingGPRS"),
	_S("IncomingGPRS"),
	_S("DefaultGPRS"),
	_S("LANService"),
	_S("IAP"),
	_S("ConnectionPreferences"),
	_S("BTDevice"),
	_S("BTSecurity"),
	_S("BTDefault"),
	_S("Proxy"),
	_S("AgentLookup"),
	_S("WAPAccessPoint"),
	_S("WAPIPBearer"),
	_S("WAPSMSBearer"),
	_S("SecureSocket"),
	_S("BTPersist"),
	_S("VirtualBearer"),
	_S("VpnService"),
	_S("WLANServiceExtension"),
	_S("AccessTypeTable"), // This must be the last entry in the table. Add new entries before it.
	NO_MORE_RECORDS   
	};     

/**
@internalComponent
*/
const TText** const ColumnArray[] =
	{
	(const TText** const)NETWORKcolumnArray,
	/*	(const TText** const)MODEMBEARERcolumnArray,
	(const TText** const)LANBEARERcolumnArray,
	(const TText** const)LOCATIONcolumnArray,
	(const TText** const)CHARGECARDcolumnArray,
	(const TText** const)GLOBALcolumnArray,
	(const TText** const)ISPOUTcolumnArray,
	(const TText** const)ISPINcolumnArray,
	(const TText** const)GPRScolumnArray,
	(const TText** const)GPRScolumnArray,
	(const TText** const)DEFGPRScolumnArray,
	(const TText** const)LANSERVICEcolumnArray,*/
	(const TText** const)IAPcolumnArray,
	/*	(const TText** const)CONNECTPREFcolumnArray,
	(const TText** const)BLUETOOTHcolumnArray,
	(const TText** const)BLUETOOTHSECURITYcolumnArray,
	(const TText** const)BLUETOOTHDEFcolumnArray,
	(const TText** const)PROXIEcolumnArray,
	(const TText** const)AGENTLOOKUPcolumnArray,
	(const TText** const)WAPACCESSPOINTcolumnArray,
	(const TText** const)WAPIPBEARERcolumnArray,
	(const TText** const)WAPSMSBEARERcolumnArray,
	(const TText** const)SScolumnArray,
	(const TText** const)BLUETOOTHPERSISTcolumnArray,
	(const TText** const)VIRTUAL_BEARERcolumnArray,
	(const TText** const)VPN_SERVICEcolumnArray,
	(const TText** const)WLAN_SERVICEEXTENSIONcolumnArray,*/
	(const TText** const)TABLEACCcolumnArray	// This must be the last colarray in the table.  	
	};											    // Add new tables before it.


													/**
													List of all columns in CommDB that have enumerated values, and
													the arrays of enumerations that match to each table.
													@internalComponent
*/
const TText* const ENUMcolumnArray[] =
	{
	ISP_TYPE,
	ISP_BEARER_NAME,
	ISP_BEARER_SPEED,
	ISP_BEARER_PROTOCOL,
	ISP_BEARER_CE,
	ISP_BEARER_TYPE,
	ISP_CHANNEL_CODING,
	ISP_AIUR,
	ISP_IF_CALLBACK_TYPE,
	MODEM_DATA_BITS,
	MODEM_STOP_BITS,
	MODEM_PARITY,
	MODEM_RATE,
	MODEM_FAX_CLASS_PREF,
	MODEM_SPEAKER_PREF,
	MODEM_SPEAKER_VOL_PREF,
	WAP_WSP_OPTION,
	/*	CONNECT_PREF_RANKING,
	CONNECT_PREF_DIRECTION,
	CONNECT_PREF_BEARER_SET,
	CONNECT_PREF_DIALOG_PREF,*/
	GPRS_PDP_TYPE,
	GPRS_CLASS_C_BEARER,
	CDMA_PDP_TYPE,
	CDMA_SERVICE_OPTION,
	CDMA_RLP_MODE,
	CDMA_REQ_FWD_PRIORITY,
	CDMA_REQ_REV_PRIORITY,
	CDMA_REQ_FWD_BITRATE,
	CDMA_REQ_REV_BITRATE,
	CDMA_REQ_FWD_LOSS,
	CDMA_REQ_REV_LOSS,
	CDMA_REQ_FWD_MAXDELAY,
	CDMA_REQ_REV_MAXDELAY,
	CDMA_MIN_FWD_BITRATE,
	CDMA_MIN_REV_BITRATE,
	CDMA_ACCPT_FWD_LOSS,
	CDMA_ACCPT_REV_LOSS,
	CDMA_ACCPT_FWD_MAXDELAY,
	CDMA_ACCPT_REV_MAXDELAY,
	NO_MORE_RECORDS
	};

	/*
	This should of the same order as gTableArray[] for tables defined above
*/

TMDBElementId elementIdArray[] =
	{
	//	KCDTIdConnectionPrefsRecord,		
	//	KCDTIdGlobalSettingsRecord,		 
	KCDTIdNetworkRecord,				
	KCDTIdIAPRecord,					
	//	KCDTIdLocationRecord,
	//	KCDTIdWAPAccessPointRecord,		
	//	KCDTIdDialOutISPRecord,			
	//	KCDTIdDialInISPRecord,			
	//	KCDTIdLANServiceRecord,			
	//	KCDTIdWLANServiceExtRecord,
	//	KCDTIdVPNServiceRecord,			
	//	KCDTIdOutgoingWCDMARecord,		
	//	KCDTIdIncomingWCDMARecord,		
	//	KCDTIdWCDMAPacketServiceRecord,	
	//	KCDTIdDefaultWCDMARecord,			
	//	KCDTIdModemBearerRecord,			
	//	KCDTIdLANBearerRecord,			
	//	KCDTIdVirtualBearerRecord,		
	//	KCDTIdWAPSMSBearerRecord,			  
	//	KCDTIdWAPIPBearerRecord,			
	//	KCDTIdChargecardRecord,	
	//	KCDTIdProxiesRecord,				
	//	KCDTIdSSProtoRecord,				
	//	KCDTIdAgentLookupRecord,			
	NULL	
	};


#endif
