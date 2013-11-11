/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file WapStackSuiteDefs.h
*/

#include <es_wsms.h>
#include <wapmsgerr.h>

// Although Publish and Subscribe replaces System Agent, 
// the UIDs we need are still defined by System Agent
#include <sacls.h>

_LIT(KWapCommonSec, "COMMON");

_LIT(KWapBearer,"Bearer");
_LIT(KWapRemoteHost,"Remote_Host");
_LIT(KWapRemotePort,"Remote_Port");
_LIT(KWapLocalHost,"Local_Host");
_LIT(KWapLocalPort,"Local_Port");
_LIT(KLocalPort,"LocalPort");
_LIT(KRemotePort,"RemotePort");
_LIT(KWapExError, "ExError");
_LIT(KWapExError2, "ExError2");
_LIT(KWapReconnect, "Reconnect");
_LIT(KWapSecure, "Secure");
_LIT(KWapRepeat, "Repeat");
_LIT(KWapData, "Data");
_LIT(KWapDataType, "Type");
_LIT(KWapUseSIMTSY, "SimTsy");
_LIT(KWapDetailedLogging, "DetailedLogging");
_LIT(KWapDelayPortClosure, "DelayPortClosure");
_LIT(KWapSIMTSYState, "TsyState");
_LIT(KWapTimeOut, "TimeOut");
_LIT(KWapWdpBufLength, "WDPBufLength");
_LIT(KAwaitLengthCancel, "AwaitLengthCancel");
_LIT(KMiddleCancel, "MiddleCancel");
_LIT(KMultiReadCancel, "MultiReadCancel");
_LIT(KAwaitLengthTwice, "AwaitLengthTwice");
// WSP 
_LIT(KWapWSPMethod, "WSPMethod");
_LIT(KWapWSPHeaders, "WSPHeaders");
_LIT(KWapWSPBody, "WSPBody");
_LIT(KWapURI, "WSPURI");
_LIT(KWapWSPRecvHeaderLength, "WSPHeaderLength");
_LIT(KWapWSPRecvBodyLength, "WSPBodyLength");

_LIT(KWapWDPStackSuiteLogFile,"WapWDPLog");
_LIT(KWapPushStackSuiteLogFile,"WapPushLog");
_LIT(KWapWSPStackSuiteLogFile,"WapWSPLog");


_LIT(KWapStartC32,"StartC32");

// For test step panics
_LIT(KWapStackSuitePanic,"WapStackSuite");
enum TWapSuitePanicCodes{EDataCorrupt = 1};

// Port for the echo server
// Could be read from the ini file
const TInt KEchoPort = 7;

//////////////////////////////////////////////////////////////////////////////
// Wap Address / Port Settings
//////////////////////////////////////////////////////////////////////////////
/*
	EWapPortUnspecified = -1,  // Unspecified
	EWapPortWsp         = 9200,  // Connectionless session protocol
	EWapPortWspWtp      = 9201,  // Connection oriented session protocol
	EWapPortWspWtls     = 9202,  // Secure connectionless session protocol
	EWapPortWspWtpWtls  = 9203,  // Secure connection oriented session protocol
	EWapPortVCard       = 9204,  // vCard
	EWapPortVCardWtls   = 9206,  // Secure vCard
	EWapPortVCal        = 9205,  // vCal
	EWapPortVCalWtls    = 9207   // Secure vCal
	};
*/


