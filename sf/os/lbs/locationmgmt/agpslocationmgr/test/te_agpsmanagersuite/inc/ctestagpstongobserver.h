// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test Network Gateway Observer for AGPS Manager UNIT testing
// 
//

#ifndef __CTESTNETWORKGATEWAYOBSERVER_H__
#define __CTESTNETWORKGATEWAYOBSERVER_H__

#include <e32cmn.h>
#include <ecom/ecom.h>
#include "lbsnetinternalapi.h"

#include "ctestobserverbase.h"

class CTestSendWaiter;


/** GPS Module observer class definition.

This class opens the NG2AGPS communications channel
which allows the test owning this class to observe
messages sent by the AGPS Manager to the Network
Gateway.

Since the channel also works in the opposite direction,
the test has to use this class when it wants to inject
messages into the AGPS Manager as if they had
originated in the Network Gateway.

The channel (RLbsNetChannel) has to be opened within
an active object (CTestSendWaiter) because the "Send" operation
is asynchronous (it only completes when the AGPS Manager
has read the message)
*/
class CTestAgpsToNgObserver : public CTestObserverBase , public MLbsNetChannelObserver
	{

public:

	/*
	One flag for each message the AGPS Manager
	may send to the Network Gateway
	*/
	enum TObservedNetworkGatewayCalls
		{
		ESelfLocRequestCall 	= 0x0001,
		ESelfLocCancelCall		= 0x0002,
		EAssistDataReqCall		= 0x0004,
		ESystemStatusAdviceCall = 0x0008
		};

public:

	static CTestAgpsToNgObserver* NewL();

	~CTestAgpsToNgObserver();

	void SendSessionCompleteToAgpsManager(TInt aReason);
	
	// Not yet implemented as no UNIT test case needs tests this yet
	//SendAssistanceDataIndicationToAgpsManager(TInt aError, TLbsAsistanceDataGroup aMask);
    
	// Derived from MLbsNetChannelObserver. It gets called whenever the AGPS
	// Manager publishes a message for the Network Gateway
	//
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);	
	
	// Methods called by the test to find out what callbacks/messages have been
	// sent by the Manager to the Network Gateway (calling this methods decreases
	// the internal count of callbacks/messages received and clears the internal
	// flag corresponding to the callback)
	TBool WasSelfLocationRequested();
	TBool WasSelfLocationCancelled();
	TBool WasAssistanceDataRequested();
	TBool WasSystemStatusAdvised();
	
	// NOTE: no UNIT test currently verifies the contents of messages sent from
	// the AGPS Manager to the network gateway. If it had to be done, then a number
	// of internal data members with the parameters carried in those messages should
	// be created which would be set when ProcessNetChannelMessage is called. Then
	// getter methods would have to be provided to be called by the test.

private:

	CTestAgpsToNgObserver();
	
	void ConstructL();
	
	// Check that a callback has happened and decrease callback count
	TBool CheckCallbackObserved(const TObservedNetworkGatewayCalls aCall);

private:

	// Each bit in iObservedNgEvents represents a call made (message sent)
	// by the AGPS Manager to the Network Gateway (see 
	TUint iObservedNgEvents;
	
	TLbsNetSessionIdInt iSessionId; // Received from the Manager in a SelfLocate Request

	// Utility active object (only needed to send messages to AGPS Manager)
	CTestSendWaiter* iSendWaiter;
	};


// This active object is only required because the "send"
// operation in class CTestAgpsToNgObserver
// requires a TRequestStatus.
class CTestSendWaiter : public CActive
{
public:

	static CTestSendWaiter* NewL(MLbsNetChannelObserver& aChannelObserver);
	void SendMessageToAgpsManager(const TLbsNetInternalMsgBase& aMsg);
	~CTestSendWaiter();
	
private:
	CTestSendWaiter();
	void ConstructL(MLbsNetChannelObserver& aChannelObserver);
	// From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
private:

	// Comms channel to/from AGPS Manager
	RLbsNetChannel iNg2AgpsManager;
};


#endif // __CTESTNETWORKGATEWAYOBSERVER_H__

