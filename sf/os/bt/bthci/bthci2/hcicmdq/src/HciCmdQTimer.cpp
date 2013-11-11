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
 @internalComponent
*/

#include "HciCmdQTimer.h"
#include "HciCmdQUtil.h"

#include <bluetooth/hcicmdqcontroller.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCICMDQ);
#endif

/**
Starts the completion timer.
*/
void CHCICmdQTimer::Restart(TUint aMilliseconds,
							TUint aCommandQItemId,
							MHCITimerClient& aClient)
	{
	LOG_FUNC

	iClient = &aClient;
	iCommandQItemId = aCommandQItemId;
	
	Cancel();
	After(aMilliseconds * 1000);
	}

void CHCICmdQTimer::ConstructL()
	{
	LOG_FUNC

	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

CHCICmdQTimer::CHCICmdQTimer()
  : CTimer(EPriorityStandard), iCommandQItemId(KInvalidCommandQueueItemId)
	{
	LOG_FUNC
	}

/*
This timer ensures that the stack can recover from hardware failing to
respond to a command in a timely manner, specified by the QDP, by
calling back into the stack when the timeout expires.  The timer is
started when the command is issued. A command must not take longer
than the starvation timeout to complete.
*/
/*static*/ CHCICmdQCompletionTimer* CHCICmdQCompletionTimer::NewL()
	{
	LOG_STATIC_FUNC

	CHCICmdQCompletionTimer* self = new (ELeave) CHCICmdQCompletionTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHCICmdQCompletionTimer::CHCICmdQCompletionTimer()
  : CHCICmdQTimer() 
	{
	}

void CHCICmdQCompletionTimer::RunL()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iClient != NULL, PANIC(KHCICmdQPanic, ENullTimerClient));
	
	iClient->CompletionTimeoutFired(iCommandQItemId);
	}

/*
This timer is started whenever the head of the pending commands queue
changes.  It is used to restart the stack in the event that the
pending queue is not processed frequently enough.
*/
/*static*/ CHCICmdQStarvationTimer* CHCICmdQStarvationTimer::NewL()
	{
	LOG_STATIC_FUNC

	CHCICmdQStarvationTimer* self =	new (ELeave) CHCICmdQStarvationTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHCICmdQStarvationTimer::CHCICmdQStarvationTimer() 
	: CHCICmdQTimer()
	{
	}

void CHCICmdQStarvationTimer::RunL()
	{
	LOG_FUNC

	__ASSERT_ALWAYS(iClient != NULL, PANIC(KHCICmdQPanic, ENullTimerClient));

	iClient->StarvationTimeoutFired(iCommandQItemId);
	}
