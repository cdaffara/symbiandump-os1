
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

#ifndef DBDEF_H
#define DBDEF_H

#include <e32base.h>
#include <e32std.h>

//#include <commdb.h>//todo remove
#include <commsdattypesv1_1.h>	
#include <commsdattypeinfov1_1.h>
#include "commsdattypeinfov1_1_internal.h"
using namespace CommsDat;

#include <cdbtemp.h>
#include <d32comm.h>
#include <nifvar.h>
#include <etelmm.h>
#include <cdbcols.h>
#include "CDBSTD.H"
#include <cdbpreftable.h>
// Hex Uid presentation support.
//
_LIT(KHexMarker, "0x");
_LIT(KUidMarker, "Uid");

/**
access to local error strings
@internalComponent
*/
extern TPtrC errorCode[];

/**
local CED defines, for values not already defined by CommDB
*/
//#define CONNECTION_PREFS_OUT	_S("ConnectionPrefsOut")		// pref table outbound
//#define CONNECTION_PREFS_IN		_S("ConnectionPrefsIn")			// pref table inbound

/**
end of array
*/
#define NO_MORE_RECORDS			_S("NO_MORE_RECORDS")
const TMDBElementId NO_MORE_ID	= 0xFFFFFFFF;

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

#define OFF_VAL 				_S("Off")
#define ON_VAL 					_S("On")

/**
empty enum
*/
#define NOTHING					_S("")

/**
max amount of column updates per table we can handle
*/
#define MAX_DB_COLUMNS			100

/**
max number of characters for UINT
*/
#define MAX_CHAR_INT_LEN		9

/**
max length for column name
*/
#define MAX_COL_NAME_LEN		64

/**
max length of column contents
*/
#define MAX_COL_VAL_LEN			96

/**
max length of Med text column contents
*/
#define MAX_COL_MED_VAL_LEN		256

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
#endif // __COMMDB_ROHID_SUPPORT__
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

