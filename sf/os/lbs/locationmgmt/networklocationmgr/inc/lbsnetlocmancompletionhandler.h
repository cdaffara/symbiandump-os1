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
// netlocmanresponsehandler.h
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __LBSNETLOCMANCOMPLETIONHANDLER_H__
#define __LBSNETLOCMANCOMPLETIONHANDLER_H__

//****************************************************************
// Includes
//****************************************************************
//System
#include <e32base.h>

//LBS
#include "LbsInternalInterface.h"
#include "lbsngnlmint.h"


/** This class defines the handler for a session complete from the 
NetworkGateway.

@internalComponent
@released
*/
class MLbsLocSessionCompleteObserver
	{
public:
	virtual void OnSessionComplete(TInt aStatus, const TLbsNetSessionIdInt& aSessionId) = 0;
	};


/** This class implements the handling of a location responses from the 
NetworkGateway, which it publises to the LocServer.

@internalComponent
@released
*/
class CNetLocManCompletionHandler : public CActive
	{
public:
	static CNetLocManCompletionHandler* NewL(MLbsLocSessionCompleteObserver* aCompletionObserver);
	~CNetLocManCompletionHandler();
	
	void StartMonitor();

protected:
	CNetLocManCompletionHandler(MLbsLocSessionCompleteObserver* aCompletionObserver); 
	void ConstructL();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:
	RLbsNetworkLocationResponses	iLocationResp;
	MLbsLocSessionCompleteObserver* iCompletionObserver;
	};

#endif //__LBSNETLOCMANCOMPLETIONHANDLER_H__

