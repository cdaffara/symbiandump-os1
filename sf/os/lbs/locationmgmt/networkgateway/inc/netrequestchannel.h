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
// Definition of network request channel component.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef NETREQUESTCHANNEL_H_
#define NETREQUESTCHANNEL_H_

#include <e32hashtab.h>
#include "lbsnrhngmsgs.h"
#include "lbsnetinternalapi.h"
#include "requestmessagebuffer.h"


class CNetworkRequestMessageBuffer;

//
// Network Request Handler Channel
//

/** Observer for Network Request Handler interface
*/
class MNetworkRequestObserver
	{
public:
	virtual void ProcessNetRequestMessage(const TLbsNetInternalMsgBase& aMessage) = 0;
	};

/** Manager for the Network Request Handler interface.

This class handles sending and receiving messages on the
Network Request Handler interface. It is implemented as an active object
to allow it to asnychronously wait for messages to arrive. Sending
is synchronous.
*/
class CNetworkRequestChannel : public CActive, 
							   public MLbsNetChannelObserver
	{
public:
	static CNetworkRequestChannel* NewL(MNetworkRequestObserver& aObserver);
	~CNetworkRequestChannel();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	// Send a message to the NRH.
	void SendNetRequestMessage(const TLbsNetInternalMsgBase& aMessage);
	
	// from MLbsNetChannelObserver : process an incoming message from the NRH
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
private:
	CNetworkRequestChannel(MNetworkRequestObserver& aObserver);
	void ConstructL();
	
	void SendMessageAndNotifyForResponse(const TLbsNetInternalMsgBase& aMessage);
	void SendNextBufferedMessage();
	void RemoveEmergencyMessage(const TLbsNetInternalMsgBase* aMessage);
	
private:
	MNetworkRequestObserver& iObserver;
	RLbsNetChannel iNetRequestChannel;
	CRequestMessageBuffer* iMsgBuffer;
	
	RPointerArray<TLbsNetInternalMsgBase> iEmergencyBuffer; // Buffer used for emergancy requests OOM safe.
	TLbsNetLocationRequestMsg iEmergencyLocationRequestMsg;
	TLbsNetMtLrRequestMsg iEmergencyPrivacyRequestMsg;
	};



#endif // NETREQUESTCHANNEL_H_