/**
list of values in ISP type column
@internalComponent
*/
const TText* const ENUM_ISP_TYPEArray[] =
	{
	_S("INTERNETONLY"), _S("WAPONLY"), _S("INTERNETANDWAP"),
		NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define BEARER_NAME_ASYNCHRONOUS			_S("ASYNCHRONOUS")
#define BEARER_NAME_ASYNCHRONOUSRDI			_S("ASYNCHRONOUSRDI")
#define BEARER_NAME_SYNCHRONOUS				_S("SYNCHRONOUS")
#define BEARER_NAME_SYNCHRONOUSRDI			_S("SYNCHRONOUSRDI")
#define BEARER_NAME_PADASYNCUDI				_S("PADASYNCUDI")
#define BEARER_NAME_PADASYNCRDI				_S("PADASYNCRDI")
#define BEARER_NAME_ACCESSSYNCUDI			_S("DADS")
#define BEARER_NAME_ACCESSSYNCRDI			_S("ACCESSSYNCRDI")
#define BEARER_NAME_SERVICEEXTENDED			_S("SERVICEEXTENDED")


/**
list of values in ISP Bearer name column
@internalComponent
*/
const TText* const ENUM_ISP_BEARERNAMEArray[] =
	{
	BEARER_NAME_ASYNCHRONOUS, BEARER_NAME_ASYNCHRONOUSRDI,
	BEARER_NAME_SYNCHRONOUS, BEARER_NAME_SYNCHRONOUSRDI,
	BEARER_NAME_PADASYNCUDI, BEARER_NAME_PADASYNCRDI,
	BEARER_NAME_ACCESSSYNCUDI, BEARER_NAME_ACCESSSYNCRDI,
	BEARER_NAME_SERVICEEXTENDED,
	NO_MORE_RECORDS
	};


/**
list of values in ISP Bearer speed column
@internalComponent
*/
const TText* const ENUM_ISP_BEARERSPEEDArray[] =
	{
	_S("UNSPECIFIED"), _S("AUTOBAUDING"), _S("2400"), _S("4800"), _S("9600"),
	_S("14400"), _S("19200"), _S("28800"), _S("32000"), _S("33600"), _S("38400"),
	_S("43200"), _S("48000"), _S("56000"), _S("57600"), _S("64000"),
	NO_MORE_RECORDS
	};



/**
list of values in ISP BearerPROTOCOL column
@internalComponent
*/
#define BEARER_PROTOCOL_UNSPECIFIED		_S("UNSPECIFIED")
#define BEARER_PROTOCOL_0				_S("0")
#define BEARER_PROTOCOL_V22BIS			_S("V22BIS")
#define BEARER_PROTOCOL_V32				_S("V32")
#define BEARER_PROTOCOL_V34				_S("V34")
#define BEARER_PROTOCOL_V110			_S("V110")
#define BEARER_PROTOCOL_V120			_S("V120")
#define BEARER_PROTOCOL_X31FLAGSTUFFING _S("X31FLAGSTUFFING")
#define BEARER_PROTOCOL_PIAFS			_S("PIAFS")
#define BEARER_PROTOCOL_BITTRANSPARENT  _S("BITTRANSPARENT")
#define BEARER_PROTOCOL_PSTNMULTIMEDIAVOICEFALLBACK  _S("PSTNMULTIMEDIAVOICEFALLBACK")
#define BEARER_PROTOCOL_PSTNMULTIMEDIA  _S("PSTNMULTIMEDIA")
#define BEARER_PROTOCOL_ISDNMULTIMEDIA  _S("ISDNMULTIMEDIA")


/**
@internalComponent
*/
const TText* const ENUM_ISP_BEARERPROTOCOLArray[] =
	{
	
	BEARER_PROTOCOL_UNSPECIFIED,	BEARER_PROTOCOL_0,
	BEARER_PROTOCOL_V22BIS,			BEARER_PROTOCOL_V32,
	BEARER_PROTOCOL_V34,			BEARER_PROTOCOL_V110,
	BEARER_PROTOCOL_V120,			BEARER_PROTOCOL_X31FLAGSTUFFING,
	BEARER_PROTOCOL_PIAFS,			BEARER_PROTOCOL_BITTRANSPARENT,
	BEARER_PROTOCOL_PSTNMULTIMEDIAVOICEFALLBACK,
	BEARER_PROTOCOL_PSTNMULTIMEDIA,	BEARER_PROTOCOL_ISDNMULTIMEDIA,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
typedef struct{
	const TText*   OptionDescriptor;
	TUint32 EnumVal;
	}TDescEnumMap;

/**
Map the descriptors used in the ced.cfg file to the correct enumertaion value used by the rest of the system.
NB this is not in enumeration order. The code uses RMobileCall::EProtocolUnspecified as
a termination condition, so the last entry is in fact redundant.
				
@internalComponent
*/
const TDescEnumMap  IspBearerProtocolMap[]= {
	{BEARER_PROTOCOL_V22BIS,RMobileCall::EProtocolV22bis},
	{BEARER_PROTOCOL_V32,RMobileCall::EProtocolV32},
	{BEARER_PROTOCOL_V34,RMobileCall::EProtocolV34},
	{BEARER_PROTOCOL_V110,RMobileCall::EProtocolV110},
	{BEARER_PROTOCOL_V120,RMobileCall::EProtocolV120},
	{BEARER_PROTOCOL_X31FLAGSTUFFING,RMobileCall::EProtocolX31FlagStuffing},
	{BEARER_PROTOCOL_PIAFS,RMobileCall::EProtocolPIAFS},
	{BEARER_PROTOCOL_BITTRANSPARENT,RMobileCall::EProtocolBitTransparent},
	{BEARER_PROTOCOL_PSTNMULTIMEDIAVOICEFALLBACK,RMobileCall::EProtocolPstnMultimediaVoiceFallback},
	{BEARER_PROTOCOL_PSTNMULTIMEDIA,RMobileCall::EProtocolPstnMultimedia},
	{BEARER_PROTOCOL_ISDNMULTIMEDIA,RMobileCall::EProtocolIsdnMultimedia},
	{BEARER_PROTOCOL_UNSPECIFIED,RMobileCall::EProtocolUnspecified},
	{BEARER_PROTOCOL_0,RMobileCall::EProtocolUnspecified}
	};


/**
list of values in ISP BearerCE column
@internalComponent
*/
const TText* const ENUM_ISP_BEARERCEArray[] =
	{
	_S("UNSPECIFIED"), _S("TRANSPARENT"), _S("NONTRANSPARENT"),
	_S("TRANSPARENTPREF"),_S("NONTRANSPARENTPREF"),
	NO_MORE_RECORDS
	};

/**
list of values in ISP bearer type column
@internalComponent
*/
const TText* const ENUM_ISP_BEARER_TYPEArray[] =
	{
	_S("CSD"), _S("HSCSD"),
	NO_MORE_RECORDS
	};


/**
list of values in ISP channel coding column
@internalComponent
*/
const TText* const ENUM_ISP_CHANNEL_CODINGArray[] =
	{
	_S("UNSPECIFIED"), _S("48"), _S("96"), _S("144"),
	_S("288"), _S("320"), _S("432"),
	NO_MORE_RECORDS
	};

/**
list of values in ISP AIUR column
@internalComponent
*/
const TText* const ENUM_ISP_AIURArray[] =
	{
	_S("UNSPECIFIED"),
	_S("9600"), _S("14400"), _S("19200"), _S("28800"),
	_S("38400"), _S("43200"), _S("57600"),
	NO_MORE_RECORDS
	};

/**
#defines because we reference them in the CPP code
@internalComponent
*/
#define MSCBCPCLIENTNUM		_S("MSCBCPCLIENTNUM")
#define MSCBCPSERVERNUM		_S("MSCBCPSERVERNUM")
#define MSCBCPOVERRIDESER	_S("MSCBCPOVERRIDESER")

/**
list of values in callback type column
@internalComponent
*/
const TText* const ENUM_ISP_IFCALLBACKTYPEArray[] =
	{
	_S("IETF0"), _S("IETF1"), _S("IETF2"), _S("IETF3"), _S("IETF4"), _S("IETF5"),
	MSCBCPCLIENTNUM, MSCBCPSERVERNUM, MSCBCPOVERRIDESER,
	NO_MORE_RECORDS
	};

/**
list of values in gprs pdp type column
@internalComponent
*/
const TText* const ENUM_GPRS_PDP_TYPEArray[] =
	{
	_S("IPV4"), _S("IPV6"), _S("X25"), _S("PPP"), _S("CDPD"),
	NO_MORE_RECORDS
	};

/**
list of values in default gprs pdp type column
@internalComponent
*/
const TText* const ENUM_CDMA_PDP_TYPEArray[] =
	{
	_S("IPV4"), _S("IPV6"), _S("X25"), _S("PPP"), _S("CDPD"),
	NO_MORE_RECORDS
	};

/**
list of values in start bit column
@internalComponent
*/
const TText* const ENUM_MODEM_STARTArray[] =
	{
	_S("5"), _S("6"), _S("7"), _S("8"),
	NO_MORE_RECORDS
	};

/**
list of values in stop bit column
@internalComponent
*/
const TText* const ENUM_MODEM_STOPArray[] =
	{
	_S("1"), _S("2"),
	NO_MORE_RECORDS
	};

/**
list of values in parity column
@internalComponent
*/
const TText* const ENUM_MODEM_PARITYArray[] =
	{
	_S("NONE"), _S("EVEN"), _S("ODD"), _S("MARK"), _S("SPACE"),
	NO_MORE_RECORDS
	};

/**
	#defines because we reference them in the CPP code
@internalComponent
*/
#define MODEM_RATE_AUTO	_S("AUTO")
#define MODEM_RATE_SPECIAL	_S("SPECIAL")

/**
list of values in rate column
@internalComponent
*/
const TText* const ENUM_MODEM_RATEArray[] =
	{
	_S("50"), _S("75"), _S("110"), _S("134"), _S("150"),
	_S("300"), _S("600"), _S("1200"), _S("1800"), _S("2000"),
	_S("2400"), _S("3600"), _S("4800"), _S("7200"), _S("9600"),
	_S("19200"), _S("38400"), _S("57600"), _S("115200"),
	_S("230400"), _S("460800"), _S("576000"), _S("1152000"), _S("4000000"),
	MODEM_RATE_AUTO, MODEM_RATE_SPECIAL,
	NO_MORE_RECORDS
	};



/**
list of values in fax class preference column
@internalComponent
*/
#define MODEM_FAX_AUTO			_S("AUTO")
#define MODEM_FAX_1				_S("1")
#define MODEM_FAX_2 			_S("2")
#define MODEM_FAX_2POINT0		_S("2.0")
#define MODEM_FAX_1POINT0		_S("1.0")
#define MODEM_FAX_2POINT1		_S("2.1")

/**
@internalComponent
*/
const TText* const ENUM_MODEM_FAX_CLASSArray[] =
	{
	_S("AUTO"), _S("1"), _S("2"), _S("2.0"), _S("1.0"), _S("2.1"),
	NO_MORE_RECORDS
	};

/**
list of values in modem speaker preference column
@internalComponent
*/
const TText* const ENUM_MODEM_SPEAKER_PREFArray[] =
	{
	_S("NEVER"), _S("UNTILCALLANSWERED"), _S("ALWAYS"), _S("AFTERDIALUNTILANSWER"),
	NO_MORE_RECORDS
	};

/**
list of values in modem speaker volumn column
@internalComponent
*/
const TText* const ENUM_MODEM_SPEAKER_VOL_PREFArray[] =
	{
	_S("QUIET"), _S("MEDIUM"), _S("LOUD"),
	NO_MORE_RECORDS
	};

/**
list of values in WAP WSP option column
@internalComponent
*/
const TText* const ENUM_WAP_WSP_OPTIONArray[] =
	{
	_S("CONNECTIONLESS"), _S("CONNECTIONORIENTED"),
	NO_MORE_RECORDS
	};

/**
list of values in connection prefs ranking column
@internalComponent
*/
const TText* const ENUM_CONNECT_PREF_RANKINGArray[] =
	{
	_S("0"), _S("1"), _S("2"),
	NO_MORE_RECORDS
	};

/**
list of values in connection preference direction column
@internalComponent
*/
const TText* const ENUM_CONNECT_PREF_DIRECTIONArray[] =
	{
	_S("UNKNOWN"), _S("OUTGOING"), _S("INCOMING"),
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define BEARER_SET_UNKNOWN		_S("UNKNOWN")
#define BEARER_SET_CSD			_S("CSD")
#define BEARER_SET_WCDMA		_S("WCDMA")
#define BEARER_SET_LAN			_S("LAN")
#define BEARER_SET_PAN			_S("PAN")
#define BEARER_SET_WLAN			_S("WLAN")
#define BEARER_SET_PSD			_S("PSD")
#define BEARER_SET_CSD_WCDMA    _S("CSD & WCDMA")
#define BEARER_SET_CSD_PSD      _S("CSD & PSD")
#define BEARER_SET_VIRTUAL			_S("Virtual")

/**
list of values in connection preference bearer column
@internalComponent
*/
const TText* const ENUM_CONNECT_PREF_BEARER_SETArray[] =
	{
	BEARER_SET_UNKNOWN, BEARER_SET_CSD, BEARER_SET_WCDMA, BEARER_SET_LAN,
	BEARER_SET_PSD, BEARER_SET_CSD_WCDMA,
	BEARER_SET_CSD_PSD,BEARER_SET_VIRTUAL, BEARER_SET_PAN, BEARER_SET_WLAN,
	NO_MORE_RECORDS
	};


/**
list of values in connection preference dialog column
@internalComponent
*/
const TText* const ENUM_CONNECT_PREF_DIALOG_PREFArray[] =
	{
	_S("UNKNOWN"), _S("PROMPT"), _S("WARN"), _S("DONOTPROMPT"),
	_S("PROMPTIFWRONGMODE"),
	NO_MORE_RECORDS
	};

/**
list of values in gprs bearer column
@internalComponent
*/
const TText* const ENUM_GPRS_CLASS_C_BEARERArray[] =
	{
	_S("GPRS"), _S("GSM"),
	NO_MORE_RECORDS
	};
/**
list of values in GPRS R5 Data Compression column
@internalComponent
*/
const TText* const ENUM_GPRS_R5_DATA_COMPRESSIONArray[] =
	{
	_S("NoDataCompression"), _S("ManufacturerDefaultDataCompression"),
	_S("V42"), _S("V44"),
	NO_MORE_RECORDS
	};

/**
list of values in GPRS R5 Header Compression column
@internalComponent
*/
const TText* const ENUM_GPRS_R5_HEADER_COMPRESSIONArray[] =
	{
	_S("NoHeaderCompression"), _S("ManufacturerDefaultHeaderCompression"),
	_S("Rfc1144"), _S("Rfc2507"),  _S("Rohc"),
	NO_MORE_RECORDS
	};

/**
list of values in GPRS Flow Identifier column
@internalComponent
*/
const TText* const ENUM_GPRS_PACKET_FLOW_IDENTIFIERArray[] =
	{
	_S("BestEffort"), _S("Signalling"), _S("Sms"),
	NO_MORE_RECORDS
	};

/**
list of values in GPRS UMTS GPRS Releases Indicator column
@internalComponent
*/
const TText* const ENUM_GPRS_UMTS_GPRS_RELEASEArray[] =
	{
	_S("Unspecified"), _S("ConfigGPRS"), _S("ConfigCDMA"), _S("Unspecified"), _S("ConfigRel99Rel4"), _S("ConfigRel5"),
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define CDMA_SERVICE_LOWSPEEDDATA							_S("LOWSPEEDDATA")
#define CDMA_SERVICE_HIGHSPEEDDATA							_S("HIGHSPEEDDATA")
#define CDMA_SERVICE_LOW_AND_HIGHSPEEDDATA					_S("LOW_AND_HIGHSPEEDDATA")

/**
list of values in cdma service option column
@internalComponent
*/
const TText* const ENUM_CDMA_SERVICE_OPTIONArray[] =
	{
	CDMA_SERVICE_LOWSPEEDDATA,
	CDMA_SERVICE_HIGHSPEEDDATA,
	CDMA_SERVICE_LOW_AND_HIGHSPEEDDATA,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define CDMA_RLP_UNKNOWN			_S("UNKNOWN")
#define CDMA_RLP_TRANSPARENT		_S("TRANSPARENT")
#define CDMA_RLP_NONTRANSPARENT		_S("NONTRANSPARENT")
#define CDMA_RLP_TRANSPARENTPREF	_S("TRANSPARENTPREF")
#define CDMA_RLP_NONTRANSPARENTPREF _S("NONTRANSPARENTPREF")

/**
list of values in cdma rlp mode column
@internalComponent
*/
const TText* const ENUM_CDMA_RLP_MODEArray[] =
	{
	CDMA_RLP_UNKNOWN, CDMA_RLP_TRANSPARENT, CDMA_RLP_NONTRANSPARENT,
	CDMA_RLP_TRANSPARENTPREF, CDMA_RLP_NONTRANSPARENTPREF,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define CDMA_PRIORITY_00		 _S("PRIORITY00")
#define CDMA_PRIORITY_01		 _S("PRIORITY01")
#define CDMA_PRIORITY_02		 _S("PRIORITY02")
#define CDMA_PRIORITY_03		 _S("PRIORITY03")
#define CDMA_PRIORITY_04		 _S("PRIORITY04")
#define CDMA_PRIORITY_05		 _S("PRIORITY05")
#define CDMA_PRIORITY_06		 _S("PRIORITY06")
#define CDMA_PRIORITY_07		 _S("PRIORITY07")
#define CDMA_PRIORITY_08		 _S("PRIORITY08")
#define CDMA_PRIORITY_09		 _S("PRIORITY09")
#define CDMA_PRIORITY_10		 _S("PRIORITY10")
#define CDMA_PRIORITY_11		 _S("PRIORITY11")
#define CDMA_PRIORITY_12		 _S("PRIORITY12")
#define CDMA_PRIORITY_13		 _S("PRIORITY13")

/**
list of values in cdma req fwd AND rev priority column
@internalComponent
*/
const TText* const ENUM_CDMA_PRIORITYArray[] =
	{
	CDMA_PRIORITY_00,
	CDMA_PRIORITY_01, CDMA_PRIORITY_02, CDMA_PRIORITY_03,
	CDMA_PRIORITY_04, CDMA_PRIORITY_05, CDMA_PRIORITY_06,
	CDMA_PRIORITY_07, CDMA_PRIORITY_08, CDMA_PRIORITY_09,
	CDMA_PRIORITY_10, CDMA_PRIORITY_11, CDMA_PRIORITY_12,
	CDMA_PRIORITY_13,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define CDMA_BITRATE_8				_S("8KBPS")
#define CDMA_BITRATE_32				_S("32KBPS")
#define CDMA_BITRATE_64				_S("64KBPS")
#define CDMA_BITRATE_144			_S("144KBPS")
#define CDMA_BITRATE_384			_S("384KBPS")

/**
list of values in cdma req fwd bitrate, rev bitrate
min fwd bitrate, and min rev bitrate column
@internalComponent
*/
const TText* const ENUM_CDMA_BITRATEArray[] =
	{
	CDMA_BITRATE_8, CDMA_BITRATE_32, CDMA_BITRATE_64,
	CDMA_BITRATE_144, CDMA_BITRATE_384,
	NO_MORE_RECORDS
	};

/**
list of values in cdma req fwd loss, rev loss,
	accept fwd loss, and accept rev loss column
@internalComponent
*/
#define CDMA_LOSS_1				_S("LOSS1")
#define CDMA_LOSS_2				_S("LOSS2")
#define CDMA_LOSS_5				_S("LOSS5")
#define CDMA_LOSS_10			_S("LOSS10")

/**
@internalComponent
*/
const TText* const ENUM_CDMA_LOSSArray[] =
	{
	CDMA_LOSS_1, CDMA_LOSS_2, CDMA_LOSS_5, CDMA_LOSS_10,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define CDMA_MAXDELAY_CLASS					_S("CLASS")
#define CDMA_MAXDELAY_CLASS1				_S("CLASS1")
#define CDMA_MAXDELAY_CLASS2				_S("CLASS2")
#define CDMA_MAXDELAY_CLASS3				_S("CLASS3")
#define CDMA_MAXDELAY_CLASS4				_S("CLASS4")
#define CDMA_MAXDELAY_40MS					_S("40MS")
#define CDMA_MAXDELAY_120MS					_S("120MS")
#define CDMA_MAXDELAY_360MS					_S("360MS")

/**
list of values in cdma req fwd maxdelay, rev maxdelay
and accpt fwd maxdelay, accpt rev maxdelay column
@internalComponent
*/
const TText* const ENUM_CDMA_MAXDELAYArray[] =
	{
	CDMA_MAXDELAY_CLASS, CDMA_MAXDELAY_CLASS1, CDMA_MAXDELAY_CLASS2, CDMA_MAXDELAY_CLASS3,
	CDMA_MAXDELAY_CLASS4, CDMA_MAXDELAY_40MS, CDMA_MAXDELAY_120MS, CDMA_MAXDELAY_360MS,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
#define PAN_ROLE_U			_S("PANU")
#define PAN_ROLE_GN			_S("PANGN")
#define PAN_ROLE_NAP		_S("PANNAP")
#define PAN_ROLE_UNKNOWN	_S("PANUNKNOWN")

/**
List of allowable values for roles in Bluetooth PAN profile
@note Strictly, there are actually BNEP values
@internalComponent
*/
const TText* const ENUM_PAN_ROLEArray[] = 
{
	PAN_ROLE_U, PAN_ROLE_GN, PAN_ROLE_NAP, PAN_ROLE_UNKNOWN,
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
#ifdef __COMMDB_ROHID_SUPPORT__	
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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
    COMMDB_ID,
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
    COMMDB_ID,
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
	GPRS_AP_TYPE, GPRS_QOS_WARNING_TIMEOUT,GPRS_SERVICE_ENABLE_LLMNR, 
	QOS_UMTS_R99_AND_ON_TABLE,
	GPRS_R5_DATA_COMPRESSION,
	GPRS_R5_HEADER_COMPRESSION,
	GPRS_R4_PACKET_FLOW_IDENTIFIER,
	GPRS_UMTS_GPRS_RELEASE,
	NO_MORE_RECORDS
	};


const TText* const GPRSQoSR99columnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
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
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	MODEM_AGENT, MODEM_NIF_NAME, MODEM_BCA_STACK,
	MODEM_PORT_NAME, MODEM_CONTROL_CHANNEL_PORT_NAME, MODEM_TSY_NAME, MODEM_CSY_NAME,
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
	MODEM_SIR_SETTINGS, MODEM_COMM_ROLE, BEARER_TECHNOLOGY, NO_MORE_RECORDS
	};

/**
list of columns in LANBEARER table
@internalComponent
*/
const TText* const LANBEARERcolumnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	LAN_BEARER_AGENT, LAN_BEARER_NIF_NAME, LAN_BEARER_LDD_FILENAME, LAN_BEARER_LDD_NAME,
	LAN_BEARER_PDD_FILENAME, LAN_BEARER_PDD_NAME, LAN_BEARER_PACKET_DRIVER_NAME,
	LAST_SOCKET_ACTIVITY_TIMEOUT, LAST_SESSION_CLOSED_TIMEOUT, LAST_SOCKET_CLOSED_TIMEOUT,
	BEARER_TECHNOLOGY, NO_MORE_RECORDS
	};

/**
list of columns in Location table
@internalComponent
*/
const TText* const LOCATIONcolumnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
	BT_DEV_ADD, BT_SEC_DEVICE_SECURITY, BT_SEC_UID,
	NO_MORE_RECORDS
	};

/**
list of columns in BT Persistance table
@internalComponent
*/
const TText* const BLUETOOTHPERSISTcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
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
const TText* const CONNECTPREFcolumnArray[] =
	{
    COMMDB_ID,
	COMMDB_NAME,
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
	CONNECT_PREF_RANKING, CONNECT_PREF_DIRECTION, CONNECT_PREF_BEARER_SET,
	CONNECT_PREF_DIALOG_PREF, CONNECT_PREF_IAP,
	NO_MORE_RECORDS
	};

/**
list of columns in Global Settings table
@internalComponent
*/
const TText* const GLOBALcolumnArray[] =
	{

#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,

	COMMDB_NAME,
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
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	DEFAULT_SNAP,
	PROMPT_USER,
	DEFAULT_TIER,
#else
    //Don't know why do the fields have different names in different settings...
    DEFAULT_AP, // = DEFAULT_SNAP
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    PROMPT_FOR_AP, // = PROMPT_USER
	REGULATORY_DOMAIN,
	WMM_ENABLED,
	ENABLE_COUNTRYCODE,
	DEFAULT_REGULATORY_DOMAIN,
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
#ifdef __COMMDB_ROHID_SUPPORT__	
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	SS_PROTO_NAME, SS_PROTO_LIBRARY,
	NO_MORE_RECORDS
	};

/**
list of columns in LAN SERVICE table
@internalComponent
*/
const TText* const LANSERVICEcolumnArray[] =
	{	
#ifdef __COMMDB_ROHID_SUPPORT__	
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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

// list of columns in PAN SERVICE table
const TText* const PANSERVICEEXTENSIONScolumnArray[] = 
{
    COMMDB_ID,
	COMMDB_NAME, 
	PAN_LOCAL_ROLE, PAN_PEER_ROLE, PAN_PEER_MAC_ADDRESSES,
	PAN_PROMPT_FOR_REMOTE_DEVICES, PAN_DISABLE_SDP_QUERY,
	PAN_ALLOW_INCOMING, PAN_PROMPT_IF_MAC_LIST_FAILS,
#ifdef SYMBIAN_NETWORKING_DHCPSERVER
	PAN_NAP_SERVICE_ENABLED,
#endif // SYMBIAN_NETWORKING_DHCPSERVER	
	NO_MORE_RECORDS
};

/**
list of columns in NETWORK table
@internalComponent
*/
const TText* const NETWORKcolumnArray[] =
	{    
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	AGENT_NAME, IF_NAME,
	LAST_SOCKET_ACTIVITY_TIMEOUT, LAST_SESSION_CLOSED_TIMEOUT, LAST_SOCKET_CLOSED_TIMEOUT,
	BEARER_TECHNOLOGY, NO_MORE_RECORDS
	};

/**
list of columns in VPN_SERVICE table
@internalComponent
*/
const TText* const VPN_SERVICEcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__	
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	VPN_SERVICE_POLICY,VPN_SERVICE_IAP,KCDTypeNameVPNSNAPRecord,VPN_SERVICE_NETWORKID,
	VPN_SERVICE_ENABLE_LLMNR,
	NO_MORE_RECORDS
	};

/**
list of columns in WLANEXTENSIONSERVICE table
@internalComponent
*/
const TText* const WLAN_SERVICEEXTENSIONcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
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

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
list of columns in GENERICQOS table
@internalComponent
*/
const TText* const GENERICQOScolumnArray[] =
	{
	STYPEID,
	DOWNLINKBANDWIDTH,
	UPLINKBANDWIDTH,
	DOWNLINKMAXIMUMBURSTSIZE,
	UPLINKMAXIMUMBURSTSIZE,
	DOWNLINKAVERAGEPACKETSIZE,
	UPLINKAVERAGEPACKETSIZE,
	DOWNLINKMAXIMUMPACKETSIZE,
	UPLINKMAXIMUMPACKETSIZE,
	DOWNLINKDELAY,
	UPLINKDELAY,
	DOWNLINKDELAYVARIATION,
	UPLINKDELAYVARIATION,
	DOWNLINKPRIORITY,
	UPLINKPRIORITY,
	HEADERMODE,
//	QOSNAME
	NO_MORE_RECORDS
	};

/**
list of columns in POLICYSELECTOR table
@internalComponent
*/
const TText* const POLICYSELECTORcolumnArray[] =
	{
	POLICYID,
	APPUID,
	SRCADDRESS,
	SRCMASK,
	DSTADDRESS,
	DSTMASK,
	SRCPORT,
	DSTPORT,
	SRCPORTMAX,
	DSTPORTMAX,
	PROTOCOLID,
	IAPID,
	PRIORITY,
	NO_MORE_RECORDS
	};

/**
list of columns in POLICYSELECTOR2PARAMS table
@internalComponent
*/
const TText* const POLICYSELECTOR2PARAMScolumnArray[] =
	{
	POLICYID,
	PARAMSID,
	NO_MORE_RECORDS
	};

/**
list of columns in WIFISCANENGINE table
@internalComponent
*/
const TText* const WIFISCANENGINEcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif
    COMMDB_ID,
	COMMDB_NAME,
	SCANPERIODMS,
	RSSIMIN,
	RSSIMAX,
	NO_MORE_RECORDS
	};

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	
/**
List of boolean lookup values
@internalComponent
*/
const TUint ENUM_WLAN_BOOLEAN_VALArray[] =
	{
	EFalse,
	ETrue
	};

/**
list of values in WLAN Encryption Status column
@internalComponent
*/
const TText* const ENUM_WLAN_ENCRYPTION_STATUSArray[] =
	{
	OFF_VAL, 
	ON_VAL, 
	NO_MORE_RECORDS
	};

/**
list of values in WLAN Allow Unencrypted Mode column
@internalComponent
*/
const TText* const ENUM_WLAN_ALLOW_UNENCRYPT_MODEArray[] =
	{
	FALSE_VAL, 
	TRUE_VAL, 
	NO_MORE_RECORDS
	};

/**
list of values in WLAN Encryption Type column
@internalComponent
*/
const TText* const ENUM_WLAN_ENCRYPTION_TYPEArray[] =
	{
	_S("NONE"), 
	_S("Static WEP"), 
	_S("TKIP"), 
	_S("AES"), 
	/*_S("Dynamic WEP"),*/
	NO_MORE_RECORDS
	};

/**
list of lookup values in WLAN Encryption Type column
@internalComponent
*/
const TUint ENUM_WLAN_ENCRYPTION_TYPE_VALArray[] =
	{
	ECommsDatWlanEncryptionTypeNone
	,ECommsDatWlanEncryptionTypeStaticWEP
	,ECommsDatWlanEncryptionTypeTKIP
	,ECommsDatWlanEncryptionTypeAES
	/*,ECommsDatWlanEncryptionTypeDynamicWEP*/
	};

/**
list of values in WLAN Dialog Preference column
@internalComponent
*/
const TText* const ENUM_WLAN_DIALOG_PREFArray[] =
	{
	_S("UNKNOWN"), 
	_S("PROMPT"), 
	_S("DONOTPROMPT"), 
	_S("WARN"),
	NO_MORE_RECORDS
	};

/**
list of lookup values in WLAN Dialog Preference column
@internalComponent
*/
const TUint ENUM_WLAN_DIALOG_PREF_VALArray[] =
	{
	ECommsDatWlanDialogPrefUnknown,
	ECommsDatWlanDialogPrefPrompt,
	ECommsDatWlanDialogPrefNoPrompt,
	ECommsDatWlanDialogPrefWarn
	};

/**
list of values in WLAN Network Type column
@internalComponent
*/
const TText* const ENUM_WLAN_NETWORK_TYPEArray[] =
	{
	_S("Infrastructure"), 
	_S("Ad-Hoc"),
	NO_MORE_RECORDS
	};

/**
list of lookup values in WLAN Network Type column
@internalComponent
*/
const TUint ENUM_WLAN_NETWORK_TYPE_VALArray[] =
	{
	ECommsDatWlanNetworkTypeInfrastructure,
	ECommsDatWlanNetworkTypeAdHoc
	};

/**
list of values in WLAN Desired Transmit Rate column
@internalComponent
*/
const TText* const ENUM_WLAN_DES_TRANS_RATEArray[] =
	{
	_S("1 Mbps"), 
	_S("2 Mbps"), 
	_S("5.5 Mbps"), 
	_S("6 Mbps"), 
	_S("9 Mbps"), 
	_S("11 Mbps"), 
	_S("12 Mbps"), 
	_S("18 Mbps"),
	_S("22 Mbps"),
	_S("24 Mbps"),
	_S("33 Mbps"),
	_S("36 Mbps"),
	_S("48 Mbps"),
	_S("54 Mbps"),
	_S("Auto"),
	NO_MORE_RECORDS
	};
	
/**
list of lookup values in WLAN Desired Transmit Rate column
@internalComponent
*/
const TUint ENUM_WLAN_DES_TRANS_RATE_VALArray[] =
	{
	ECommsDatWlanDesTransRate1Mbps,
	ECommsDatWlanDesTransRate2Mbps,
	ECommsDatWlanDesTransRate5Mbps,
	ECommsDatWlanDesTransRate6Mbps,
	ECommsDatWlanDesTransRate9Mbps,
	ECommsDatWlanDesTransRate11Mbps,
	ECommsDatWlanDesTransRate12Mbps,
	ECommsDatWlanDesTransRate18Mbps,
	ECommsDatWlanDesTransRate22Mbps,
	ECommsDatWlanDesTransRate24Mbps,
	ECommsDatWlanDesTransRate33Mbps,
	ECommsDatWlanDesTransRate36Mbps,
	ECommsDatWlanDesTransRate48Mbps,
	ECommsDatWlanDesTransRate54Mbps,
	ECommsDatWlanDesTransRateAuto
	};

/**
list of values in WLAN Power Save Mode column
@internalComponent
*/
const TText* const ENUM_WLAN_POWERSAVE_MODEArray[] =
	{
	_S("Disabled"), 
	_S("Fast Powersave mode"), 
	_S("Max Powersave mode"),
	NO_MORE_RECORDS
	};

/**
list of lookup values in WLAN Power Save Mode column
@internalComponent
*/
const TUint ENUM_WLAN_POWERSAVE_MODE_VALArray[] =
	{
	ECommsDatWlanPowerSaveModeDisabled,
	ECommsDatWlanPowerSaveModeFast,
	ECommsDatWlanPowerSaveModeMax
	};

/**
list of values in WLAN Allow Short Preamble column
@internalComponent
*/
const TText* const ENUM_WLAN_ALLOW_SHORT_PREArray[] =
	{
	FALSE_VAL, 
	TRUE_VAL,
	NO_MORE_RECORDS
	};

/**
list of values in WLAN Preamble Type column
@internalComponent
*/
const TText* const ENUM_WLAN_PREAMBLE_TYPEArray[] =
	{
	_S("Short"), 
	_S("Long"), 
	NO_MORE_RECORDS
	};

/**
list of lookup values in WLAN Preamble Type column
@internalComponent
*/
const TUint ENUM_WLAN_PREAMBLE_TYPE_VALArray[] =
	{
	ECommsDatWlanPreambleTypeShort,
	ECommsDatWlanPreambleTypeLong
	};

/**
list of values in WLAN Regulatory Domain column
@internalComponent
*/
const TText* const ENUM_WLAN_REG_DOMAINArray[] =
	{
	_S("FCC"), 
	_S("IC"), 
	_S("ETSI"), 
	_S("Spain"), 
	_S("France"), 
	_S("MKK"), 
	_S("AT"),
	_S("AU"),
	_S("BE"),
	_S("BR"),
	_S("CA"),
	_S("CH"),
	_S("CN"),
	_S("CY"),
	_S("CZ"),
	_S("DE"),
	_S("DK"),
	_S("EE"),
	_S("ES"),
	_S("FI"),
	_S("FR"),
	_S("GB"),
	_S("GR"),
	_S("HK"),
	_S("HU"),
	_S("ID"),
	_S("IE"),
	_S("IL"),
	_S("IS"),
	_S("IT"),
	_S("JP"),
	_S("KR"),
	_S("LT"),
	_S("LU"),
	_S("LV"),
	_S("MY"),
	_S("NL"),
	_S("NO"),
	_S("NZ"),
	_S("PH"),
	_S("PL"),
	_S("PT"),
	_S("SE"),
	_S("SG"),
	_S("SI"),
	_S("SK"),
	_S("TH"),
	_S("TW"),
	_S("US"),
	_S("ZA"),
	_S("World"),
	NO_MORE_RECORDS
	};

/**
list of lookup values in WLAN Regulatory Domain column
@internalComponent
*/
const TUint ENUM_WLAN_REG_DOMAIN_VALArray[] =
	{
	ECommsDatWlanRegDomainFCC,
	ECommsDatWlanRegDomainIC,
	ECommsDatWlanRegDomainETSI,
	ECommsDatWlanRegDomainSpain,
	ECommsDatWlanRegDomainFrance,
	ECommsDatWlanRegDomainMKK,
	ECommsDatWlanRegDomainAT,
	ECommsDatWlanRegDomainAU,
	ECommsDatWlanRegDomainBE,
	ECommsDatWlanRegDomainBR,
	ECommsDatWlanRegDomainCA,
	ECommsDatWlanRegDomainCH,
	ECommsDatWlanRegDomainCN,
	ECommsDatWlanRegDomainCY,
	ECommsDatWlanRegDomainCZ,
	ECommsDatWlanRegDomainDE,
	ECommsDatWlanRegDomainDK,
	ECommsDatWlanRegDomainEE,
	ECommsDatWlanRegDomainES,
	ECommsDatWlanRegDomainFI,
	ECommsDatWlanRegDomainFR,
	ECommsDatWlanRegDomainGB,
	ECommsDatWlanRegDomainGR,
	ECommsDatWlanRegDomainHK,
	ECommsDatWlanRegDomainHU,
	ECommsDatWlanRegDomainID,
	ECommsDatWlanRegDomainIE,
	ECommsDatWlanRegDomainIL,
	ECommsDatWlanRegDomainIS,
	ECommsDatWlanRegDomainIT,
	ECommsDatWlanRegDomainJP,
	ECommsDatWlanRegDomainKR,
	ECommsDatWlanRegDomainLT,
	ECommsDatWlanRegDomainLU,
	ECommsDatWlanRegDomainLV,
	ECommsDatWlanRegDomainMY,
	ECommsDatWlanRegDomainNL,
	ECommsDatWlanRegDomainNO,
	ECommsDatWlanRegDomainNZ,
	ECommsDatWlanRegDomainPH,
	ECommsDatWlanRegDomainPL,
	ECommsDatWlanRegDomainPT,
	ECommsDatWlanRegDomainSE,
	ECommsDatWlanRegDomainSG,
	ECommsDatWlanRegDomainSI,
	ECommsDatWlanRegDomainSK,
	ECommsDatWlanRegDomainTH,
	ECommsDatWlanRegDomainTW,
	ECommsDatWlanRegDomainUS,
	ECommsDatWlanRegDomainZA,
	ECommsDatWlanRegDomainWorld
	};

/**
list of values in WLAN Authentication Mode column
@internalComponent
*/
const TText* const ENUM_WLAN_AUTHENTICATION_MODEArray[] =
	{
	_S("Open"), 
	_S("Shared"), 
	_S("WPA"), 
	_S("WPA-PSK"), 
	_S("Open with EAP"), 
	_S("WPA2"), 
	_S("WPA2-PSK"), 
	_S("EAP-WPS"), 
	NO_MORE_RECORDS
	};

/**
list of lookup values in WLAN Authentication Mode column
@internalComponent
*/
const TUint ENUM_WLAN_AUTHENTICATION_MODE_VALArray[] =
	{
	ECommsDatWlanAuthenticationModeOpen,
	ECommsDatWlanAuthenticationModeShared,
	ECommsDatWlanAuthenticationModeWPA,
	ECommsDatWlanAuthenticationModeWPAPSK,
	ECommsDatWlanAuthenticationModeOPENEAP,
	ECommsDatWlanAuthenticationModeWPA2,
    ECommsDatWlanAuthenticationModeWPA2PSK,
    ECommsDatWlanAuthenticationModeEapWps
	};

/**
list of columns in Access Point table
@internalComponent
*/
const TText* const ACCESSPOINTcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    /*#@deprecated with SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY maps to ACCESS_POINT_TIER*/
    ACCESS_POINT_GID,   
    ACCESS_POINT_POLICY,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
     //this is the same field as pointed to by the ACCESS_POINT_POLICY
     //because in the vFuture env both field names should be valid
     //because of the configuration files...
    //OLD_ACCESS_POINT_POLICY,
    ACCESS_POINT_TIER,
    ACCESS_POINT_MCPR,
    ACCESS_POINT_CPR,
    ACCESS_POINT_SCPR,
    ACCESS_POINT_PROTOCOL,
    ACCESS_POINT_CPRCONFIGID,
    ACCESS_POINT_APPSID,
    ACCESS_POINT_CONFIG_AP_ID_LIST,
    ACCESS_POINT_CUSTOM_SELECTION_POLICY,
    ACCESS_POINT_PRIORITY,
#endif
	NO_MORE_RECORDS
};

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
list of columns in Config Access Point table
@internalComponent
*/
const TText* const CONFIGACCESSPOINTcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__	
    COMMDB_ID,
    COMMDB_NAME,
	ACCESS_POINT_SCPR,
	ACCESS_POINT_PROTOCOL,
	PROTOCOL_CONFIG,
	LAYER_BELOW,
	NO_MORE_RECORDS
};


/**
list of columns in TIER table
@internalComponent
*/
const TText* const TIERcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    TIER_IMPL_UID,
    TIER_THREAD_NAME,
    TIER_MANAGER_NAME,
    DEFAULT_ACCESS_POINT,
    PROMPT_USER,
	NO_MORE_RECORDS
};

/**
list of columns in MCPR table
@internalComponent
*/
const TText* const MCPRcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__	
    COMMDB_ID,
    COMMDB_NAME,
    MCPR_UID,
	NO_MORE_RECORDS
};

/**
list of columns in CPR table
@internalComponent
*/
const TText* const CPRcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__	
    COMMDB_ID,
    COMMDB_NAME,
    CPR_UID,
	NO_MORE_RECORDS
};

