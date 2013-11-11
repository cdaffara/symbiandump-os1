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
// Definition of Test Harness request handler component.
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef CTLBSAGPSHANDLER_H
#define CTLBSAGPSHANDLER_H

#include <e32hashtab.h>
#include <lbs/test/tlbschannel.h>


//
// Test Harness Request Handler Channel
//

/** Observer for TH Request Handler interface
*/
class MT_ResponseObserver
	{
public:
	virtual void ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse) = 0;
	};

/** Manager for the TH channel.

This class handles sending and receiving messages on the Test Harness channel.
It is implemented as an active object to allow it to asnychronously wait for 
messages to arrive. Sending is synchronous.
*/
class CT_LbsAGpsHandler : public CActive, 
						  public MT_LbsChannelObserver
	{
public:
	IMPORT_C static CT_LbsAGpsHandler* NewL(MT_ResponseObserver* aObserver);
	~CT_LbsAGpsHandler();

	IMPORT_C void SendRequestUpdateInitMsg(const TDesC& aConfigFileName, 
								  const TDesC& aConfigSection);
	IMPORT_C void SendRequestTimeOutMsg(const TTimeIntervalMicroSeconds& aTimeOut);
	IMPORT_C void SendRequestSetAllModuleOptions();
	IMPORT_C void SendRequestClearAllModuleOptions();
	IMPORT_C void SendRequestModuleOption(TLbsHybridModuleOptions aOption, TBool aValue);
	IMPORT_C void SendRequestForcedUpdate();
	IMPORT_C void SendRequestError(TInt aError);
	IMPORT_C void SendImmediateMeasurementsMsg(const TInt& aImmediateMeasurements); 

			
private:
	CT_LbsAGpsHandler(MT_ResponseObserver* aObserver);
	void ConstructL();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	// Send a message.
	void SendRequestMessage(const TT_LbsMsgBase* aMessage);

	// Buffer a message.
	void BufferMessage(const TT_LbsMsgBase* aMessage);
	
	// from MT_LbsChannelObserver
	void ProcessChannelMessage(RT_LbsChannel::TT_LbsChannelId aChannelId, const TT_LbsMsgBase& aMessage);

private:
	MT_ResponseObserver* iObserver;
	RT_LbsChannel iTHChannel;
	RPointerArray<TT_LbsMsgBase> iMsgBuffer;
	};


#endif // CTLBSAGPSHANDLER_H
