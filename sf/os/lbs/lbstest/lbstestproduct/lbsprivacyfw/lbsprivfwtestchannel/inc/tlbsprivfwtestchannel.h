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
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef TLBS_PRIVFW_TESTCHANNEL_H
#define TLBS_PRIVFW_TESTCHANNEL_H


#include <e32def.h>
#include <e32property.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/test/tlbsprivfwtestchannelmsgtypes.h>

class MLbsTestChannelObserver;
class CLbsTestChannelMonitor;

/*
Message are sent by calling SendMessage.

Messages are received via the Mixin callback
MLbsTestChannelObserver::ProcessNetChannelMessage.
*/
NONSHARABLE_CLASS(RLbsTestChannel)
	{
public:
	/**
	@see KLbsTestChannelIdMax */
	enum TLbsTestChannelId
		{
		EChannelUnknown			= -1,
		EChannelHandler2Proxy 	= 0,
		EChannelProxy2Handler	= 1,

		};

	IMPORT_C static void InitializeL(TLbsTestChannelId aChannelId);
	IMPORT_C static void ShutDownL(TLbsTestChannelId aChannelId);
	
	IMPORT_C RLbsTestChannel();
	IMPORT_C void OpenL(TLbsTestChannelId aChannelId, MLbsTestChannelObserver& aObserver);
	IMPORT_C void Close();

	IMPORT_C void SendMessage(const TLbsTestChannelMsgBase& aMessage, TRequestStatus& aStatus);
	IMPORT_C void CancelSendMessageNotification();

private:
	RProperty iSendProperty;
	RProperty iSendMsgReadProperty;
	CLbsTestChannelMonitor* iMonitor;
	};

/** Observer for incoming messages from a channel
*/
class MLbsTestChannelObserver
	{
public:	
	/** Called when a message arrives from a channel.
	@param aMessage The message. */
	virtual void ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage) = 0;
	};

/**
Monitors for incoming messages from a channel.
*/
NONSHARABLE_CLASS(CLbsTestChannelMonitor) : public CActive
	{
public:
	static CLbsTestChannelMonitor* NewL(MLbsTestChannelObserver& aObserver, RLbsTestChannel::TLbsTestChannelId aChannelId, TUid aPropId, TUint aPropKey, TUint aAckPropKey);
	~CLbsTestChannelMonitor();
	
	void NotifyTestChannelMessage();
		
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:
	CLbsTestChannelMonitor(MLbsTestChannelObserver& aObserver, RLbsTestChannel::TLbsTestChannelId aChannelId);
	void ConstructL(TUid aPropId, TUint aPropKey, TUint aAckPropKey);

private:
	MLbsTestChannelObserver&			 iObserver;
	RLbsTestChannel::TLbsTestChannelId iChannelId;
	
	RProperty iReceiveProperty;
	RProperty iReceiveMsgReadProperty;
	};

#endif // TLBS_PRIVFW_TESTCHANNEL_H