/**
list of columns in SCPR table
@internalComponent
*/
const TText* const SCPRcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    SCPR_UID,
	NO_MORE_RECORDS
};

/**
list of columns in PROTOCOL table
@internalComponent
*/
const TText* const PROTOCOLcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif	 // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    PROTOCOL_UID,
	PROTOCOL_CONFIG_LOADER_UID,
	NO_MORE_RECORDS
};
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

const TText* const IAPPRIORITYSELECTIONPOLICYcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    SELECTION_POLICY_IAP1,
    SELECTION_POLICY_IAP2,
    SELECTION_POLICY_IAP3,
    SELECTION_POLICY_IAP4,
    SELECTION_POLICY_IAP5,
    SELECTION_POLICY_IAP6,
    SELECTION_POLICY_IAP7,
    SELECTION_POLICY_IAP8,
    SELECTION_POLICY_IAP9,
    SELECTION_POLICY_IAP10,
    SELECTION_POLICY_IAP11,
    SELECTION_POLICY_IAP12,
    SELECTION_POLICY_IAP13,
    SELECTION_POLICY_IAP14,
    SELECTION_POLICY_IAP15,
    SELECTION_POLICY_IAPCOUNT,
	NO_MORE_RECORDS
	};

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
list of columns in AP Priority Selection Policy table
@internalComponent
*/
const TText* const APPRIORITYSELECTIONPOLICYcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    COMMDB_ID,
#endif
    COMMDB_NAME,
    SELECTION_POLICY_AP1,
    SELECTION_POLICY_AP2,
    SELECTION_POLICY_AP3,
    SELECTION_POLICY_AP4,
    SELECTION_POLICY_AP5,
    SELECTION_POLICY_AP6,
    SELECTION_POLICY_AP7,
    SELECTION_POLICY_AP8,
    SELECTION_POLICY_AP9,
    SELECTION_POLICY_AP10,
    SELECTION_POLICY_AP11,
    SELECTION_POLICY_AP12,
    SELECTION_POLICY_AP13,
    SELECTION_POLICY_AP14,
    SELECTION_POLICY_AP15,
    SELECTION_POLICY_APCOUNT,
	NO_MORE_RECORDS
	};
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	
/** 
list of columns in EAP_SEC_TABLE 
@internalComponent
*/
const TText* const EAP_SEC_TABLEcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	EAP_SEC_OUTER_EAP_TYPE,
	EAP_SEC_ID,
	EAP_SEC_PASSWORD,
	EAP_SEC_CONFIGID,
	EAP_SEC_DATA,
    EAP_SEC_VENDORID,
    EAP_SEC_VENDORTYPE,
	NO_MORE_RECORDS
	};

