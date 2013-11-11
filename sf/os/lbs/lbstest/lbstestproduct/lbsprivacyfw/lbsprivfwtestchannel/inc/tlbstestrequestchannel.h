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
// lbstestrequestchannel.h
// Definition of network request channel component.
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef LBS_TEST_REQUEST_CHANNEL_H
#define LBS_TEST_REQUEST_CHANNEL_H

#include <e32hashtab.h>
#include <lbs/test/tlbsprivfwtestchannel.h>
#include <lbs/test/tlbsprivfwtestchannelmsgfifo.h>


/**
Panic string for Testchannel
*/
_LIT(KTLbsPrivFwTestChannelPanic,"TLbsPrivFwTestChannelPanic");

//
// Network Request Handler Channel
//

/** Observer for Network Request Handler interface
*/
class MLbsTestChannelRequestObserver
	{
public:
	virtual void ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage) = 0;
	};


/** Manager for the Proxy/Privacy Notifier interfaces.

This class handles sending and receiving messages on the
Proxy/PrivacyNotifier interfaces. It is implemented as an active object
to allow it to asnychronously wait for messages to arrive. Sending
is synchronous.
*/
class CTestRequestChannel : public CActive, 
							   public MLbsTestChannelObserver
	{
public:
	IMPORT_C static CTestRequestChannel* NewL(MLbsTestChannelRequestObserver* aObserver, RLbsTestChannel::TLbsTestChannelId aChannelId);
	~CTestRequestChannel();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	// Send a response to the Controller/Request to the Proxy.
	IMPORT_C void SendPrivFwRequestMessage(const TLbsTestChannelMsgBase& aMessage);

	// from MLbsTestChannelObserver : process an incoming message from the Privacy Controller/Recieve response from Proxy
	IMPORT_C void ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);
	
private:
	CTestRequestChannel(MLbsTestChannelRequestObserver* aObserver);
	void ConstructL(RLbsTestChannel::TLbsTestChannelId aChannelId);
	
	void BufferMessage(const TLbsTestChannelMsgBase& aMessage);
	
private:
	MLbsTestChannelRequestObserver* 	iObserver;
	RLbsTestChannel 					iPrivFwRequestChannel;
	RLbsTestChannelMsgFifo				iQ;
		
	};


#endif // LBS_TEST_REQUEST_CHANNEL_H
