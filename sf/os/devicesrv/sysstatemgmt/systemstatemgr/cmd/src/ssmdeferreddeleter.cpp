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
//

#include <e32const.h>

#include "ssmdeferreddeleter.h"
#include "ssmcommandlistimpl.h"
#include <ssm/ssmcommand.h>
#include "ssmdebug.h"
#include "ssmpanic.h"

#ifdef _DEBUG
const TInt KMilliSecondsInOneSecond = 1000000;
const TInt KMilliSecondsInFiveSeconds = 5000000;
#endif

CSsmDeferredDeleter* CSsmDeferredDeleter::NewL()
	{
	CSsmDeferredDeleter* self = new (ELeave) CSsmDeferredDeleter;
#ifdef _DEBUG
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
#endif
	return self;
	}

CSsmDeferredDeleter::~CSsmDeferredDeleter()
	{
	Cancel();
	delete iCommandList;
#ifdef _DEBUG
	DEBUGPRINT1A("All deferred commands completed and command list is deleted.");
	delete iTimer;
#endif
	}

void CSsmDeferredDeleter::RunL()
	{
	TInt indexOfPendingCommand = iCommandList->PendingCommandIndex();
	if (KErrNotFound == indexOfPendingCommand)
		{
		// all commands are finished
		delete this;
		}
	else
		{
		//register for pending command
		SetActive();
		MSsmCommand* cmd = (MSsmCommand*)((*iCommandList)[indexOfPendingCommand]);
		// if cmd is NULL then this object would register for next pending command when RunL() is called next time.
		if (cmd)
			{
			cmd->RegisterCompletionObserver(iStatus);
			}
		}
	}

void CSsmDeferredDeleter::DoCancel()
	{
#ifdef _DEBUG
	if (iTimer)
		{
		iTimer->Cancel();
		}
#endif	
	}

CSsmDeferredDeleter::CSsmDeferredDeleter()
	: CActive(EPriorityIdle)
	{
	}

/**
@panic ECmdNullPtr if the information used to create command is null
*/
void CSsmDeferredDeleter::DeferredDelete(CSsmCommandListImpl* aCommandList)
	{
	__ASSERT_DEBUG(aCommandList, PanicNow(KPanicDeferredDeleter, ECmdNullPtr));
	iCommandList = aCommandList;
	CActiveScheduler::Add(this);
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

#ifdef _DEBUG
TInt CSsmDeferredDeleter::TimeoutCallback(TAny* aSelf)
	{
	if (aSelf)
		{
		static_cast<CSsmDeferredDeleter*>(aSelf)->PrintWarning();
		}
	return KErrNone;
	}

void CSsmDeferredDeleter::PrintWarning()
	{
	DEBUGPRINT1A("Warning: Deferred deleter is waiting for pending command!!!");
	}

void CSsmDeferredDeleter::ConstructL()
	{
	if (IsAdded())
		{
		iTimer = CPeriodic::NewL(EPriorityNormal);
		iTimer->Start(KMilliSecondsInFiveSeconds, KMilliSecondsInOneSecond, TCallBack(CSsmDeferredDeleter::TimeoutCallback, this));
		}
	}


#endif
