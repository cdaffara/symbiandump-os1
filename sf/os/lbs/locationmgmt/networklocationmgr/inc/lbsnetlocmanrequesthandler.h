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
// netlocmanrequesthandler.h
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __LBSNETLOCMANREQHANDLER_H__
#define __LBSNETLOCMANREQHANDLER_H__

//****************************************************************
// Includes
//****************************************************************
//System
#include <e32base.h>

//LBS
#include "LbsInternalInterface.h"
#include "lbsngnlmint.h"
#include "lbstimer.h"

//****************************************************************
// Classes
//****************************************************************
class CNetLocManLogic; //forward declaration

/** This class implements the handling of a location request from the 
LocServer through the NetworkLocationManager onto the NetworkGateway.

@internalComponent
@released
*/
class CNetLocManRequestHandler : public CActive, public MLbsCallbackTimerObserver
	{
public:
	static CNetLocManRequestHandler* NewL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, CNetLocManLogic& aNetLocMan);
	~CNetLocManRequestHandler();
	void CancelRequestTimeout();
	void StartGettingRequests();
	
protected:
	CNetLocManRequestHandler(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, CNetLocManLogic& aNetLocMan); 
	void ConstructL();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
	//From MLbsCallbackTimerObserver
	virtual void OnTimerEventL(TInt aTimerId);
	virtual TInt OnTimerError(TInt aTimerId, TInt aError);

	
private:
	RLbsPositionUpdateRequests::TChannelIdentifer iChannel;
	RLbsPositionUpdateRequests 		iPosUpdateReq;
	CNetLocManLogic&				iNetLocMan;
	RLbsNetworkLocationRequests 	iLocationReq;
	CLbsCallbackTimer*				iTimer;
	};

#endif //__LBSNETLOCMANREQHANDLER_H__

