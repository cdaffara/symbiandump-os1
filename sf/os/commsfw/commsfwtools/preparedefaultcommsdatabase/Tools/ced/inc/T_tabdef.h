
// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Table definitions
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TABDEF_H__
#define __TABDEF_H__

#include <e32math.h>
#include <f32file.h>
#include "CDBSTD.H"

/**
@internalComponent
*/
namespace commdb_dll
	{
const TUint KSingleQuoteChar='\'';

/**
Literals to speed up commdb by not having to create TPtr's at runtime 
@internalComponent
*/
CDB_LIT(KGlobalSettings,_GLOBAL_SETTINGS);
CDB_LIT(KConnectionPreferences,_CONNECTION_PREFERENCES);
CDB_LIT(KConnectPrefRanking,_CONNECT_PREF_RANKING);
CDB_LIT(KConnectPrefDirection,_CONNECT_PREF_DIRECTION);
CDB_LIT(KCommDbIntValue, _COMMDB_INT_VALUE);
CDB_LIT(KCommDbTextValue, _COMMDB_TEXT_VALUE);
CDB_LIT(KStartSQLQuery,_SQL_START_SQL_QUERY);
CDB_LIT(KWhereKeyword,_SQL_WHERE_KEYWORD);
CDB_LIT(KAndKeyword,_SQL_AND_KEYWORD);
CDB_LIT(KOrderKeyword,_SQL_ORDER_KEYWORD);
CDB_LIT(KOrderByKeyword, _SQL_ORDER_BY_KEYWORD);
CDB_LIT(KWhereHidden,_SQL_WHERE_HIDDEN);
CDB_LIT(KAndHidden,_SQL_AND_HIDDEN);
CDB_LIT(KWhereIntMatch,_SQL_WHERE_INT_MATCH);
CDB_LIT(KWhereTextMatch,_SQL_WHERE_TEXT_MATCH);
CDB_LIT(KAndIntMatch,_SQL_AND_INT_MATCH) ;
CDB_LIT(KAndTextMatch,_SQL_AND_TEXT_MATCH);
CDB_LIT(KOrTextMatch,_SQL_OR_TEXT_MATCH);
CDB_LIT(KWhereIntGreaterThan,_SQL_WHERE_INT_GREATERTHAN);


/**
Default widths for DB columns
@internalComponent
*/
const TInt KDefaultColumnWidth = KDbUndefinedLength  ;
const TInt KDefaultTextColumnWidth = 50 ;

/**
Non-default DB column widths
*/
const TInt KAUTH_NAME_FIELD_WIDTH = 255 ;
const TInt KAUTH_PASS_FIELD_WIDTH = 255 ;


/**
Specifies the name, type and special attributes of a column in a table.
Used during the creation of a new database.
@internalComponent
*/
struct TColumn
	{
	const TText* iName;
	TDbColType iType;
	TUint iAttributes;
	TInt iColumnWidth ;
	};

/**
Specifies the name and columns of a table.
An array of `TTable` objects is used during the creation of a new database.
@internalComponent
*/
struct TTable
	{
	const TText* iName;
	const TColumn* iTable;
	};

/**
Maps a bearer type and direction to a service table (Dialout,GPRS)
Used by `OpenIAPTableViewMatchingBearerSetLC()`.
@internalComponent
*/
struct TMapBearerToServiceType
	{
	TCommDbBearer				bearerMask;
	TCommDbConnectionDirection	direction;
	const TText*				serviceType;
	};


/**
Table defintions
@internalComponent
*/ 
TColumn const KNetworkTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{HOST_NAME,EDbColText,0, KDefaultTextColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KIAPTable[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{IAP_SERVICE,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{IAP_SERVICE_TYPE, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{IAP_BEARER,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{IAP_BEARER_TYPE,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	
	{IAP_NETWORK,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{IAP_NETWORK_WEIGHTING,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{IAP_LOCATION,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
#ifdef SYMBIAN_NETWORKING_UMTSR5
	{IAP_APPSID,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
#endif //SYMBIAN_NETWORKING_UMTSR5
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KDialOutISPTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{ISP_DESCRIPTION,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_TYPE,EDbColUint32,0, KDefaultColumnWidth},
	//
	{ISP_DEFAULT_TEL_NUM,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_DIAL_RESOLUTION,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_USE_LOGIN_SCRIPT,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_LOGIN_SCRIPT,EDbColLongText,0, KDefaultColumnWidth},
	{ISP_PROMPT_FOR_LOGIN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_LOGIN_NAME,EDbColText,0, KAUTH_NAME_FIELD_WIDTH},
	{ISP_LOGIN_PASS,EDbColText,0, KAUTH_PASS_FIELD_WIDTH},
	{ISP_CHARGECARD,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_DISPLAY_PCT,EDbColBit,0, KDefaultColumnWidth},
	//
	{ISP_IF_PARAMS,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IF_NETWORKS,EDbColText,0, KDefaultTextColumnWidth},
	//
	{ISP_IF_PROMPT_FOR_AUTH,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_IF_AUTH_NAME,EDbColText,0, KAUTH_NAME_FIELD_WIDTH},
	{ISP_IF_AUTH_PASS,EDbColText,0, KAUTH_PASS_FIELD_WIDTH},
	{ISP_IF_AUTH_RETRIES,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_IF_CALLBACK_ENABLED,EDbColBit,0, KDefaultColumnWidth},
	{ISP_IF_CALLBACK_TYPE,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_IF_CALLBACK_INFO,EDbColText8,0, KDefaultTextColumnWidth},
	{ISP_CALLBACK_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	//
	{ISP_IP_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_IP_ADDR,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_NETMASK,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_GATEWAY,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_DNS_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_IP_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	
	{ISP_IP6_DNS_ADDR_FROM_SERVER, EDbColBit,0, KDefaultColumnWidth},
	{ISP_IP6_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP6_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	
	{ISP_IP_ADDR_LEASE_VALID_FROM,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_ADDR_LEASE_VALID_TO,EDbColText,0, KDefaultTextColumnWidth}, 
	//
	{ISP_CONFIG_DAEMON_MANAGER_NAME,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_CONFIG_DAEMON_NAME,EDbColText,0, KDefaultTextColumnWidth},
	//
	{ISP_ENABLE_IP_HEADER_COMP,EDbColBit,0, KDefaultColumnWidth},
	{ISP_ENABLE_LCP_EXTENSIONS,EDbColBit,0, KDefaultColumnWidth},
	{ISP_DISABLE_PLAIN_TEXT_AUTH,EDbColBit,0, KDefaultColumnWidth},
	{ISP_ENABLE_SW_COMP,EDbColBit,0, KDefaultColumnWidth},
	//
	{ISP_BEARER_NAME,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_BEARER_SPEED,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_BEARER_CE,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_INIT_STRING,EDbColText8,0, KDefaultTextColumnWidth},
	//
	{ISP_BEARER_TYPE,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_CHANNEL_CODING,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_AIUR,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_REQUESTED_TIME_SLOTS,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_MAXIMUM_TIME_SLOTS,EDbColUint32,0, KDefaultColumnWidth},
	//
	/**	new multimode columns */
	{ISP_BEARER_SERVICE,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_BEARER_PROTOCOL,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_RLP_VERSION,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_IWF_TO_MS,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_MS_TO_IWF,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_ACK_TIMER,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_RETRANSMISSION_ATTEMPTS,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_RESEQUENCE_PERIOD,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_V42_COMPRESSION,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_V42_CODEWORDS,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_V42_MAX_LENGTH,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_ASYMMETRY,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_USER_INIT_UPGRADE,EDbColBit,0, KDefaultColumnWidth},
	{ISP_USE_EDGE,EDbColBit,0, KDefaultColumnWidth},
	
	{ISP_IF_SERVER_MODE,EDbColBit,0, KDefaultColumnWidth},
	{ISP_SERVICE_ENABLE_LLMNR,EDbColBit,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KDialInISPTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{ISP_DESCRIPTION,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_TYPE,EDbColUint32,0, KDefaultColumnWidth},
	//
	{ISP_USE_LOGIN_SCRIPT,EDbColBit,0, KDefaultColumnWidth},
	{ISP_LOGIN_SCRIPT,EDbColLongText,0, KDefaultColumnWidth},
	//
	{ISP_AUTHENTICATION,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_IF_AUTH_RETRIES,EDbColUint32,0, KDefaultColumnWidth},
	//
	{ISP_IF_PARAMS,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IF_NETWORKS,EDbColText,0, KDefaultTextColumnWidth},
	//
	{ISP_IP_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_IP_ADDR,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_NETMASK,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_GATEWAY,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_DNS_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{ISP_IP_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	
	{ISP_IP6_DNS_ADDR_FROM_SERVER,EDbColBit,0, KDefaultColumnWidth},
	{ISP_IP6_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{ISP_IP6_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	
	//
	{ISP_ENABLE_IP_HEADER_COMP,EDbColBit,0, KDefaultColumnWidth},
	{ISP_ENABLE_LCP_EXTENSIONS,EDbColBit,0, KDefaultColumnWidth},
	{ISP_DISABLE_PLAIN_TEXT_AUTH,EDbColBit,0, KDefaultColumnWidth},
	{ISP_ENABLE_SW_COMP,EDbColBit,0, KDefaultColumnWidth},
	//
	{ISP_BEARER_NAME,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_BEARER_SPEED,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_BEARER_CE,EDbColUint32,0, KDefaultColumnWidth},
	{ISP_INIT_STRING,EDbColText8,0, KDefaultTextColumnWidth},
	{ISP_USE_EDGE,EDbColBit,0, KDefaultColumnWidth},
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KGPRSTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	
	{GPRS_APN,EDbColLongText,0, KDefaultColumnWidth},
	//
	{GPRS_PDP_TYPE,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{GPRS_PDP_ADDRESS,EDbColText,0, KDefaultTextColumnWidth},
	//
	{GPRS_REQ_PRECEDENCE,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_REQ_DELAY,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_REQ_RELIABILITY,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_REQ_PEAK_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_REQ_MEAN_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	//
	{GPRS_MIN_PRECEDENCE,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_MIN_DELAY,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_MIN_RELIABILITY,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_MIN_PEAK_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_MIN_MEAN_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	//
	{GPRS_DATA_COMPRESSION,EDbColBit,0, KDefaultColumnWidth},
	{GPRS_HEADER_COMPRESSION,EDbColBit,0, KDefaultColumnWidth},
	{GPRS_USE_EDGE, EDbColBit, 0, KDefaultColumnWidth},
	//
	{GPRS_ANONYMOUS_ACCESS,EDbColBit,0, KDefaultColumnWidth},
	//
	{GPRS_IF_PARAMS,EDbColText,0, KDefaultTextColumnWidth},
	{GPRS_IF_NETWORKS,EDbColText,0, KDefaultTextColumnWidth},
	//
	{GPRS_IF_PROMPT_FOR_AUTH,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{GPRS_IF_AUTH_NAME,EDbColText,0, KAUTH_NAME_FIELD_WIDTH},
	{GPRS_IF_AUTH_PASS,EDbColText,0, KAUTH_PASS_FIELD_WIDTH},
	//
	{GPRS_IF_AUTH_RETRIES,EDbColUint32,0, KDefaultColumnWidth},
	//
	{GPRS_IP_NETMASK,EDbColText,0, KDefaultTextColumnWidth},
	{GPRS_IP_GATEWAY,EDbColText,0, KDefaultTextColumnWidth},
	{GPRS_IP_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{GPRS_IP_ADDR,EDbColText,0, KDefaultTextColumnWidth},
	{GPRS_IP_DNS_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull,KDefaultColumnWidth},
	{GPRS_IP_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{GPRS_IP_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	//
	{GPRS_IP6_DNS_ADDR_FROM_SERVER,EDbColBit,0, KDefaultColumnWidth},
	{GPRS_IP6_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{GPRS_IP6_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	
	{GPRS_IP_ADDR_LEASE_VALID_FROM,EDbColText,0, KDefaultTextColumnWidth},
	{GPRS_IP_ADDR_LEASE_VALID_TO,EDbColText,0, KDefaultTextColumnWidth},
	//
	{GPRS_CONFIG_DAEMON_MANAGER_NAME,EDbColText,0, KDefaultTextColumnWidth},	
	{GPRS_CONFIG_DAEMON_NAME,EDbColText,0, KDefaultTextColumnWidth},
	//
	{GPRS_ENABLE_LCP_EXTENSIONS,EDbColBit,0, KDefaultColumnWidth},
	{GPRS_DISABLE_PLAIN_TEXT_AUTH,EDbColBit,0, KDefaultColumnWidth},
	//
	{GPRS_AP_TYPE,EDbColUint32,0, KDefaultColumnWidth},
	//
	{GPRS_QOS_WARNING_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{GPRS_SERVICE_ENABLE_LLMNR,EDbColBit,0, KDefaultColumnWidth},

	{QOS_UMTS_R99_AND_ON_TABLE, EDbColUint32, 0, KDefaultColumnWidth},
	
	{GPRS_R5_DATA_COMPRESSION,EDbColUint32, 0, KDefaultColumnWidth},
	{GPRS_R5_HEADER_COMPRESSION,EDbColUint32, 0, KDefaultColumnWidth},
	{GPRS_R4_PACKET_FLOW_IDENTIFIER,EDbColUint32, 0, KDefaultColumnWidth},
	{GPRS_UMTS_GPRS_RELEASE,EDbColUint32, 0, KDefaultColumnWidth},

	{0}
	};


TColumn const KGPRSQoSR99Table[]=	
	{
	{GPRS_QOS_REQ_TRAFFIC_CLASS, EDbColUint32, 0, KDefaultColumnWidth},	
	{GPRS_QOS_MIN_TRAFFIC_CLASS, EDbColUint32, 0, KDefaultColumnWidth},           	
	{GPRS_QOS_REQ_DELIVERY_ORDER, EDbColUint32, 0, KDefaultColumnWidth},			
	{GPRS_QOS_MIN_DELIVERY_ORDER, EDbColUint32, 0, KDefaultColumnWidth},			
	{GPRS_QOS_REQ_DELIVER_ERRONEOUS_SDU, EDbColUint32, 0, KDefaultColumnWidth},	
	{GPRS_QOS_MIN_DELIVER_ERRONEOUS_SDU, EDbColUint32, 0, KDefaultColumnWidth},	
	{GPRS_QOS_REQ_MAX_SDUSIZE, EDbColInt32, 0, KDefaultColumnWidth}, 			
	{GPRS_QOS_MIN_ACCEPTABLE_MAX_SDU_SIZE, EDbColInt32, 0, KDefaultColumnWidth},
	{GPRS_QOS_REQ_MAX_UPLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},     		
	{GPRS_QOS_REQ_MIN_UPLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},			
	{GPRS_QOS_REQ_MAX_DOWNLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},		
	{GPRS_QOS_REQ_MIN_DOWNLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},		
	{GPRS_QOS_REQ_BER, EDbColUint32, 0, KDefaultColumnWidth},						
	{GPRS_QOS_MAX_BER, EDbColUint32, 0, KDefaultColumnWidth},						
	{GPRS_QOS_REQ_SDU_ERROR_RATIO, EDbColUint32, 0, KDefaultColumnWidth},		
	{GPRS_QOS_MAX_SDU_ERROR_RATIO, EDbColUint32, 0, KDefaultColumnWidth},		
	{GPRS_QOS_REQ_TRAFFIC_HANDLING_PRIORITY, EDbColUint32, 0, KDefaultColumnWidth},
	{GPRS_QOS_MIN_TRAFFIC_HANDLING_PRIORITY, EDbColUint32, 0, KDefaultColumnWidth},
	{GPRS_QOS_REQ_TRANSFER_DELAY, EDbColInt32, 0, KDefaultColumnWidth}, 			
	{GPRS_QOS_MAX_TRANSFER_DELAY, EDbColInt32, 0, KDefaultColumnWidth},			
	{GPRS_QOS_REQ_GUARANTEED_UPLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},	
	{GPRS_QOS_MIN_GUARANTEED_UPLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},	
	{GPRS_QOS_REQ_GUARANTEED_DOWNLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},
	{GPRS_QOS_MIN_GUARANTEED_DOWNLINK_RATE, EDbColInt32, 0, KDefaultColumnWidth},
	{GPRS_QOS_SIGNALLING_INDICATION, EDbColBit, 0, KDefaultColumnWidth},		
	{GPRS_QOS_IM_CN_SIGNALLING_INDICATOR, EDbColBit, 0, KDefaultColumnWidth},
	{GPRS_QOS_SOURCE_STATISTICS_DESCRIPTOR, EDbColUint32, 0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KLANServiceTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{LAN_IF_NETWORKS,EDbColText,0, KDefaultTextColumnWidth},
	
	//
	{LAN_IP_NETMASK,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_IP_GATEWAY,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_IP_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{LAN_IP_ADDR,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_IP_DNS_ADDR_FROM_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{LAN_IP_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_IP_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	//
	{LAN_IP6_DNS_ADDR_FROM_SERVER,EDbColBit,0, KDefaultColumnWidth},
	{LAN_IP6_NAME_SERVER1,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_IP6_NAME_SERVER2,EDbColText,0, KDefaultTextColumnWidth},
	
	{LAN_IP_ADDR_LEASE_VALID_FROM,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_IP_ADDR_LEASE_VALID_TO,EDbColText,0, KDefaultTextColumnWidth},
	
	{LAN_CONFIG_DAEMON_MANAGER_NAME,EDbColText,0, KDefaultTextColumnWidth}, 
	{LAN_CONFIG_DAEMON_NAME,EDbColText,0, KDefaultTextColumnWidth}, 
	{LAN_SERVICE_EXTENSION_TABLE_NAME,EDbColText,0, KDefaultTextColumnWidth},	// LanServiceExtensionTableName
	{LAN_SERVICE_EXTENSION_TABLE_RECORD_ID,EDbColUint32,0, KDefaultColumnWidth},	// LanServiceExtensionTableRecordId
	{LAN_SERVICE_ENABLE_LLMNR,EDbColBit,0, KDefaultColumnWidth},
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KPANServiceExtensionsTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},

	//
	{PAN_LOCAL_ROLE,EDbColUint32,TDbCol::ENotNull,KDefaultColumnWidth},
	{PAN_PEER_ROLE,EDbColUint32,TDbCol::ENotNull,KDefaultColumnWidth},
	{PAN_PEER_MAC_ADDRESSES,EDbColText,0,KDefaultTextColumnWidth},
	{PAN_PROMPT_FOR_REMOTE_DEVICES,EDbColBit,TDbCol::ENotNull,KDefaultColumnWidth},
	{PAN_DISABLE_SDP_QUERY,EDbColBit,TDbCol::ENotNull,KDefaultColumnWidth},
	{PAN_ALLOW_INCOMING,EDbColBit,TDbCol::ENotNull,KDefaultColumnWidth},
#ifdef SYMBIAN_NETWORKING_DHCPSERVER
	{PAN_NAP_SERVICE_ENABLED,EDbColUint32,TDbCol::ENotNull,KDefaultColumnWidth},
#endif // SYMBIAN_NETWORKING_DHCPSERVER	
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KModemTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{MODEM_AGENT,EDbColText,0, KDefaultTextColumnWidth},
	{MODEM_NIF_NAME,EDbColText,0, KDefaultTextColumnWidth},
  	{MODEM_BCA_STACK, EDbColText, 0, KDefaultTextColumnWidth},
	{MODEM_PORT_NAME,EDbColText,0, KDefaultTextColumnWidth},
	{MODEM_CONTROL_CHANNEL_PORT_NAME,EDbColText,0, KDefaultTextColumnWidth},
	{MODEM_TSY_NAME,EDbColText,0, KDefaultTextColumnWidth},
	{MODEM_CSY_NAME,EDbColText,0, KDefaultTextColumnWidth},
	{MODEM_DATA_BITS,EDbColUint8,0, KDefaultColumnWidth},
	{MODEM_STOP_BITS,EDbColUint8,0, KDefaultColumnWidth},
	{MODEM_PARITY,EDbColUint8,0, KDefaultColumnWidth},
	{MODEM_RATE,EDbColUint32,0, KDefaultColumnWidth},
	{MODEM_HANDSHAKING,EDbColUint32,0, KDefaultColumnWidth},
	{MODEM_SPECIAL_RATE,EDbColUint32,0, KDefaultColumnWidth},
	{MODEM_XON_CHAR,EDbColUint8,0, KDefaultColumnWidth},
	{MODEM_XOFF_CHAR,EDbColUint8,0, KDefaultColumnWidth},
	//
	{MODEM_FAX_CLASS_PREF,EDbColUint8,0, KDefaultColumnWidth},
	{MODEM_SPEAKER_PREF,EDbColUint8,0, KDefaultColumnWidth},
	{MODEM_SPEAKER_VOL_PREF,EDbColUint8,0, KDefaultColumnWidth},
	//
	{MODEM_MODEM_INIT_STRING,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_DATA_INIT_STRING,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_FAX_INIT_STRING,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_ISP_INIT_STRING,EDbColText8,0, KDefaultTextColumnWidth},
	//
	{MODEM_DIAL_PAUSE_LENGTH,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_CARRIER_TIMEOUT,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_AUTO_ANSWER_RING_COUNT,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_SPEAKER_VOL_CONTROL_LOW,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_SPEAKER_VOL_CONTROL_MEDIUM,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_SPEAKER_VOL_CONTROL_HIGH,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_SPEAKER_ALWAYS_OFF,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_SPEAKER_ON_UNTIL_CARRIER,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_SPEAKER_ALWAYS_ON,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_SPEAKER_ON_AFTER_DIAL_UNTIL_CARRIER,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_DIAL_TONE_WAIT_MODIFIER,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_CALL_PROGRESS_1,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_CALL_PROGRESS_2,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_CALL_PROGRESS_3,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_CALL_PROGRESS_4,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_ECHO_OFF,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_VERBOSE_TEXT,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_QUIET_OFF,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_QUIET_ON,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_DIAL_COMMAND_STATE_MODIFIER,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_ON_LINE,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_RESET_CONFIGURATION,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_RETURN_TO_FACTORY_DEFS,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_DCD_ON_DURING_LINK,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_DTR_HANG_UP,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_DSR_ALWAYS_ON,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_RTS_CTS_HANDSHAKE,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_XON_XOFF_HANDSHAKE,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_ESCAPE_CHARACTER,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_ESCAPE_GUARD_PERIOD,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_FAX_CLASS_INTERROGATE,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_FAX_CLASS,EDbColText8,0, KDefaultTextColumnWidth},
	//
	{MODEM_NO_DIAL_TONE,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_BUSY,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_NO_ANSWER,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_CARRIER,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_CONNECT,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_COMPRESSION_CLASS_5,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_COMPRESSION_V42BIS,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_COMPRESSION_NONE,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_PROTOCOL_LAPD,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_PROTOCOL_ALT,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_PROTOCOL_ALTCELLULAR,EDbColText8,0, KDefaultTextColumnWidth},
	{MODEM_PROTOCOL_NONE,EDbColText8,0, KDefaultTextColumnWidth},
	//
	{MODEM_MESSAGE_CENTRE_NUMBER,EDbColText,0, KDefaultTextColumnWidth},
	{MODEM_MESSAGE_VALIDITY_PERIOD,EDbColUint32,0, KDefaultColumnWidth},
	{MODEM_MESSAGE_DELIVERY_REPORT,EDbColBit,0, KDefaultColumnWidth},
	//
	{MODEM_MIN_SIGNAL_LEVEL,EDbColUint32,0, KDefaultColumnWidth},
	//
	{LAST_SOCKET_ACTIVITY_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{LAST_SESSION_CLOSED_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{LAST_SOCKET_CLOSED_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	//
	{MODEM_SIR_SETTINGS,EDbColUint32,0, KDefaultColumnWidth},
	{MODEM_COMM_ROLE,EDbColUint32,0, KDefaultColumnWidth},
	{BEARER_TECHNOLOGY,EDbColUint32,0, KDefaultColumnWidth},
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KLANBearerTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{LAN_BEARER_AGENT,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{LAN_BEARER_NIF_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{LAN_BEARER_LDD_FILENAME,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_BEARER_LDD_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{LAN_BEARER_PDD_FILENAME,EDbColText,0, KDefaultTextColumnWidth},
	{LAN_BEARER_PDD_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{LAN_BEARER_PACKET_DRIVER_NAME,EDbColText,0, KDefaultTextColumnWidth},
	{LAST_SOCKET_ACTIVITY_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{LAST_SESSION_CLOSED_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{LAST_SOCKET_CLOSED_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{BEARER_TECHNOLOGY,EDbColUint32,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/   
TColumn const KLocationTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{LOCATION_INTL_PREFIX_CODE,EDbColText,0, KDefaultTextColumnWidth},
	{LOCATION_NAT_PREFIX_CODE,EDbColText,0, KDefaultTextColumnWidth},
	{LOCATION_NAT_CODE,EDbColText,0, KDefaultTextColumnWidth},
	{LOCATION_AREA_CODE,EDbColText,0, KDefaultTextColumnWidth},
	{LOCATION_DIAL_OUT_CODE,EDbColText,0, KDefaultTextColumnWidth},
	{LOCATION_DISABLE_CALL_WAITING_CODE,EDbColText,0, KDefaultTextColumnWidth},
	{LOCATION_MOBILE,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{LOCATION_USE_PULSE_DIAL,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{LOCATION_WAIT_FOR_DIAL_TONE,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{LOCATION_PAUSE_AFTER_DIAL_OUT,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KChargecardTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{CHARGECARD_ACCOUNT_NUMBER,EDbColText,0, KDefaultTextColumnWidth},
	{CHARGECARD_PIN,EDbColText,0, KDefaultTextColumnWidth},
	{CHARGECARD_LOCAL_RULE,EDbColText,0, KDefaultTextColumnWidth},
	{CHARGECARD_NAT_RULE,EDbColText,0, KDefaultTextColumnWidth},
	{CHARGECARD_INTL_RULE,EDbColText,0, KDefaultTextColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KProxiesTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{PROXY_ISP,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{PROXY_SERVICE_TYPE,EDbColText,0, KDefaultTextColumnWidth},
	{PROXY_USE_PROXY_SERVER,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{PROXY_SERVER_NAME,EDbColLongText,0, KDefaultColumnWidth},
	{PROXY_PROTOCOL_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{PROXY_PORT_NUMBER,EDbColUint32,0, KDefaultColumnWidth},
	{PROXY_EXCEPTIONS,EDbColLongText,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KWAPAccessPointTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{WAP_CURRENT_BEARER,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{WAP_START_PAGE,EDbColLongText,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KWAPIPBearerTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{WAP_ACCESS_POINT_ID,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{WAP_GATEWAY_ADDRESS,EDbColText,0, KDefaultTextColumnWidth},
	{WAP_IAP,EDbColUint32,0, KDefaultColumnWidth},
	/**	The following two fields are obsoleted for WAP2.0 but maintained for
	WAP1.0. They should be removed in Symbian OS 8.0 */
	{WAP_WSP_OPTION,EDbColUint8,0, KDefaultColumnWidth},
	{WAP_SECURITY,EDbColBit,0, KDefaultColumnWidth},
	//
	{WAP_PROXY_PORT,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{WAP_PROXY_LOGIN_NAME,EDbColText,0, KAUTH_NAME_FIELD_WIDTH},
	{WAP_PROXY_LOGIN_PASS,EDbColText,0, KAUTH_PASS_FIELD_WIDTH},
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KWAPSMSBearerTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{WAP_ACCESS_POINT_ID,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{WAP_GATEWAY_ADDRESS,EDbColText,0, KDefaultTextColumnWidth},
	{WAP_SERVICE_CENTRE_ADDRESS,EDbColText,0, KDefaultTextColumnWidth},
	{WAP_WSP_OPTION,EDbColUint8,0, KDefaultColumnWidth},
	{WAP_SECURITY,EDbColBit,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KConnectionPreferencesTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{CONNECT_PREF_RANKING,EDbColUint32,0, KDefaultColumnWidth},
	{CONNECT_PREF_DIRECTION,EDbColUint32,0, KDefaultColumnWidth},
	{CONNECT_PREF_BEARER_SET,EDbColUint32,0, KDefaultColumnWidth},
	{CONNECT_PREF_DIALOG_PREF,EDbColUint32,0, KDefaultColumnWidth},
	{CONNECT_PREF_IAP,EDbColUint32,0, KDefaultColumnWidth},
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KAgentLookup[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{AGENT_FRIENDLY_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{AGENT_FILENAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KGlobalSettingsTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	//
	{COMMDB_INT_VALUE,EDbColUint32,0, KDefaultColumnWidth},
	{COMMDB_TEXT_VALUE,EDbColText,0, KDefaultTextColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KDefaultGPRSTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{DEFAULT_GPRS_USAGE,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_APN,EDbColText, 0, KDefaultTextColumnWidth},
	//
	{DEFAULT_GPRS_PDP_TYPE,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{DEFAULT_GPRS_PDP_ADDRESS,EDbColText, 0, KDefaultTextColumnWidth},
	//
	{DEFAULT_GPRS_PRECEDENCE,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_DELAY,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_RELIABILITY,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_PEAK_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_MEAN_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	//
	{DEFAULT_GPRS_MIN_PRECEDENCE,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_MIN_DELAY,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_MIN_RELIABILITY,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_MIN_PEAK_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_MIN_MEAN_THROUGHPUT,EDbColUint32,0, KDefaultColumnWidth},
	//
	{DEFAULT_GPRS_DATA_COMPRESSION,EDbColBit,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_HEADER_COMPRESSION,EDbColBit,0, KDefaultColumnWidth},
	{DEFAULT_GPRS_USE_EDGE, EDbColBit, 0, KDefaultColumnWidth},
	//
	{DEFAULT_GPRS_ANONYMOUS_ACCESS,EDbColBit,0, KDefaultColumnWidth},
	//
	{0}
	};

/**
@internalComponent
*/
TColumn const KCBTDeviceTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{BT_DEV_ADD,EDbColText8,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{BT_DEV_DFLT_NAME,EDbColLongText16,0, KDefaultColumnWidth},
	{BT_DEV_FRIENDLY_NAME,EDbColLongText16,0, KDefaultColumnWidth},
	{BT_DEVICE_CLASS,EDbColUint32,0, KDefaultColumnWidth},
	{BT_LINKKEY,EDbColText8,0, KDefaultTextColumnWidth},
	{BT_DEVICE_GLOBAL_SECURITY, EDbColUint32,0, KDefaultColumnWidth},
	{BT_DEV_STATE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	/** specific details concerning the juice of each device */
	{BT_PAGE_SCAN_REP_MODE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{BT_PAGE_SCAN_PERIOD_MODE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{BT_PAGE_SCAN_MODE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{BT_SPARE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{BT_CLK_OFFSET, EDbColUint32, 0, KDefaultColumnWidth}, 
	{BT_COD, EDbColUint32, 0, KDefaultColumnWidth}, 
	{0}
	};

/**
@internalComponent
*/
TColumn const KCBTDefaultServiceTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{BT_DFLT_SER_NAME,EDbColText16,0, KDefaultTextColumnWidth},
	{BT_DFLT_SER_UUID,EDbColText8,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{BT_DFLT_VIRTUAL_PORT,EDbColUint32,TDbCol::ENotNull, KDefaultColumnWidth},
	{BT_DFLT_DEV_ADD,EDbColText8,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{BT_DFLT_SECURITY_LEVEL,EDbColUint8,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KCBTSecurityTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{BT_DEV_ADD,EDbColText8,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{BT_SEC_DEVICE_SECURITY,EDbColUint32,0, KDefaultColumnWidth},
	{BT_SEC_UID,EDbColUint32,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
LOCAL_D TColumn const KAccessTypeTable[]=
	{	
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
		//
	{TABLE_ACC_NAME,EDbColText16,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{TABLE_ACC_TYPE,EDbColUint8,0, KDefaultColumnWidth},
	{0}
	};


/**
@internalComponent
*/
TColumn const KCBTPersistTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{BT_PERSIST_COD,EDbColUint32,0 , KDefaultColumnWidth},
	{BT_PERSIST_PAGE_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_PAGE_SCAN_INTERVAL,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_PAGE_SCAN_WINDOW,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_INQUIRY_SCAN_INTERVAL,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_INQUIRY_SCAN_WINDOW,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_HOLD_MODE_ACTIVITY,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_SCO_FLOW_CONTROL,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_PAGE_SCAN_PERIOD,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_PAGE_SCAN_MODE,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_POWER_SETTINGS,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_SCAN,EDbColUint32,0, KDefaultColumnWidth},
	{BT_PERSIST_LOCAL_NAME,EDbColLongText16,0 , KDefaultColumnWidth},
	{BT_PERSIST_IAC_LAP, EDbColLongText16, 0, KDefaultColumnWidth},
	{BT_PERSIST_STATE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{0}
	};



/**
@internalComponent
*/
TColumn const KSecureSocketTable[]=
	{
	{COMMDB_ID,EDbColUint32,TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_HIDDEN,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY,EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{SS_PROTO_NAME,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{SS_PROTO_LIBRARY,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KVirtualBearerTable[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{VIRTUAL_BEARER_AGENT,EDbColText,TDbCol::ENotNull, KDefaultTextColumnWidth},
	{VIRTUAL_BEARER_NIF,EDbColText,TDbCol::ENotNull, KDefaultColumnWidth},
	{LAST_SOCKET_ACTIVITY_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{LAST_SESSION_CLOSED_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},
	{LAST_SOCKET_CLOSED_TIMEOUT,EDbColUint32,0, KDefaultColumnWidth},	
	{BEARER_TECHNOLOGY,EDbColUint32,0, KDefaultColumnWidth},	
	{0}
	};

/**
@internalComponent
*/
TColumn const KVpnServiceTable[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{VPN_SERVICE_POLICY,EDbColText,TDbCol::ENotNull, KDefaultColumnWidth},
	{VPN_SERVICE_IAP,EDbColUint32,0, KDefaultColumnWidth},
	{VPN_SERVICE_NETWORKID,EDbColUint32,0, KDefaultColumnWidth},
	{VPN_SERVICE_ENABLE_LLMNR,EDbColBit,0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KWlanServiceExtensionTable[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{WLAN_PROFILE,  EDbColText ,  0,  KDefaultColumnWidth},  
	{WLAN_SSID, EDbColText , 0, KDefaultColumnWidth}, 
	{WLAN_ENCRYPTION_STATUS, EDbColBit, 0, KDefaultColumnWidth}, 
	{WLAN_ENCRYPTION_TYPE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_DEFAULT_WEP_KEY, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_ENCRYPT_KEY1_VALID, EDbColBit, 0, KDefaultColumnWidth}, 
	{WLAN_WEP_Key1, EDbColText, 0, KDefaultColumnWidth}, 
	{WLAN_ENCRYPT_KEY2_VALID, EDbColBit, 0, KDefaultColumnWidth}, 
	{WLAN_WEP_KEY2, EDbColText, 0, KDefaultColumnWidth}, 
	{WLAN_ENCRYPT_KEY3_VALID, EDbColBit, 0, KDefaultColumnWidth}, 
	{WLAN_WEP_KEY3, EDbColText, 0, KDefaultColumnWidth}, 
	{WLAN_ENCRYPT_KEY4_VALID, EDbColBit, 0, KDefaultColumnWidth}, 
	{WLAN_WEP_KEY4, EDbColText, 0, KDefaultColumnWidth}, 
	{WLANRANKING, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_DIALOG_PREF, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_NETWORK_TYPE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_FRAG_THRES, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_RTS_THRES, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_DES_TRANS_RATE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_BEACON_INT, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_CHANNEL_ID, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_AUTHENTICATION_MODE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_POWERSAVE_MODE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_ALLOW_UNENCRYPT_MODE, EDbColBit, 0, KDefaultColumnWidth}, 
	{WLAN_ALLOW_SHORT_PRE, EDbColBit, 0, KDefaultColumnWidth}, 
	{WLAN_PREAMBLE_TYPE, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_TX_POWER_LEVEL, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_EAP_PRESHARED_KEY, EDbColText, 0, KAUTH_PASS_FIELD_WIDTH}, 
	{WLAN_REG_DOMAIN, EDbColUint32, 0, KDefaultColumnWidth}, 
	{WLAN_SEC_DATA, EDbColText,  0,  KDefaultColumnWidth}, 
	{0}
	};

/**
@internalComponent
*/
TColumn const KEAPSecRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{EAP_SEC_OUTER_EAP_TYPE, EDbColUint32, 0, KDefaultColumnWidth},
	{EAP_SEC_ID, EDbColText, 0, KDefaultColumnWidth},
	{EAP_SEC_PASSWORD, EDbColText, 0, KDefaultColumnWidth},
	{EAP_SEC_CONFIGID,  EDbColUint32, 0, KDefaultColumnWidth},	
	{EAP_SEC_DATA, EDbColText, 0, KDefaultColumnWidth},
    {EAP_SEC_VENDORID,  EDbColText, 0, KDefaultColumnWidth},
    {EAP_SEC_VENDORTYPE,  EDbColUint32, 0, KDefaultColumnWidth},
	{0}
	};
	
/**
@internalComponent
*/
TColumn const KTunEAPRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{TUN_EAP_INNERTYPE, EDbColUint32, 0, KDefaultColumnWidth},
	{TUN_EAP_DATA, EDbColText, 0, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KEAPTLSRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{EAP_TLS_CLIENT_CERT_NAME, EDbColText, TDbCol::ENotNull, KDefaultColumnWidth},
	{0}
	};
	
/**
@internalComponent
*/
TColumn const KLEAPRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{EAP_LEAP_TIMEOUT,  EDbColUint32, 0, KDefaultColumnWidth},	
	{0}
	};


/**
@internalComponent
*/
TColumn const KEapSimRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{EAPSIM_PROTOCOL_ID, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{EAPSIM_PROTOCOL_PSEUDONYM, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{EAPSIM_PROTOCOL_USE_NAI_REALM, EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{EAPSIM_PROTOCOL_NAI_REALM, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{EAPSIM_PROTOCOL_MIN_RANDS, EDbColUint8,0, KDefaultColumnWidth},
	{0}
	};


/**
@internalComponent
*/
TColumn const KEapAkaRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	{EAPAKA_PROTOCOL_ID, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{EAPAKA_PROTOCOL_PSEUDONYM, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{EAPAKA_PROTOCOL_USE_NAI_REALM, EDbColBit,TDbCol::ENotNull, KDefaultColumnWidth},
	{EAPAKA_PROTOCOL_NAI_REALM, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{0}
	};


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
@internalComponent
*/
TColumn const KPolicySelectorRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	/** Columns of the `PolicySelector Table` */
	{POLICYID, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{APPUID, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{SRCADDRESS, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{SRCMASK, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{DSTADDRESS, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{DSTMASK, EDbColText8, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{SRCPORT, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DSTPORT, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{SRCPORTMAX, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DSTPORTMAX, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{PROTOCOLID, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{IAPID, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{PRIORITY, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{0}
	};

TColumn const KPolicySelector2QoSParametersRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	/** Columns of the `PolicySelector2Params Table` */
	{POLICYSELECTORID, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{PARAMSID, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{0}
	};

TColumn const KGenericQoSRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	/** Columns of the `GenericQos Table` */
	{DOWNLINKBANDWIDTH, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{UPLINKBANDWIDTH, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DOWNLINKMAXIMUMBURSTSIZE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{UPLINKMAXIMUMBURSTSIZE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DOWNLINKAVERAGEPACKETSIZE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{UPLINKAVERAGEPACKETSIZE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DOWNLINKMAXIMUMPACKETSIZE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{UPLINKMAXIMUMPACKETSIZE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DOWNLINKDELAY, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{UPLINKDELAY, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DOWNLINKDELAYVARIATION, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{UPLINKDELAYVARIATION, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{DOWNLINKPRIORITY, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{UPLINKPRIORITY, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{HEADERMODE, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	//{QOSNAME, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{0}
	};

/**
@internalComponent
*/
TColumn const KWifiScanEngineRecord[]=
	{
	{COMMDB_ID, EDbColUint32, TDbCol::EAutoIncrement|TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_NAME, EDbColText, TDbCol::ENotNull, KDefaultTextColumnWidth},
	{COMMDB_HIDDEN,EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	{COMMDB_READ_ONLY, EDbColBit, TDbCol::ENotNull, KDefaultColumnWidth},
	//
	/** Columns of the `WifiScanEngine Table` */
	{SCANPERIODMS, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{RSSIMIN, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{RSSIMAX, EDbColUint32, TDbCol::ENotNull, KDefaultColumnWidth},
	{0}
	};

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

	
/**
Array of Tables
@internalComponent
*/
TTable const KTableList[]=
	{
	{NETWORK, KNetworkTable},
	{IAP, KIAPTable},
	{DIAL_OUT_ISP, KDialOutISPTable},
	{DIAL_IN_ISP, KDialInISPTable},
	{WAP_ACCESS_POINT, KWAPAccessPointTable},
	{WAP_IP_BEARER, KWAPIPBearerTable},
	{WAP_SMS_BEARER, KWAPSMSBearerTable},
	{OUTGOING_GPRS, KGPRSTable},
	{INCOMING_GPRS, KGPRSTable},
	{LAN_SERVICE,KLANServiceTable},
	{PAN_SERVICE_EXTENSIONS,KPANServiceExtensionsTable},
	{MODEM_BEARER, KModemTable},
	{LAN_BEARER, KLANBearerTable},
	{LOCATION, KLocationTable},
	{CHARGECARD, KChargecardTable},
	{PROXIES, KProxiesTable},
	{CONNECTION_PREFERENCES, KConnectionPreferencesTable},
	{GLOBAL_SETTINGS, KGlobalSettingsTable},
	{BT_DEVICE_TABLE, KCBTDeviceTable},
	{BT_DEFAULT_TABLE, KCBTDefaultServiceTable},
	{DEFAULT_GPRS, KDefaultGPRSTable},
	{BT_SECURITY_TABLE, KCBTSecurityTable},
	{BT_PERSIST_TABLE, KCBTPersistTable}, 
	{AGENT_LOOKUP,KAgentLookup},
	{SS_PROTO_TABLE,KSecureSocketTable},
	{ACCESS_TYPE_TABLE, KAccessTypeTable},
	{VIRTUAL_BEARER, KVirtualBearerTable},
	{VPN_SERVICE, KVpnServiceTable},
	{WLAN_SERVICE_EXTENSION_TABLE, KWlanServiceExtensionTable},
	{QOS_UMTS_R99_AND_ON_TABLE, KGPRSQoSR99Table},
	{EAP_SEC_TABLE,	KEAPSecRecord},	
	{TUN_EAP_TABLE,	KTunEAPRecord},
	{EAP_TLS_TABLE,	KEAPTLSRecord},	
	{LEAP_TABLE, KLEAPRecord},
	{EAPSIM_PROTOCOL_TABLE, KEapSimRecord},
	{EAPAKA_PROTOCOL_TABLE, KEapAkaRecord},
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	{POLICY_SELECTOR_TABLE, KPolicySelectorRecord},
	{POLICYSELECTOR2QOSPARAMETERS_TABLE, KPolicySelector2QoSParametersRecord},
	{GENERIC_QOS_TABLE, KGenericQoSRecord},
	{WIFI_SCANENGINE_TABLE, KWifiScanEngineRecord},
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	{0}
	};

/**
@internalComponent
*/
const TInt KNumServiceType = 12;
const TMapBearerToServiceType KMap[KNumServiceType] =
	{
	{KCommDbBearerCSD,		ECommDbConnectionDirectionOutgoing, DIAL_OUT_ISP},
	{KCommDbBearerCSD,		ECommDbConnectionDirectionIncoming, DIAL_IN_ISP},
	{KCommDbBearerWcdma,	ECommDbConnectionDirectionOutgoing, OUTGOING_GPRS},
	/** The following two mappings are only required for tornado */
	{KCommDbBearerWcdma,	ECommDbConnectionDirectionIncoming, INCOMING_GPRS},
	{KCommDbBearerLAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
	{KCommDbBearerLAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE},
	{KCommDbBearerVirtual,		ECommDbConnectionDirectionIncoming,	VPN_SERVICE},
	{KCommDbBearerVirtual,		ECommDbConnectionDirectionOutgoing,	VPN_SERVICE},
	{KCommDbBearerPAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
	{KCommDbBearerPAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE},
	};


}
using namespace commdb_dll;


#endif
