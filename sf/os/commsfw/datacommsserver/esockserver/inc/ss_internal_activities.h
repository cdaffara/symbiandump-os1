/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
* 
*
*/



/**
 @file ss_activities.h
 @internalTechnology
*/

#ifndef SYMBIAN_SS_INTERNAL_ACTIVITIES_H
#define SYMBIAN_SS_INTERNAL_ACTIVITIES_H

#include <elements/nm_node.h>
#include <comms-infras/ss_common.h>
#include <comms-infras/ss_activities.h>

namespace ESock
{

/* message ID is a 16bit number, range<0,0xFFFF> see Messages::TSignatureBase
*/
// activity ID is 16bit number, range<0,0xFFFF>, and needs to be unique for a node only
// but an activity ID used to define activity in static activity map is only 8 bits
// range <0,255>.  

// All publishedPartner activity IDs belong in ss_activities.h 
// Activity IDs in the range ECFActivityEndOfList - ECFActivityCustom
// may be used on any nodes and are internal to esock

// When allocating numbers for activities please avoid common esock numbers that overlap 
// with the same number space in logging output see usage in ss_conn.cpp see sockmes.h for enum's
//	ECNStart = 67,
//	ECNStop = 69,
//	ECNAttach = 84,
//	ECNWaitForIncoming		  = 1011, // = 0x3f3
//	ECNProgressNotification = 71,
//	ECNClose = 65,
//	ESCPSStop = 87,

enum TCFInternalActivityId
	{
	
	// internal esock activities range is 96 - 126

	// core activity IDs could be used on any node
	ECFActivityBuildStack				= ECFActivityEndOfList + 1,
	ECFActivityProviderStatusChange 	= 97,
	ECFActivityDataClientStatusChange           = 98,
	
	// Other activity IDs (could be reuse when other IDs exhausted)
	//CConnection / CPR client specific activities (could be reuse when other IDs exhausted)
	ECFActivityConnectionAttach             = 99,
	ECFActivityConnectionWaitForIncoming    = 100,
	ECFActivityConnSubConnEvents			= 101,
	ECFActivityConnectionAllInterfaceNotification 	= 102,
	ECFActivityLegacyConnEnumResponse		= 103,
	ECFActivityConnectionEnumerateConnections		= 104,
	ECFActivityStopSCPR 					= 105,
	ECFActivityLegacyRMessage2Handler       = 106,
	ECFActivityConnectionLegacyRMessage2Handler       = 107,
	ECFActivitySubConnectionLegacyRMessage2Handler       = 108,

	ECFActivityReportProviderStatusDeactivationWatcher = 109,

	//CFlowRequest specific activities (could be reuse when other IDs exhausted)
	ECFActivityImplicitFlow				= 112,
	ECFActivityConnectionFlow			= 113,
	ECFActivitySubConnectionFlow		= 114,
	ECFActivityDestroyOrphans           = 115,

	//CSubConnection / SCPR specific activities (could be reuse when other IDs exhausted)
	ECFActivityGetParamLength			= 116,
	ECFActivityGetParam					= 117,
	ECFActivitySubConnectionCreate 		= 118,
	// spare = 119,

	ECFActivityDataClientJoin			= 122, //TODO REMOVE ME used in umts QoSIPSCprJoinActivity that needs major work
	ECFActivityBuildLowerLayer          = 123, // TODO removed activity 
	ECFActivityReBindLayer              = 124, // TODO removed activity
	ECFActivityReConnect			    = 125, //TODO removed activity
	
	// IF IN DOUBT INSERT NEW INDIVIDUAL ACTIVITIES HERE..
	//  Please, please reflect changes here in the esockmessages.definition.txt file for svg logging

	
	// Dont implement more activities than ECFActivityEndOfInternalList!
	ECFActivityEndOfInternalList 		= 126,
	// ECFActivityCustom				= 0x007F,

	};

} //namespace ESock

#endif //SYMBIAN_SS_INTERNAL_ACTIVITIES_H

