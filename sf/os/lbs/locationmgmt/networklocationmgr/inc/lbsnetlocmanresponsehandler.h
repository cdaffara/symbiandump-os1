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

#ifndef __LBSNETLOCMANRESPHANDLER_H__
#define __LBSNETLOCMANRESPHANDLER_H__

//****************************************************************
// Includes
//****************************************************************
//System
#include <e32base.h>

//LBS
#include "LbsInternalInterface.h"
#include "lbsreffnpint.h"
#include "lbsnetlocmancompletionhandler.h"
#include "LbsLogger.h"

//****************************************************************
// Classes
//****************************************************************
class CNetLocManLogic; //forward declaration

/** This class implements the handling of a location responses from the 
NetworkGateway, which it publises to the LocServer.

@internalComponent
@released
*/
class CNetLocManResponseHandler : public CActive,
									public MLbsLocSessionCompleteObserver
	{
public:
	static CNetLocManResponseHandler* NewL(CNetLocManLogic& aNetLocMan);
	~CNetLocManResponseHandler();
	
	void StartGettingRequests();

protected:
	CNetLocManResponseHandler(CNetLocManLogic& aNetLocMan); 
	void ConstructL();

protected: // From MLbsLocSessionCompleteObserver
	void OnSessionComplete(TInt aStatus, const TLbsNetSessionIdInt& aSessionId);
	
protected:
	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private:
	void ReportPosUpdate(TInt aStatus, TUid SessionOwner,
				TPositionInfo& aPosInfo, TTime aTargetTime);
				
private:
	RLbsNetworkPositionUpdates 	iPositionUpdate;
	CNetLocManCompletionHandler*	iCompletionHandler;

	CNetLocManLogic&				iNetLocMan;
	
	// Logging
	RLbsLogger						iLogger;
	};

#endif //__LBSNETLOCMANRESPHANDLER_H__

