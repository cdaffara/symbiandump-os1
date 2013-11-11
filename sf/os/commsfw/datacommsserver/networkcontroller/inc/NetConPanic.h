/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Network Controller panic codes
* 
*
*/



/**
 @file NetConPanic.h
 @internalComponent
*/

#ifndef __NETCONPANIC_H__
#define __NETCONPANIC_H__

#include <e32def.h>

namespace NetworkController
	{
/**
panic codes for the Network Controller classes
*/
	enum TNetConPanic
		{
		ENetConBadStartType,					///< Network Controller Bad Start Type panic code
		ENetConNoCurrentRequest,				///< Network Controller No Current Request panic code
		ENetConBadDialogServerCompleteSignal,	///< Network Controller Bad Dialog Server Complete Signal panic code
		ENetConBadRequestCallback,				///< Network Controller Bad Request Callback panic code
		ENetConTelBearerMissing,				///< Network Controller Tel Bearer Missing panic code
		ETelBearerBadState,						///< TelBearer Bad State panic code
		ETelBearerTsyNameMissing,				///< TelBearer TsyName Missing panic code
		ETelBearerSigStrengthWatcherMissing,	///< TelBearer Signal Strength Watcher Missing panic code
		ENetConTelBearerAlreadyChecking			///< Network Controller TelBearer Already Checking panic code
		};
	}

/**
Panic - programming error!
@param aPanic,Network Controller Panic. 
*/
GLREF_C void NetConPanic(NetworkController::TNetConPanic aPanic);

#endif // __NETCONPANIC_H__