/**
list of values in EAP Security Outer Method Type column
@internalComponent
*/
const TText* const ENUM_EAP_SEC_OUTER_EAP_TYPEArray[] =
	{
	_S("None"), 
	_S("EAP-MD5"), 
	_S("EAP-TLS"), 
	_S("EAP-TTLS"), 
	_S("PEAP"), 
	_S("LEAP"), 
	_S("EAP-SIM"), 
	_S("EAP-AKA"), 
	_S("EAP-MSCHAPv2"), 
	_S("EAP-WPS"),
	NO_MORE_RECORDS
	};

/**
list of lookup values in EAP Security Outer Method Type column
@internalComponent
*/
const TUint ENUM_EAP_SEC_OUTER_EAP_TYPE_VALArray[] =
	{
	ECommsDatEAPSecNone,
	ECommsDatEAPSecEAPMD5,
	ECommsDatEAPSecEAPTLS,
	ECommsDatEAPSecEAPTTLS,
	ECommsDatEAPSecPEAP,
	ECommsDatEAPSecLEAP,
	ECommsDatEAPSecEAPSIM,
	ECommsDatEAPSecEAPAKA,
	ECommsDatEAPSecEAPMSCHAPv2,
	ECommsDatEAPSecEAPWPS
	};

/**
list of columns in TUN_EAP_TABLE 
@internalComponent
*/
const TText* const TUN_EAP_TABLEcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	TUN_EAP_INNERTYPE,
	TUN_EAP_DATA,
	NO_MORE_RECORDS
	};

