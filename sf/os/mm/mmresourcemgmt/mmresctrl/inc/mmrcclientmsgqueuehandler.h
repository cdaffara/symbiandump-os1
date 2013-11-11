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


#ifndef MMRCCLIENTMSGQUEUEHANDLER_H
#define MMRCCLIENTMSGQUEUEHANDLER_H

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include <e32msgqueue.h>
#include "mmrcclientserver.h"

class MMMRCMsgQueueHandlerObserver;

NONSHARABLE_CLASS(CMMRCClientMsgQueueHandler) : public CActive
	{
public:
	static CMMRCClientMsgQueueHandler* NewL(
		RMsgQueue<TMMRCQueueItem>& aMsgQueue,
		MMMRCMsgQueueHandlerObserver& aObserver);

	~CMMRCClientMsgQueueHandler();
	void ReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	CMMRCClientMsgQueueHandler(
		RMsgQueue<TMMRCQueueItem>& aMsgQueue,
		MMMRCMsgQueueHandlerObserver& aObserver);
	void ConstructL();

private:
	RMsgQueue<TMMRCQueueItem>& iMsgQueue;
	MMMRCMsgQueueHandlerObserver& iObserver;
	};

#endif //__MMRCCLIENTMSGQUEUEHANDLER_H
