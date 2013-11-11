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

#include "mmrcclientmsgqueuehandler.h"
#include "mmrcmsgqueuehandlerobserver.h"

/**
*/
CMMRCClientMsgQueueHandler* CMMRCClientMsgQueueHandler::NewL(
   RMsgQueue<TMMRCQueueItem>& aMsgQueue,
   MMMRCMsgQueueHandlerObserver& aObserver)
	{
	CMMRCClientMsgQueueHandler* self = new(ELeave) CMMRCClientMsgQueueHandler(aMsgQueue,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
*/
CMMRCClientMsgQueueHandler::CMMRCClientMsgQueueHandler(
			RMsgQueue<TMMRCQueueItem>& aMsgQueue,
			MMMRCMsgQueueHandlerObserver& aObserver) 
	:CActive(EPriorityStandard), 
	iMsgQueue(aMsgQueue),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/**
*/
void CMMRCClientMsgQueueHandler::ConstructL()
	{
	ReceiveEvents();
	}

/**
*/
CMMRCClientMsgQueueHandler::~CMMRCClientMsgQueueHandler()
	{
	Cancel();
	iMsgQueue.Close();
	}

/**
*/
void CMMRCClientMsgQueueHandler::ReceiveEvents()
	{
	if(!IsActive())
		{
		iMsgQueue.NotifyDataAvailable(iStatus);
		SetActive();	   
		}
	}

/**
*/
void CMMRCClientMsgQueueHandler::RunL()
	{
	TMMRCQueueItem message;
	TInt error = iMsgQueue.Receive(message);	
	iObserver.HandleMessage(message);
	ReceiveEvents();
	}

/**
*/
TInt CMMRCClientMsgQueueHandler::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
	
/**
*/
void CMMRCClientMsgQueueHandler::DoCancel()
	{
	iMsgQueue.CancelDataAvailable();
	}

//EOF
