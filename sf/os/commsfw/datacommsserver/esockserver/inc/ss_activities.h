// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_SS_ACTIVITIES_H
#define SYMBIAN_SS_ACTIVITIES_H

#include <elements/mm_activities.h> 

namespace ESock
{

/**
activity ID is 16bit number, range<0,0xFFFF>, and needs to be unique for a node only.
but an activity ID used to define activity in static activity map is only 8 bits
range <0,255>.  Note internal esock activities use some of this range.
*/
enum TCFNodeActivityId
	{
	/**
	Core activity IDs
	*/
	ECFActivityNoBearer				    = MeshMachine::KActivityNull + 1,
	ECFActivitySelect				    = 2,
	ECFActivityBinderRequest			= 3,
    ECFActivityBindTo       			= 4,
	ECFActivityClientJoin	    		= 5,
	ECFActivityClientLeave			    = 6,
	ECFActivityStart				    = 7,
	ECFActivityStartDataClient          = 8,
	ECFActivityStop			            = 9,
	ECFActivityStopDataClient			= 10,
	ECFActivityGoneDown      			= 11,
	ECFActivityDataClientGoneDown		= 12,
	ECFActivityDestroy				    = 13,

	ECFActivityAvailabilityNotification	= 14,

	ECFActivityTierNotification			= 15,

	ECFActivityMonitorProviderStatus	= 16,
	ECFActivityReportProviderStatus		= 17,

	ECFActivityTierGetDataCollectors	= 18,

	ECFActivityStoreProvision			= 19, 
	ECFActivityDataClientActive      	= 20, 
	ECFActivityDataClientIdle		    = 21, 
	ECFActivityForwardStateChange		= 22, 
	ECFActivityError					= 23, 	// if found in node activity map, is run by the meshmachine
												// whenever an error is received and not processed by
												// its target activity
	ECFActivityErrorRecovery            = 24,	// responds to TErrorRecoveryReq

	ECFActivityGoneUp                   = 25,
	ECFActivityAny			    = 26,
	ECFActivityClientLeft       = 27,

	//spare = 28,
	//spare = 29,
	//spare = 30,
	//spare = 31,
	
	/**
	Other activity IDs (could be reuse when other IDs exhausted)
	*/
	ECFActivityNotification				= 32,
	ECFActivityRejoin                   = 33,
	ECFActivityParamRequest             = 34,
	ECFActivityApplyChanges             = 35,
	
	ECFActivityConnectionStateChangeRequest = 36,
	ECFActivityStateChange					= 37,	
	ECFActivityConnectionStartRecovery   	= 38,
	ECFActivityConnectionGoneDownRecovery 	= 39,
	
	ECFActivityAvailabilityStart		= 40,
	ECFActivityAvailabilityStop			= 41,
	
	// INSERT NEW ACTIVITIES HERE..
	//  Please, please reflect changes here in the esockmessages.definition.txt file for svg logging

	ECFActivityPolicyCheckRequest		= 42,
	

	/**
	Dont implement more activities than ECFActivityEndOfList!
	*/
	ECFActivityEndOfList = 95,

	ECFActivityCustom                   = 0x007F,
	
	/**
	the range is used for activities that can run more than one instance in parallel
	such activities must be type of or derived from CCFNodeParallerActivityBase (ss_nodeactivity.h)
	*/
	ECFActivityParallelRangeMin         = 0x0100,
	ECFActivityParallelRangeMax         = 0xFF00,
	};

} //namespace ESock


#endif //SYMBIAN_SS_ACTIVITIES_H

