// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of Test A_Gps module request handler component.
// 
//
 

#ifndef CTESTHARNESSHANDLER_H
#define CTESTHARNESSHANDLER_H

#include <lbs/test/tlbschannel.h>


/** Observer for TH Request Handler interface
*/
class MT_RequestObserver
	{
public:
	virtual TInt ProcessRequestUpdateInit(const TDesC& aConfigFileName, const TDesC& aConfigSection) = 0;
	virtual void ProcessRequestTimeOut(const TTimeIntervalMicroSeconds& aTimeOut) = 0;
	virtual void ProcessRequestError(TInt aError) = 0;
	virtual TInt ProcessRequestModuleOptions(TLbsHybridModuleOptions aModuleOption, TBool aModuleValue) = 0;
	virtual void ProcessRequestForcedUpdate() = 0;
	virtual void ProcessImmediateMeasurements(TInt aImmediateMeasurements) = 0;
	};

/** Gps Handler for the TH channel.

This class handles sending and receiving messages on the Test Harness channel.
It is implemented as an active object to allow it to asnychronously wait for 
messages to arrive. Sending is synchronous.
*/
class CT_TestHarnessHandler : public CActive, 
							  public MT_LbsChannelObserver
	{
public:
	static CT_TestHarnessHandler* NewL(MT_RequestObserver* aObserver);
	~CT_TestHarnessHandler();

	void SendDataOutResponse(TT_LbsAGpsResponseMsg::TModuleResponseType aResponse);
	
private:
	CT_TestHarnessHandler(MT_RequestObserver* aObserver);
	void ConstructL();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	// Send a message.
	void SendResponseMessage(const TT_LbsMsgBase* aMessage);

	// Buffer a message.
	void BufferMessage(const TT_LbsMsgBase* aMessage);
	
	// from MT_LbsChannelObserver
	void ProcessChannelMessage(RT_LbsChannel::TT_LbsChannelId aChannelId, const TT_LbsMsgBase& aMessage);

private:
	MT_RequestObserver* iObserver;
	RT_LbsChannel iTHChannel;
	RPointerArray<TT_LbsMsgBase> iMsgBuffer;
	};


#endif // CTESTHARNESSHANDLER_H
