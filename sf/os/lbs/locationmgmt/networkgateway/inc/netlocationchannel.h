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
// Definition of network location channel component.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef NETLOCATIONCHANNEL_H_
#define NETLOCATIONCHANNEL_H_

#include "lbsnetinternalapi.h"
#include "lbsngnlmint.h"
#include "lbsreffnpint.h" // for RLbsNetworkPositionUpdates
#include <lbs/lbsextendedsatellite.h>


class TPositionInfoBase;

//
// Network Location Channel
//

typedef TInt TLbsNetworkLocationRequestBase;

/** Observer for the Network Location interface.
*/
class MNetworkLocationObserver
	{
public:
	virtual void ProcessNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage) = 0;
	};

/** Manager for the Network Location interface.

This class handles sending and receiving messages on the
Network Location interface. It is implemented as an active object
to allow it to asnychronously wait for messages to arrive. Sending
is synchronous.
*/
class CNetworkLocationChannel : public CActive
	{
public:
	static CNetworkLocationChannel* NewL(MNetworkLocationObserver& aObserver);
	~CNetworkLocationChannel();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void SetNetworkPosition(const TLbsNetSessionIdInt& aSessionId, const TPositionInfoBase& aPosition);
	
	const TPositionInfoBase	& GetLastReferencePosition();

	void SendNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage);

private:
	void SetReferenceNetworkPosition(const TLbsNetSessionIdInt& aSessionId, const TPositionInfoBase& aPosition);
	void SetFinalNetworkPosition(const TLbsNetSessionIdInt& aSessionId, const TPositionInfoBase& aPosition);
	
private:
	CNetworkLocationChannel(MNetworkLocationObserver& aObserver);
	void ConstructL();
	
	void NotifyNetworkLocationRequestUpdate();

private:
	MNetworkLocationObserver& iObserver;
	RLbsNetworkLocationRequests iLocationRequests;
	RLbsNetworkLocationResponses iLocationResponses;
	RLbsNetworkPositionUpdates iReferencePositionUpdates;
	RLbsNetworkPositionUpdates iFinalPositionUpdates;
	TPositionExtendedSatelliteInfo iRefPosInfo;
	TPositionExtendedSatelliteInfo iFinalPosInfo;
	};

#endif // NETLOCATIONCHANNEL_H_
