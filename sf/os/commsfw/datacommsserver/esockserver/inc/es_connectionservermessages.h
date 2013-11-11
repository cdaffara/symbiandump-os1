// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released since 399
*/

#if !defined(__ES_CONNECTIONSERVERMESSAGES_H__)
#define __ES_CONNECTIONSERVERMESSAGES_H__

#include <e32def.h>

/**
Socket server messages
@internalComponent
*/
enum TConnectionServerMessage
	{
    //When editing this list, please do not forget to update
    //the associated policy file  (ss_secpol.cpp)
    // .. and the logging file (ss_log.cpp)

	// Connection server messages
	ECMCreate 				= 0x00,
	ECMClose				= 0x01,
	ECMAttachToTierManager	= 0x02,

	// Access Point Status Query
	
	ECMAccessPointStatusQuery_DoThenGetResultOrSize	= 0x11,
	// args: 1- query bundle buffer, 2- result bundle buffer, 3- ptr to size

	ECMAccessPointStatusQuery_GetResult				= 0x12,
	// args: 1- result bundle buffer

	ECMAccessPointStatusQuery_Cancel				= 0x13,
	// no args


	// Access Point Notification

	ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize	= 0x21,
	// args: 1- setup bundle buffer, 2- result bundle buffer, 3- ptr to size

	ECMAccessPointNotification_AwaitThenGetResultOrSize				= 0x22,
	// args: 1- ignore,              2- result bundle buffer, 3- ptr to size

	ECMAccessPointNotification_GetResult							= 0x23,
	// args: 1- ignore,              2- result bundle buffer

	ECMAccessPointNotification_Cancel 								= 0x24,
	// no args

    //When editing this list, please do not forget to update
    //the associated policy file  (ss_secpol.cpp)
    // .. and the logging file (ss_log.cpp)

	ECMApiExtBindIface					= 0x31,
	ECMApiExtIfaceSendReceive			= 0x32,
	ECMApiExtIfaceSend					= 0x33,
	ECMApiExtIfaceClose					= 0x34,
	

    };


#endif // __ES_CONNECTIONSERVERMESSAGES_H__