/**
list of values in EAP Security Inner Method Type column
@internalComponent
*/
const TText* const ENUM_TUN_EAP_INNERTYPEArray[] =
	{
	_S("None"), 
	_S("EAP-MD5"), 
	_S("EAP-MSCHAPv2"),
	NO_MORE_RECORDS
	};

/**
list of lookup values in EAP Security Inner Method Type column
@internalComponent
*/
const TUint ENUM_TUN_EAP_INNERTYPE_VALArray[] =
	{
	ECommsDatTunEapNone, 
	ECommsDatTunEapEAPMD5, 
	ECommsDatTunEapMSCHAPv2 
	};

/**
list of columns in EAP_TLS_TABLE 
@internalComponent
*/
const TText* const EAP_TLS_TABLEcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	EAP_TLS_CLIENT_CERT_NAME,
	NO_MORE_RECORDS
	};


/**
list of columns in LEAP_TABLE 
@internalComponent
*/
const TText* const LEAP_TABLEcolumnArray[] =
	{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
	COMMDB_NAME,
	EAP_LEAP_TIMEOUT,
	NO_MORE_RECORDS
	};

/**
list of columns in EAP-SIM CF Protocol table
@internalComponent
*/
const TText* const EapSimPROTOCOLcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    EAPSIM_PROTOCOL_ID,
    EAPSIM_PROTOCOL_PSEUDONYM,
    EAPSIM_PROTOCOL_USE_NAI_REALM,
	EAPSIM_PROTOCOL_NAI_REALM,
	EAPSIM_PROTOCOL_MIN_RANDS,
	NO_MORE_RECORDS
};


