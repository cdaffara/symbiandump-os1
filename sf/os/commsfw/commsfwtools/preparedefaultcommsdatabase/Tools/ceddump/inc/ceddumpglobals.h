// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the global definitions needed across the ceddump module/exe
// 
//

/**
 @file
 @internalComponent
*/


#ifndef __CEDDUMPGLOBALS_H__
#define __CEDDUMPGLOBALS_H__

#include <metadatabase.h>
#include <commsdattypesv1_1.h>
#include <commsdat.h>

using namespace CommsDat;

#if defined(_UNICODE)
#define _I(x) L ## x
#else
#define _I(x) x
#endif

/*
#define MAX_COL_NAME_LEN        64

#define 	CONNECTION_PREFERENCES				_S("ConnectionPreferences")
#define 	IAP									_S("IAP")
#define 	GLOBAL_SETTINGS 					_S("GlobalSettings")
#define 	NETWORK 							_S("Network")
#define 	LOCATION 							_S("Location")
#define 	WAP_ACCESS_POINT 					_S("WAPAccessPoint")
#define		DIAL_OUT_ISP						_S("DialOutISP")
#define		DIAL_IN_ISP							_S("DialInISP")	
#define		LAN_SERVICE							_S("LANService")
#define		WLAN_SERVICE_EXTENSION_TABLE 		_S("WLANServiceExtensionTable")
#define		VPN_SERVICE 						_S("VpnService")
#define		OUTGOING_GPRS 						_S("OutgoingGPRS")
#define		INCOMING_GPRS 						_S("IncomingGPRS")
#define		DEFAULT_GPRS 						_S("DefaultGPRS")
#define		MODEM_BEARER 						_S("ModemBearer")
#define		LAN_BEARER 							_S("LANBearer")
#define		VIRTUAL_BEARER 						_S("VirtualBearer")
#define		WAP_IP_BEARER 						_S("WAPIPBearer")
#define		WAP_SMS_BEARER 						_S("WAPSMSBearer")
#define		CHARGECARD 							_S("Chargecard")
#define		PROXIES 							_S("Proxies")
#define		SS_PROTO_TABLE 						_S("SecureSocketTable")
#define		AGENT_LOOKUP 						_S("AgentLookup")
#define 	NO_MORE_RECORDS						_S("NO_MORE_RECORDS")

#define Hidden 			_S("Hidden=Yes")
#define Private 		_S("Private=Yes")
#define ProtectedWrite 	_S("ProtectedWrite=Yes")
#define ProtectedRead 	_S("ProtectedRead=Yes")

//_LIT(Comma, 			", ");

*/

typedef TUint32 TAttributeFlags;

enum EAttributes
	{
	EHidden=0,
	EPrivate,
	EProtectedRead,
	EProtectedWrite
	};
#endif

