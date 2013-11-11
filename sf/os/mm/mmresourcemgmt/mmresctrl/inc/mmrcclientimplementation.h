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

#ifndef R_MMRCCLIENTIMPLEMENTATION_H
#define R_MMRCCLIENTIMPLEMENTATION_H

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include <e32msgqueue.h>


#include "mmrcclient.h"
#include "mmrcclientserver.h"
#include "mmrcclientsession.h"
#include "mmrcmsgqueuehandlerobserver.h"

class MMultimediaResourceControlObserver;
class CMMRCClientMsgQueueHandler;

NONSHARABLE_CLASS(CMMRCClientImplementation) : public CBase, public MMMRCMsgQueueHandlerObserver
	{
public:
	static CMMRCClientImplementation* NewL(MMultimediaResourceControlObserver& aCallback);
	~CMMRCClientImplementation();

	TUint64 LogOn(TProcessId aProcessId);
	void Close();

	TInt SendResourceRequest(MLogicalChain* aLogicalChainLastCommited, MLogicalChain* aLogicalChainRequested, CAudioContext* aContext);
	
	//for notification
	TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData);
	TInt CancelRegisterAsClient(TUid aEventType);
	TInt WillResumePlay();
  
	//from MMMRCMsgQueueHandlerObserver
	void HandleMessage(TMMRCQueueItem& aMessage);

	void ResetMessages();
private:
	void ConstructL();
	CMMRCClientImplementation(MMultimediaResourceControlObserver& aCallback);

private:
	MMultimediaResourceControlObserver& iObserver;
	RMMRCClientSession iMMRCClientSession;
	RMsgQueue<TMMRCQueueItem> iMsgQueue;
	CMMRCClientMsgQueueHandler* iMsgClientQueueHandler;
	};

#endif  /* R_MMRCCLIENTIMPLEMENTATION_H */