/**
list of columns in EAP-AKA CF Protocol table
@internalComponent
*/
const TText* const EapAkaPROTOCOLcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    EAPAKA_PROTOCOL_ID,
    EAPAKA_PROTOCOL_PSEUDONYM,
    EAPAKA_PROTOCOL_USE_NAI_REALM,
	EAPAKA_PROTOCOL_NAI_REALM,
	NO_MORE_RECORDS
};


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
const TText* const BEARERTYPEcolumnArray[] = 
{
#ifdef __COMMDB_ROHID_SUPPORT__
	COMMDB_READ_ONLY,
	COMMDB_HIDDEN,
#endif // __COMMDB_ROHID_SUPPORT__
    COMMDB_ID,
    COMMDB_NAME,
    ACCESS_POINT_TIER,
    ACCESS_POINT_MCPR,
    ACCESS_POINT_CPR,
    ACCESS_POINT_SCPR,
    ACCESS_POINT_PROTOCOL,
	NO_MORE_RECORDS
};
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 

/**
list of all tables in commdb, and the arrays of columns we are
	able to modify that match to each table.
	
The order of 'tableArray', 'xmlTableArray', 'ColumnArray' and 'elementIdArray' must be identical!

Remembering that, for XML parameters that get fixed up (ie links to recordIds of value '-1'), 
there are more lookup tables in cxmlcontenthandler.h (modTable, modParam...).

@internalComponent
*/
const TText* const tableArray[] =
	{
	NETWORK,
	MODEM_BEARER,
	LAN_BEARER,
	LOCATION,
	CHARGECARD,
	//GLOBAL_SETTINGS,
	DIAL_OUT_ISP,
	DIAL_IN_ISP,
	OUTGOING_GPRS,
	INCOMING_GPRS,
	DEFAULT_GPRS,
	LAN_SERVICE,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // These record types need to be present in the database before the IAP records are processed.
    //These tables are needed for the access_point_table
    AP_PRIORITY_SELECTION_POLICY_TABLE,
    TIER_TABLE,
	MCPR_TABLE,
	CPR_TABLE,
	SCPR_TABLE,
	PROTOCOL_TABLE,
	VIRTUAL_BEARER,
	BEARER_TYPE_TABLE,
	CONFIG_ACCESS_POINT_TABLE,
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    ACCESS_POINT_TABLE,
    GLOBAL_SETTINGS,
	IAP,
	IAP_PRIORITY_SELECTION_POLICY_TABLE,
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
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	VIRTUAL_BEARER,
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	
	VPN_SERVICE,
	WLAN_SERVICE_EXTENSION_TABLE,	
	PAN_SERVICE_EXTENSIONS,
	EAP_SEC_TABLE,			
	TUN_EAP_TABLE,			
	EAP_TLS_TABLE,			
	LEAP_TABLE,
	//IAP_PRIORITY_SELECTION_POLICY_TABLE,
	//ACCESS_POINT_TABLE,
	QOS_UMTS_R99_AND_ON_TABLE,
    EAPSIM_PROTOCOL_TABLE,
    EAPAKA_PROTOCOL_TABLE,  
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	POLICY_SELECTOR_TABLE,
	POLICYSELECTOR2QOSPARAMETERS_TABLE,	
	GENERIC_QOS_TABLE,
	WIFI_SCANENGINE_TABLE,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	NO_MORE_RECORDS     // This must be the last entry in the table. Add new entries before it. 
	};                    



#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
List of all Mesh marker tables in commdb. If we see any of these, presume that the input cfg file has been updated and
some attempt has been made to make it mesh machine compatible.
	
@internalComponent
*/

const TText* const tableMeshMarkerArray[] =
    {
  /*  ACCESS_POINT_TABLE, this is also around in pre mesh configs*/
	TIER_TABLE,
	MCPR_TABLE,
	CPR_TABLE,
	SCPR_TABLE,
	PROTOCOL_TABLE,
	BEARER_TYPE_TABLE,
	CONFIG_ACCESS_POINT_TABLE,
	NO_MORE_RECORDS   // This must be the last entry in the table. Add new entries before it. 
	};                    


const TText* const tableXMLMeshMarkerArray[] =
    {
    XML_ACCESS_POINT_TABLE,
    XML_CONFIG_ACCESS_POINT_TABLE,
	XML_TIER_TABLE,
	XML_MCPR_TABLE,
	XML_CPR_TABLE,
	XML_SCPR_TABLE,
	XML_PROTOCOL_TABLE,
	XML_BEARER_TYPE_TABLE,
	NO_MORE_RECORDS   // This must be the last entry in the table. Add new entries before it. 
	};                    

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
list of all tables in CommDB as they appear in the XML
	configuration file
*/

const TText* const xmlTableArray[] =
	{
	_S("Network"),
	_S("ModemBearer"),
	_S("LANBearer"),
	_S("Location"),
	_S("Chargecard"),
	//_S("GlobalSettings"),
	_S("DialOutISP"),
	_S("DialInISP"),
	_S("OutgoingGPRS"),
	_S("IncomingGPRS"),
	_S("DefaultGPRS"),
	_S("LANService"),
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // These record types need to be present in the database before the IAP records are processed.
    //These tables are needed for the access_point_table
    _S("APPrioritySelectionPolicy"),
    XML_TIER_TABLE,
	XML_MCPR_TABLE,
	XML_CPR_TABLE,
	XML_SCPR_TABLE,
	XML_PROTOCOL_TABLE,
	_S("VirtualBearer"),
	XML_BEARER_TYPE_TABLE,
	XML_CONFIG_ACCESS_POINT_TABLE,
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    XML_ACCESS_POINT_TABLE,
    _S("GlobalSettings"),
	_S("IAP"),
	_S("IAPPrioritySelectionPolicy"),
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
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	_S("VirtualBearer"),
#endif	// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	_S("VpnService"),
	_S("WLANServiceExtension"),
	_S("PANServiceExtensions"),
	_S("EAPSecuritySettings"),
	_S("TunnelledEAPSettings"),
	_S("EAPTLSSettings"),
	_S("LEAPSettings"),
	//_S("IAPPrioritySelectionPolicy"),
//	XML_ACCESS_POINT_TABLE,
//#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	_S("UmtsR99QoSAndOn"),
	_S("EapSimProtocol"),
	_S("EapAkaProtocol"),
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	XML_POLICY_SELECTOR_TABLE,
	XML_POLICYSELECTOR2QOSPARAMETERS_TABLE,
	XML_GENERIC_QOS_TABLE,
#endif
	NO_MORE_RECORDS   // This must be the last entry in the table. Add new entries before it.
	};                      

/**
@internalComponent
*/
const TText** const ColumnArray[] =
	{
	(const TText** const)NETWORKcolumnArray,
	(const TText** const)MODEMBEARERcolumnArray,
	(const TText** const)LANBEARERcolumnArray,
	(const TText** const)LOCATIONcolumnArray,
	(const TText** const)CHARGECARDcolumnArray,
	//(const TText** const)GLOBALcolumnArray,
	(const TText** const)ISPOUTcolumnArray,
	(const TText** const)ISPINcolumnArray,
	(const TText** const)GPRScolumnArray, 
	(const TText** const)GPRScolumnArray, 
	(const TText** const)DEFGPRScolumnArray,
	(const TText** const)LANSERVICEcolumnArray,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // These record types need to be present in the database before the IAP records are processed.
    //These tables are needed for the access_point_table
    (const TText** const)APPRIORITYSELECTIONPOLICYcolumnArray,
    (const TText** const)TIERcolumnArray,
	(const TText** const)MCPRcolumnArray,
	(const TText** const)CPRcolumnArray,
	(const TText** const)SCPRcolumnArray,
	(const TText** const)PROTOCOLcolumnArray,
	(const TText** const)VIRTUAL_BEARERcolumnArray,
	(const TText** const)BEARERTYPEcolumnArray,
	(const TText** const)CONFIGACCESSPOINTcolumnArray,
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    (const TText** const)ACCESSPOINTcolumnArray,
    (const TText** const)GLOBALcolumnArray,
	(const TText** const)IAPcolumnArray,
	(const TText** const)IAPPRIORITYSELECTIONPOLICYcolumnArray,
	(const TText** const)CONNECTPREFcolumnArray,
	(const TText** const)BLUETOOTHcolumnArray, //todo
	(const TText** const)BLUETOOTHSECURITYcolumnArray, //todo
	(const TText** const)BLUETOOTHDEFcolumnArray,//todo
	(const TText** const)PROXIEcolumnArray,
	(const TText** const)AGENTLOOKUPcolumnArray,
	(const TText** const)WAPACCESSPOINTcolumnArray,
	(const TText** const)WAPIPBEARERcolumnArray,
	(const TText** const)WAPSMSBEARERcolumnArray,
	(const TText** const)SScolumnArray,
	(const TText** const)BLUETOOTHPERSISTcolumnArray,//todo
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	(const TText** const)VIRTUAL_BEARERcolumnArray,
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	(const TText** const)VPN_SERVICEcolumnArray,
	(const TText** const)WLAN_SERVICEEXTENSIONcolumnArray, 
	(const TText** const)PANSERVICEEXTENSIONScolumnArray,
	(const TText** const)EAP_SEC_TABLEcolumnArray,
	(const TText** const)TUN_EAP_TABLEcolumnArray,
	(const TText** const)EAP_TLS_TABLEcolumnArray,
	(const TText** const)LEAP_TABLEcolumnArray,
	//(const TText** const)IAPPRIORITYSELECTIONPOLICYcolumnArray,
//	(const TText** const)ACCESSPOINTcolumnArray,
//#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	(const TText** const)GPRSQoSR99columnArray,
    (const TText** const)EapSimPROTOCOLcolumnArray,
	(const TText** const)EapAkaPROTOCOLcolumnArray,	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	(const TText** const)POLICYSELECTORcolumnArray,
	(const TText** const)POLICYSELECTOR2PARAMScolumnArray,
	(const TText** const)GENERICQOScolumnArray,
	(const TText** const)WIFISCANENGINEcolumnArray,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	(const TText** const)TABLEACCcolumnArray	// This must be the last colarray in the table.
	};											    // Add new tables before it.


