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
// @file TLbschannel.h
// This is the header file which contains the channel utilities for communication between
// the test harness and the test AGps module.
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef TLbsCHANNEL_H
#define TLbsCHANNEL_H

// User includes
#include <lbs/test/tlbschannelutils.h>

// Epoc includes
#include <e32property.h>


class MT_LbsChannelObserver;
class CT_ChannelMonitor;


/** Logical channel used for communication between Test Harness and
the test A-GPS module. 

Messages are:
- sent by calling SendMessage.
- received via the Mixin callback MT_LbsChannelObserver::ProcessChannelMessage.
*/
NONSHARABLE_CLASS(RT_LbsChannel)
	{
public:
	enum TT_LbsChannelId
		{
		EChannelUnknown			= -1,
		EChannelTH2TAGPS 		= 0,
		EChannelTAGPS2TH		= 1
		};

	IMPORT_C static void InitializeL(TT_LbsChannelId aChannelId);
	IMPORT_C static void ShutDownL(TT_LbsChannelId aChannelId);
	
	IMPORT_C RT_LbsChannel();
	IMPORT_C void OpenL(TT_LbsChannelId aChannelId, MT_LbsChannelObserver& aObserver);
	IMPORT_C void Close();

	IMPORT_C void SendMessage(const TT_LbsMsgBase& aMessage, TRequestStatus& aStatus);
	IMPORT_C void CancelSendMessageNotification();

private:
	RProperty iSendProperty;
	RProperty iSendMsgReadProperty;
	CT_ChannelMonitor* iMonitor;
	};

/** Observer for incoming messages from a channel
*/
class MT_LbsChannelObserver
	{
public:	
	/** Called when a message arrives from a channel.
	@param aChannelId The channel being monitored. 
	@param aMessage The message received. */
	virtual void ProcessChannelMessage(RT_LbsChannel::TT_LbsChannelId aChannelId, const TT_LbsMsgBase& aMessage) = 0;
	};

/**
Monitors for incoming messages from a channel.
*/
NONSHARABLE_CLASS(CT_ChannelMonitor) : public CActive
	{
public:
	static CT_ChannelMonitor* NewL(MT_LbsChannelObserver& aObserver, RT_LbsChannel::TT_LbsChannelId aChannelId, TUid aPropId, TUint aPropKey, TUint aAckPropKey);
	~CT_ChannelMonitor();
		
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:
	void NotifyChannelMessage();
	CT_ChannelMonitor(MT_LbsChannelObserver& aObserver, RT_LbsChannel::TT_LbsChannelId aChannelId);
	void ConstructL(TUid aPropId, TUint aPropKey, TUint aAckPropKey);

private:
	MT_LbsChannelObserver&			 iObserver;
	RT_LbsChannel::TT_LbsChannelId iChannelId;
	
	RProperty iReceiveProperty;
	RProperty iReceiveMsgReadProperty;
	};

#endif // TLbsCHANNEL_H


