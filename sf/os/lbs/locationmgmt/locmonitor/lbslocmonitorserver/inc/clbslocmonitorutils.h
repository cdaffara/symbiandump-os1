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

	
#include <e32base.h>
#include "clbslocmonitorsession.h"

/**
 @file
 @internalTechnology
*/

#ifndef CLBSLOCMONITORUTILS_H
#define CLBSLOCMONITORUTILS_H


/*
This class is only used to give a scope to a collection of 
static methods required by other classes in the Location 
Monitor server
*/
class CLbsLocMonitorUtils
	{
	
public:

	/*
	Struct with the information needed to uniquely
	identify the client that sent an RMessage2.
	*/
	struct sessionAndSubSessionId
		{
		CLbsLocMonitorSession* iSessionPtr;
		TInt iSubsessionId;
		};
public:

	/*
	Method to check whether an RMessage2 was sent by the client defined
	by aClientId.
	*/
	static TBool MatchClient(const sessionAndSubSessionId* aClientId, const RMessage2& aMessage);

	/*
	Method to check whether the operation requested in aMessage is the same as the
	operation requested in aOperationType (regardless of originating client)
	*/
	static TBool MatchOperation(const TInt* aOperationType, const RMessage2& aMessage);
	
	/*
	Method to check whether the aRequestMessage is a request that is to be
	terminated as a consequence of the cancel operation in aCancelMessage
	(both must originate in the same client and have related opcodes)
	*/
	static TBool MatchCancelAndRequest(const RMessage2& aCancelMessage, const RMessage2& aRequestMessage);
	};
	

#endif // CLBSLOCMONITORUTILS_H