const TMDBElementId elementIdArray[] =
	{
	KCDTIdNetworkRecord,
	KCDTIdModemBearerRecord,
	KCDTIdLANBearerRecord,
	KCDTIdLocationRecord,
	KCDTIdChargecardRecord,
	//KCDTIdGlobalSettingsRecord,	
	KCDTIdDialOutISPRecord,
	KCDTIdDialInISPRecord,
	KCDTIdOutgoingGprsRecord,
	KCDTIdIncomingGprsRecord,
	KCDTIdDefaultWCDMARecord,
	KCDTIdLANServiceRecord,	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // These record types need to be present in the database before the IAP records are processed.
    //These tables are needed for the access_point_table
    KCDTIdApPrioritySelectionPolicyRecord,
    KCDTIdTierRecord,
	KCDTIdMCprRecord,
	KCDTIdCprRecord,
	KCDTIdSCprRecord,
	KCDTIdProtocolRecord,
	KCDTIdVirtualBearerRecord,
    KCDTIdBearerTypeRecord,
	KCDTIdConfigAccessPointRecord,
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    KCDTIdAccessPointRecord,
    KCDTIdGlobalSettingsRecord,
	KCDTIdIAPRecord,
	KCDTIdIapPrioritySelectionPolicyRecord,
	KCDTIdConnectionPrefsRecord,
	0,
	0,
	0,//  TODO bluetooth TypeID	?,
	KCDTIdProxiesRecord,
	KCDTIdAgentLookupRecord,
	KCDTIdWAPAccessPointRecord,
	KCDTIdWAPIPBearerRecord,
	KCDTIdWAPSMSBearerRecord,	
	KCDTIdSSProtoRecord,
	0,//  TODO bluetooth TypeID	?,
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	KCDTIdVirtualBearerRecord,
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	KCDTIdVPNServiceRecord,	
	KCDTIdWLANServiceExtRecord,		
	KCDTIdPANServiceExtRecord,
	KCDTIdEAPSecRecord,
	KCDTIdTunEAPRecord,
	KCDTIdEAPTLSRecord,
	KCDTIdLEAPRecord,
	//KCDTIdIapPrioritySelectionPolicyRecord,
//#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	//KCDTIdAccessPointRecord,
//#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	KCDTIdUmtsR99QoSAndOnTableRecord,
	KCDTIdEapSimProtocolRecord,
	KCDTIdEapAkaProtocolRecord,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	KCDTIdPolicySelectorRecord,
	KCDTIdPolicySelector2ParamsRecord,
	KCDTIdGenericQosRecord,
	KCDTIdWifiScanEngineRecord,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	//AccessTypeTable
	NO_MORE_ID
	};



/**
list of all columns in CommDB that have enumerated values, and
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
	CONNECT_PREF_RANKING,
	CONNECT_PREF_DIRECTION,
	CONNECT_PREF_BEARER_SET,
	CONNECT_PREF_DIALOG_PREF,
	GPRS_PDP_TYPE,
	GPRS_CLASS_C_BEARER,
	GPRS_R5_DATA_COMPRESSION,
	GPRS_R5_HEADER_COMPRESSION,
	GPRS_R4_PACKET_FLOW_IDENTIFIER,
	GPRS_UMTS_GPRS_RELEASE,
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
	PAN_LOCAL_ROLE,
	PAN_PEER_ROLE,
	WLAN_ENCRYPTION_STATUS,
	WLAN_ENCRYPTION_TYPE,
	WLAN_DIALOG_PREF,
	WLAN_NETWORK_TYPE,
	WLAN_DES_TRANS_RATE,
	WLAN_POWERSAVE_MODE,
	WLAN_ALLOW_UNENCRYPT_MODE,
	WLAN_ALLOW_SHORT_PRE,
	WLAN_PREAMBLE_TYPE,
	WLAN_AUTHENTICATION_MODE,
	WLAN_REG_DOMAIN,
	EAP_SEC_OUTER_EAP_TYPE,
	TUN_EAP_INNERTYPE,
	REGULATORY_DOMAIN,
	DEFAULT_REGULATORY_DOMAIN,
	NO_MORE_RECORDS
	};


/**
@internalComponent
*/

