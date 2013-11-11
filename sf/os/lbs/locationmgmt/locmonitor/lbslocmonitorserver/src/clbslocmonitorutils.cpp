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


#include "clbslocmonitorutils.h"
#include "lbslocmonitorserverdata.h"
#include "lbsdevloggermacros.h"

/*
Method to check whether an RMessage2 was sent by the client defined
by aClientId.
*/
TBool CLbsLocMonitorUtils::MatchClient(const sessionAndSubSessionId* aClientId, const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorUtils::MatchClient");
	return ((static_cast<CSession2*> (aClientId->iSessionPtr) == aMessage.Session()) &&
			(aClientId->iSubsessionId == aMessage.Int3()));
	}
/*
Method to check whether the operation requested in aMessage is the same as the
operation requested in aFirst (regardless of originating client)
*/	
TBool CLbsLocMonitorUtils::MatchOperation(const TInt* aOperationCode, const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorUtils::MatchOperation");
	return (*aOperationCode == aMessage.Function());		
	}

/*
Method to check whether the aRequestMessage is a request that is to be
terminated as a consequence of the cancel operation in aCancelMessage
(both must originate in the same client and have related opcodes)
*/
TBool CLbsLocMonitorUtils::MatchCancelAndRequest(const RMessage2& aCancelMessage, const RMessage2& aRequestMessage)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorUtils::MatchCancelAndRequest");
	TBool ret = EFalse;	
	// Check that the messages come from the same client
	if((aCancelMessage.Session() == aRequestMessage.Session()) &&
		 (aCancelMessage.Int3() == aRequestMessage.Int3()))
		{
		// check that the cancel code 'matches' the request code
		if ((aCancelMessage.Function() == ECancelGetLastKnownPosition) &&
			 (aRequestMessage.Function() == EGetLastKnownPosition))
			{
			ret = ETrue;	
			}
		else if ((aCancelMessage.Function() == ECancelGetLastKnownPositionArea) &&
			 (aRequestMessage.Function() == EGetLastKnownPositionArea))
			{
			ret = ETrue;
			}
		}
	return ret;
	}
	

