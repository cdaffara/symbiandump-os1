/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@file
@internalComponent
@released
*/

#include "lbsmessageenums.h"

#ifndef LBSLOCMONITORSERVERDATA_H
#define LBSLOCMONITORSERVERDATA_H

/**
 * Define the file and process name of the Location Monitor
 */
_LIT(KLocMonitorExeName, "\\sys\\bin\\locationmonitor.exe");
_LIT(KLocMonitorServerName, "locationmonitor.exe");

/* Current version number of the Network Request Handler Server.
*/
const TInt8 KLbsLocMonitorMajorVersionNumber 	= 1;
const TInt8 KLbsLocMonitorMinorVersionNumber 	= 0;
const TInt16 KLbsLocMonitorBuildVersionNumber	= 0;


// IDs for IPC messages for Location Monitor server sessions and subsessions
// (these are in addition to the IDs used by the LBS Server Framework that 
// are defined in lbsmessageenums.h)
//
// As the Location Monitor uses the Server Framework, the opcodes for its sessions
// must be in the range [(ESecureSubSessionBaseClose + 1) - (ELastSessionMessageId-1)]
// and opcodes for subsessions must be greater than ELastSessionMessageId 
// (ELastSessionMessageId = KMaxInt32/2 = Decimal 16383 = 0x3FFFFFFF).
// 
enum ELbsLocMonitorMsgEnums
	{
	
	// Start of IDs for IPC messages for sessions of the Location Monitor 
	// 
	ELocMonitorSessionFirstMsgId =		ESecureSubSessionBaseClose + 1,

	EWipeOutDb =		ELocMonitorSessionFirstMsgId,
	ECancelDbWipeOut =	ELocMonitorSessionFirstMsgId + 1,
	
	// Start of IDs for IPC messages for subsessions of the Location Monitor
	// of the type RLbsLocMonitorAreaPositioner.
	//
	// Any new RLbsLocMonitorAreaPositioner message IDs should be added between 
	// EAreaPositionerFirstMsgId and 
	// EAreaPositionerLastMsgId.
	EAreaPositionerFirstMsgId =			ELastSessionMessageId + 1,
	
	ESetLastKnownPosition = 			EAreaPositionerFirstMsgId,
	EGetLastKnownPosition =			 	EAreaPositionerFirstMsgId + 1,
	EGetLastKnownPositionArea =		 	EAreaPositionerFirstMsgId + 2,
	ECancelGetLastKnownPosition =	 	EAreaPositionerFirstMsgId + 3,
	ECancelGetLastKnownPositionArea =	EAreaPositionerFirstMsgId + 4,
	
	EConvertSingleLocationInfoBufferSize = EAreaPositionerFirstMsgId + 5,
    EGetSingleLocationInfo = EAreaPositionerFirstMsgId + 6,
    EConvertMultipleLocationInfoBufferSize = EAreaPositionerFirstMsgId + 7,
    EGetMultipleLocationInfo = EAreaPositionerFirstMsgId + 8,
    ECancelConvertLocationInfo = EAreaPositionerFirstMsgId + 9,
	
	// Add here any new message IDs for subsession type AreaPositioner
	EAreaPositionerLastMessageId,

	// Add below message IDs for new subsession types
	};

/* Enum that defines the types of subsession
   supported by the Location Monitor.
   
   One of these is passed in as the first
   RMessage2 argument when creating a sub-session.
*/
enum TLbsLocMonitorSubSessionType
	{
	ELocMonitorSubSessionTypeUnknown = 0,
	ELocMonitorSubSessionTypeAreaPositioner,
	ELocMonitorSubSessionTypeConversionPositiner,
	ELocMonitorSubSessionTypeLocInfoConverter,
	};

/* Initialisation data sent to the Location Monitor server
   when creating a new Area Positioner subsession.
*/
class TLbsLocMonitorAreaPositionerData
	{
public:
	TInt iUnused; /* Reserved for future use */
	};

/* Initialisation data sent to the Location Monitor server
   when creating a new conversion Positioner subsession.
*/
class TLbsLocMonitorConversionPositionerData
    {
public:
    TInt iUnused; /* Reserved for future use */
    };

/*
	Parameters sent from client to server in a 
	GetLastKnownPositionArea request
*/
class TPosAreaReqParams
	{
public:
	TUint32 iPositionAreaType;
	};	
	

#endif // LBSLOCMONITORSERVERDATA_H