const TText* const ENUM_MiscArray[] =
	{
	GPRS_PDP_TYPE,
	GPRS_CLASS_C_BEARER,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
const TText** const ENUM_MiscValueArray[] =
	{
	(const TText** const)ENUM_GPRS_PDP_TYPEArray,
	(const TText** const)ENUM_GPRS_CLASS_C_BEARERArray
	};


	/*
list of all enum columns in COnnection Pref tables in CommDB
@internalComponent
*/

const TText* const ENUM_ConnectionPrefArray[] =
	{
	CONNECT_PREF_RANKING,
	CONNECT_PREF_DIRECTION,
	CONNECT_PREF_BEARER_SET,
	CONNECT_PREF_DIALOG_PREF,
	NO_MORE_RECORDS
	};

/**
@internalComponent
*/
const TText** const ENUM_ConnectionPrefValueArray[] =
	{
	(const TText** const)ENUM_CONNECT_PREF_RANKINGArray,
	(const TText** const)ENUM_CONNECT_PREF_DIRECTIONArray,
	(const TText** const)ENUM_CONNECT_PREF_BEARER_SETArray,
	(const TText** const)ENUM_CONNECT_PREF_DIALOG_PREFArray,
	};

/**
list of all columns in Modem Bearer tables in CommDB that have enumerated values
@internalComponent
*/

const TText* const ENUM_ModemBearerArray[] =
	{
	MODEM_DATA_BITS,
	MODEM_STOP_BITS,
	MODEM_PARITY,
	MODEM_SPEAKER_PREF,
	MODEM_SPEAKER_VOL_PREF,
	NO_MORE_RECORDS
	};


/**
@internalComponent
*/
const TText** const ENUM_ModemValueArray[] =
	{
	(const TText** const)ENUM_MODEM_STARTArray,
	(const TText** const)ENUM_MODEM_STOPArray,
	(const TText** const)ENUM_MODEM_PARITYArray,
	(const TText** const)ENUM_MODEM_SPEAKER_PREFArray,
	(const TText** const)ENUM_MODEM_SPEAKER_VOL_PREFArray	
	};



/**
list of columns in ISP tables in CommDB that have enumerated values
@internalComponent
*/

const TText* const ENUM_ISPArray[] =
	{
	ISP_TYPE,
	ISP_BEARER_SPEED,
	ISP_BEARER_CE,
	ISP_BEARER_TYPE,
	ISP_CHANNEL_CODING,
	ISP_AIUR,	
	NO_MORE_RECORDS
	};


/**
@internalComponent
*/
const TText** const ENUM_ISPValueArray[] =
	{
	(const TText** const)ENUM_ISP_TYPEArray,
	(const TText** const)ENUM_ISP_BEARERSPEEDArray,
	(const TText** const)ENUM_ISP_BEARERCEArray,
	(const TText** const)ENUM_ISP_BEARER_TYPEArray,
	(const TText** const)ENUM_ISP_CHANNEL_CODINGArray,
	(const TText** const)ENUM_ISP_AIURArray
	};


/**
list of columns in CDMA tables in CommDB that have enumerated values
@internalComponent
*/

const TText* const ENUM_CDMAArray[] =
	{
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


/**
@internalComponent
*/
const TText** const ENUM_CDMAValueArray[] =
	{
	(const TText** const)ENUM_CDMA_PDP_TYPEArray,
	(const TText** const)ENUM_CDMA_SERVICE_OPTIONArray,
	(const TText** const)ENUM_CDMA_RLP_MODEArray,
	(const TText** const)ENUM_CDMA_PRIORITYArray,
	(const TText** const)ENUM_CDMA_PRIORITYArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray
	};


/**
@internalComponent
*/
const TText** const EnumerationArray[] =
	{
	(const TText** const)ENUM_ISP_TYPEArray,
	(const TText** const)ENUM_ISP_BEARERNAMEArray,
	(const TText** const)ENUM_ISP_BEARERSPEEDArray,
	(const TText** const)ENUM_ISP_BEARERPROTOCOLArray,
	(const TText** const)ENUM_ISP_BEARERCEArray,
	(const TText** const)ENUM_ISP_BEARER_TYPEArray,
	(const TText** const)ENUM_ISP_CHANNEL_CODINGArray,
	(const TText** const)ENUM_ISP_AIURArray,
	(const TText** const)ENUM_ISP_IFCALLBACKTYPEArray,
	(const TText** const)ENUM_MODEM_STARTArray,
	(const TText** const)ENUM_MODEM_STOPArray,
	(const TText** const)ENUM_MODEM_PARITYArray,
	(const TText** const)ENUM_MODEM_RATEArray,
	(const TText** const)ENUM_MODEM_FAX_CLASSArray,
	(const TText** const)ENUM_MODEM_SPEAKER_PREFArray,
	(const TText** const)ENUM_MODEM_SPEAKER_VOL_PREFArray,
	(const TText** const)ENUM_WAP_WSP_OPTIONArray,
	(const TText** const)ENUM_CONNECT_PREF_RANKINGArray,
	(const TText** const)ENUM_CONNECT_PREF_DIRECTIONArray,
	(const TText** const)ENUM_CONNECT_PREF_BEARER_SETArray,
	(const TText** const)ENUM_CONNECT_PREF_DIALOG_PREFArray,
	(const TText** const)ENUM_GPRS_PDP_TYPEArray,
	(const TText** const)ENUM_GPRS_CLASS_C_BEARERArray,
	(const TText** const)ENUM_GPRS_R5_DATA_COMPRESSIONArray,
	(const TText** const)ENUM_GPRS_R5_HEADER_COMPRESSIONArray,
	(const TText** const)ENUM_GPRS_PACKET_FLOW_IDENTIFIERArray,
	(const TText** const)ENUM_GPRS_UMTS_GPRS_RELEASEArray,
	(const TText** const)ENUM_CDMA_PDP_TYPEArray,
	(const TText** const)ENUM_CDMA_SERVICE_OPTIONArray,
	(const TText** const)ENUM_CDMA_RLP_MODEArray,
	(const TText** const)ENUM_CDMA_PRIORITYArray,
	(const TText** const)ENUM_CDMA_PRIORITYArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_BITRATEArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_LOSSArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray,
	(const TText** const)ENUM_CDMA_MAXDELAYArray,
	(const TText** const)ENUM_PAN_ROLEArray,
	(const TText** const)ENUM_PAN_ROLEArray,
	(const TText** const)ENUM_WLAN_ENCRYPTION_STATUSArray,
	(const TText** const)ENUM_WLAN_ENCRYPTION_TYPEArray,
	(const TText** const)ENUM_WLAN_DIALOG_PREFArray,
	(const TText** const)ENUM_WLAN_NETWORK_TYPEArray,
	(const TText** const)ENUM_WLAN_DES_TRANS_RATEArray,
	(const TText** const)ENUM_WLAN_POWERSAVE_MODEArray,
	(const TText** const)ENUM_WLAN_ALLOW_UNENCRYPT_MODEArray,
	(const TText** const)ENUM_WLAN_ALLOW_SHORT_PREArray,
	(const TText** const)ENUM_WLAN_PREAMBLE_TYPEArray,
	(const TText** const)ENUM_WLAN_AUTHENTICATION_MODEArray,
	(const TText** const)ENUM_WLAN_REG_DOMAINArray,
	(const TText** const)ENUM_EAP_SEC_OUTER_EAP_TYPEArray,
	(const TText** const)ENUM_TUN_EAP_INNERTYPEArray,
	(const TText** const)ENUM_WLAN_REG_DOMAINArray // regulatory domain in global settings table
	};

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

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
/**
List of deprecated parameters
@internalComponent
*/
const TText* const DeprecParamArray[] =
	{
	 GPRS_REQ_PRECEDENCE,		//OutgoingGprs, IncomingGprs
	 GPRS_REQ_DELAY,			//OutgoingGprs, IncomingGprs
	 GPRS_REQ_RELIABILITY,		//OutgoingGprs, IncomingGprs
	 GPRS_REQ_PEAK_THROUGHPUT,	//OutgoingGprs, IncomingGprs
	 GPRS_REQ_MEAN_THROUGHPUT,	//OutgoingGprs, IncomingGprs
	 GPRS_MIN_PRECEDENCE,		//OutgoingGprs, IncomingGprs
	 GPRS_MIN_DELAY,				//OutgoingGprs, IncomingGprs
	 GPRS_MIN_RELIABILITY,		//OutgoingGprs, IncomingGprs
	 GPRS_MIN_PEAK_THROUGHPUT,	//OutgoingGprs, IncomingGprs
	 GPRS_MIN_MEAN_THROUGHPUT,	//OutgoingGprs, IncomingGprs
	 NO_MORE_RECORDS
	};
#endif  //SYMBIAN_NETWORKING_3GPPDEFAULTQOS

#define CONNECTION_PREFERENCES_S        _S("ConnectionPreferences")
#define GLOBAL_SETTINGS_S   		    _S("GlobalSettings")

/**
linked records on IAP table
@internalComponent
*/
const TText* const LINK_REC_IAPArray[] =
	{
    IAP_LOCATION,
    NO_MORE_RECORDS
	};

/**
linked records on ConnPref table
@internalComponent
*/
const TText* const LINK_REC_CONNECTION_PREFERENCESArray[] =
	{
	//The IAP field is not working as a link in the ConnPref table...
	//leave occures when trying to call StoreL.
	//IAP,
	NO_MORE_RECORDS
	};

/**
linked records on Global Settings table
@internalComponent
*/
const TText* const LINK_REC_GLOBAL_SETTINGSArray[] =
	{
    MODEM_DATA_FAX,
    LOCATION_DATA_FAX,
    LOCATION_PHONE_SERVICES_SMS,
    DEFAULT_NETWORK,
    SMS_BEARER,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    DEFAULT_TIER,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NO_MORE_RECORDS
	};

/**
linked records on AP table
@internalComponent
*/
const TText* const LINK_REC_ACCESS_POINTSArray[] =
	{
	ACCESS_POINT_POLICY,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    //OLD_ACCESS_POINT_POLICY,
    ACCESS_POINT_TIER,
    ACCESS_POINT_MCPR,
    ACCESS_POINT_CPR,
    ACCESS_POINT_SCPR,
    ACCESS_POINT_PROTOCOL,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NO_MORE_RECORDS
	};

/**
linked records on WLANServiceExtension table
@internalComponent
*/
const TText* const LINK_REC_WLANSERVICE_EXTENSIONArray[] =
	{
	WLAN_SEC_DATA,
    NO_MORE_RECORDS
	};

/**
linked records on EAP Security table
@internalComponent
*/
const TText* const LINK_REC_EAP_SEC_TABLEArray[] =
	{
	EAP_SEC_DATA,
    NO_MORE_RECORDS
	};

/**
linked records on Tunnelled EAP Settings table
@internalComponent
*/
const TText* const LINK_REC_TUN_EAP_SETTINGS_TABLEArray[] =
	{
	TUN_EAP_DATA,
    NO_MORE_RECORDS
	};

/**
linked records on WapIPBearer table
@internalComponent
*/
const TText* const LINK_REC_WAPIPBEARER_Array[] =
	{
	WAP_ACCESS_POINT_ID,
	IAP,
    NO_MORE_RECORDS
	};
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
linked records on BearerType table
@internalComponent
*/
const TText* const LINK_REC_BEARERTYPEcolumnArray[] =
	{
	ACCESS_POINT_TIER,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    ACCESS_POINT_MCPR,
    ACCESS_POINT_CPR,
    ACCESS_POINT_SCPR,
    ACCESS_POINT_PROTOCOL,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NO_MORE_RECORDS
	};

/*
linked records on APPrioritySelectionPolicy table
@internalComponent
*/
const TText* const LINK_REC_APPRIORITYSELECTIONPOLICYcolumnArray[] =
    {
    SELECTION_POLICY_AP1,
    SELECTION_POLICY_AP2,
    SELECTION_POLICY_AP3,
    SELECTION_POLICY_AP4,
    SELECTION_POLICY_AP5,
    SELECTION_POLICY_AP6,
    SELECTION_POLICY_AP7,
    SELECTION_POLICY_AP8,
    SELECTION_POLICY_AP9,
    SELECTION_POLICY_AP10,
    SELECTION_POLICY_AP11,
    SELECTION_POLICY_AP12,
    SELECTION_POLICY_AP13,
    SELECTION_POLICY_AP14,
    SELECTION_POLICY_AP15,
    NO_MORE_RECORDS
    };

/**
linked records on Tier table
@internalComponent
*/
const TText* const LINK_REC_TIERcolumnArray [] = 
    {
    DEFAULT_ACCESS_POINT,
    NO_MORE_RECORDS
    };
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

const TText* const LINK_REC_VPNSERVICEcolumnArray [] = 
    {
    KCDTypeNameVPNSNAPRecord,
    NO_MORE_RECORDS
    };
	
/**
This enum is used as index for LinkRecords Arrays
@internalComponent
*/
enum LinkedRecordsTables
	{
	ELRIAP,
	ELRConnectionPreference,
	ELRGlobalSettings,
	ELRAccessPoints,
	ELRWLANServiceExt,
	ELREAPSec,
	ELRTunEapSettings,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    ELRBearerType,
    ELRAPPrioritySel,
    ELRTier,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    ELRVPNService,
    ELRWAPIPBearer
	};
	
/**
list of all Tables in CommDB that have linked records
@internalComponent
*/
const TText* const TablesWithLinkRecords[] =
	{
    IAP,
	CONNECTION_PREFERENCES_S,
    GLOBAL_SETTINGS_S,
	ACCESS_POINT_TABLE,
	WLAN_SERVICE_EXTENSIONS,
	EAP_SEC_TABLE,
	TUN_EAP_TABLE,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    BEARER_TYPE_TABLE,
    AP_PRIORITY_SELECTION_POLICY_TABLE,
    TIER_TABLE,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    VPN_SERVICE,
    WAP_IP_BEARER,
	NO_MORE_RECORDS
	};
/**
This is index of all table arrays with linked records
@internalComponent
*/
const TText** const LinkRecordsArray[] =
	{
	(const TText** const)LINK_REC_IAPArray,
	(const TText** const)LINK_REC_CONNECTION_PREFERENCESArray,
	(const TText** const)LINK_REC_GLOBAL_SETTINGSArray,
	(const TText** const)LINK_REC_ACCESS_POINTSArray,
	(const TText** const)LINK_REC_WLANSERVICE_EXTENSIONArray,
	(const TText** const)LINK_REC_EAP_SEC_TABLEArray,
	(const TText** const)LINK_REC_TUN_EAP_SETTINGS_TABLEArray,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    (const TText** const)LINK_REC_BEARERTYPEcolumnArray,
    (const TText** const)LINK_REC_APPRIORITYSELECTIONPOLICYcolumnArray,
    (const TText** const)LINK_REC_TIERcolumnArray,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    (const TText** const)LINK_REC_VPNSERVICEcolumnArray,
    (const TText** const)LINK_REC_WAPIPBEARER_Array
	};


/**
This enum is used as index for LinkRecords Arrays in the case
of xml processing...
@internalComponent
*/
const TText* const LinkedRecordTableNames[] =
	{
	_S("IAPTable"),
	_S("ConnectionPreferencesTable"),
	_S("GlobalSettingsTable"),
	_S("AccessPointTable"),
	_S("WLANServiceExtensionTable"),
    _S("EAPSecuritySettingsTable"),
    _S("TunnelledEAPSettingsTable"),
     KCDTypeNameVPNSNAPRecord
	};

const TInt LinkedRecordTableNum = 8;


const TText* const LegacyLinkFields[] =
	{
	IAP_SERVICE_TYPE,
	IAP_BEARER_TYPE,
	LAN_SERVICE_EXTENSION_TABLE_NAME,
	NO_MORE_RECORDS
	};

#endif
// DBDEF_H
