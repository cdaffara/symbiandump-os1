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
// Definition of assistance data handler sub-component of agps manager
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef LBSNETWORKREQUESTHANDLER_H
#define LBSNETWORKREQUESTHANDLER_H

#include <e32base.h>
#include "lbscommoninternaldatatypes.h"
#include <lbs/lbsnetcommon.h>
#include "lbsnetinternalapi.h"

#include "LbsInternalInterface.h"
#include "lbsassistancedatacacheapi.h"
#include "lbsnetinternalmsgfifo.h"

class MNetworkGatewayHandlerObserver
    {
public:
    virtual void OnAssistanceDataResponse(TInt aError, TLbsAsistanceDataGroup aMask) = 0;
    virtual void OnSessionComplete(TInt aReason, const TLbsNetSessionIdInt& aSessionId) = 0;
    };

class CNetworkGatewayHandler : public CActive, private MLbsNetChannelObserver
	{
public:
	static CNetworkGatewayHandler* NewL(MNetworkGatewayHandlerObserver* aObserver);
	~CNetworkGatewayHandler();
	
	TInt GetAssistanceDataItem(TLbsAssistanceDataItem aItem,RDataReaderRootBase& aDataRoot,	TTime& aTimeStamp);
	TInt GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp);
	void SendAssistanceDataRequest(TLbsAsistanceDataGroup aDataItemMask);
	void SendSelfLocationRequest(const TLbsNetSessionIdInt& aSessionId, const TLbsNetPosRequestOptionsAssistanceInt& aOptions);
	void SendSelfLocationCancel(const TLbsNetSessionIdInt& aSessionId, TInt aReason);
	void SendSystemStatusAdvice(TBool aTracking);
		
private:
	CNetworkGatewayHandler(MNetworkGatewayHandlerObserver* aObserver); 
	void ConstructL();
	
	//from MLbsNetChannelObserver
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
	//from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	// Helpers
	void SendOrQueueMessage(TLbsNetInternalMsgBase& aMsg);
	void QueueMessage(TLbsNetInternalMsgBase& aMsg);
	
private:
	MNetworkGatewayHandlerObserver* iObserver;
	RAssistanceDataCache	 		 iAssistanceDataCache;
	RLbsNetChannel					 iNGChannel;
	RLbsNetMsgFifo					 iQ;
	};

#endif // LBSNETWORKREQUESTHANDLER_H

