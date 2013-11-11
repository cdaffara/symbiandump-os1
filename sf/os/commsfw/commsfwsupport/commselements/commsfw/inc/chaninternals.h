// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CHANINTERNALS_H__
#define __CHANINTERNALS_H__

namespace CommsFW 
	{

NONSHARABLE_CLASS(CChannelDrain) : public CBase
/** Used to read messages in the right order from channels
with more than one priority.
@internalComponent 
*/
	{
private:
	class CQueueDrain;
public:
	static CChannelDrain* NewL(RCFChannelRawBase& aChannel, RMsgQueueBase* aQueues, TInt aPriorities);
	~CChannelDrain();
	
	void QueueNotificationCompleted(TInt aStatus);
	void QueueNotificationCancelled(CChannelDrain::CQueueDrain* aCancelQueue);
	
	void NotifyDataAvailable(TRequestStatus& aStatus);
	void CancelDataAvailable();
			
private:
	CChannelDrain(RCFChannelRawBase& aChannel, RMsgQueueBase* aQueues, TInt aPriorities);
	void ConstructL();

private:
	NONSHARABLE_CLASS(CQueueDrain) : public CActive
		{
	public:
		CQueueDrain();
		
		void SetParentChannel(CChannelDrain& aChannelDrain);
		void RunL();
		void DoCancel();
		void NotifyDataAvailable(RMsgQueueBase& aQueue);
	private:
		CChannelDrain* iChannelDrain;
		};
	
	RCFChannelRawBase& iChannel;
	RMsgQueueBase* iQueues;
	TRequestStatus* iClientNotify;
	TInt iOutstandingNotifications;
	TInt iPriorities;
	CQueueDrain iDrains[1];
	};


	}

#endif	//__CHANINTERNALS_H__

