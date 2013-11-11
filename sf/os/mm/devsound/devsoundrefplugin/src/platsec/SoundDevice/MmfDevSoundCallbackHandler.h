// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMFDEVSOUNDCALLBACKHANDLER_H
#define __MMFDEVSOUNDCALLBACKHANDLER_H

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/server/sounddevice.h>
#include "MmfDevSoundProxy.h"

#include <e32msgqueue.h>
	
/*
 *
 *	Class Name:	CMsgQueueHandler
 *
 *	AO that manages the message queue
 *  @internalComponent
 *
 */
class CMsgQueueHandler : public CActive
	{
public:
	static CMsgQueueHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy,
								  MDevSoundObserver& aDevSoundObserver,
								  RMsgQueue<TMMFDevSoundQueueItem>* aMsgQueue,
								  MMMFDevSoundCustomInterfaceObserver& aDevSoundIntfcObserver);
	void SetObserver(MDevSoundObserver& aDevSoundObserver);
	~CMsgQueueHandler();
	void ReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	CMsgQueueHandler(RMMFDevSoundProxy* aDevSoundProxy,
					 MDevSoundObserver& aDevSoundObserver,
					 RMsgQueue<TMMFDevSoundQueueItem>* aMsgQueue,
					 MMMFDevSoundCustomInterfaceObserver& aDevSoundIntfcObserver);
	void ConstructL();
	
	void ReceiveAllEvents();
	
	void DoInitComplete();
	void DoPlayErrorComplete();
	void DoRecordErrorComplete();
	void DoBTBFCompleteL();
	void DoBTBECompleteL();
	void DoToneFinishedComplete();
	void DoSendEventToClientComplete();
	
private:
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver* iDevSoundObserver;
	TMMFDevSoundProxyHwBufPckg iSetPckg;
	
	TMMFEventPckg iEventPckg;

	RMsgQueue<TMMFDevSoundQueueItem>* iMsgQueue;
	TMMFDevSoundQueueItem	iCurrentItem;
	TMMFDevSoundProxyRequest iCurrentRequest;
	
	CMMFDataBuffer* iBuffer;	// buffer to be emptied
	MMMFDevSoundCustomInterfaceObserver& iDevSoundIntfcObserver;
	};

#endif
