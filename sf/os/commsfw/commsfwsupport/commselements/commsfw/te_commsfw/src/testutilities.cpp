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
// Support classes used by test steps
// 
//

/**
 @file TestUtilities.cpp
*/
#include <e32base.h>
#include "TestUtilities.h"
#include <test/testexecutelog.h>

// Upon timer expiry we complete self with this, and then replace the KRequestPending
// of the underlying request
const TInt KBoundedWaitExpired = 1;

CBoundedCompletionWait* CBoundedCompletionWait::NewLC()
	{
	CBoundedCompletionWait* self = new(ELeave) CBoundedCompletionWait;
	CleanupStack::PushL(self);
	return self;
	}

CBoundedCompletionWait::CBoundedCompletionWait()
: CActive(EPriorityStandard), 
  iResult(ECompleted)
	{
	CActiveScheduler::Add(this);
	}

CBoundedCompletionWait::~CBoundedCompletionWait()
 	{
 	Cancel();
 	}

void CBoundedCompletionWait::WaitForRequestL(TTimeIntervalMicroSeconds32 aInterval)
	{
	iResult = EPendingRequest;
	__ASSERT_ALWAYS(!iTimer, User::Invariant());
	iTimer = CPeriodic::NewL(EPriorityStandard);
	iTimer->Start(aInterval, aInterval, TCallBack(TimerCallBack, this));
 	if (!IsActive())
		SetActive();
	CActiveScheduler::Start();
	}

void CBoundedCompletionWait::WaitWithoutRequestL(TTimeIntervalMicroSeconds32 aInterval)
	{
	iResult = EPendingTimer;
	__ASSERT_ALWAYS(!iTimer, User::Invariant());
	iTimer = CPeriodic::NewL(EPriorityStandard);
	iTimer->Start(aInterval, aInterval, TCallBack(TimerCallBack, this));
	CActiveScheduler::Start();
	}

CBoundedCompletionWait::TWaitResult CBoundedCompletionWait::Result() const 
	{ 
	return iResult; 
	}

TInt CBoundedCompletionWait::Status() const 
	{ 
	return iStatus.Int(); 
	}

void CBoundedCompletionWait::SetStatus(TInt aStatus)
	{ 
	iStatus = aStatus;
	}

void CBoundedCompletionWait::RunL()
	{
	// Remember how the event completed, cancel timer
	if(iStatus == KBoundedWaitExpired)
		{
		iResult = ETimedOut;
		iStatus = KRequestPending;
		SetActive();
		}
	else
		{
		iResult = ECompleted;
		}
	delete iTimer;
	iTimer = NULL;
	CActiveScheduler::Stop();
	}

TInt CBoundedCompletionWait::TimerCallBack(TAny* aSelf)
	{
	CBoundedCompletionWait* self = reinterpret_cast<CBoundedCompletionWait*>(aSelf);
	self->TimerCompleted();
	return 0;
	}

void CBoundedCompletionWait::TimerCompleted()
	{
	// If the real request hasn't yet completed or we
	// never expected it to then it's a timeout
	if(iResult == EPendingTimer)
		{
		TRequestStatus dummyStatus;
		TRequestStatus* status = &dummyStatus;
		User::RequestComplete(status, KBoundedWaitExpired);
		SetActive();
		}
	else if(iStatus == KRequestPending)
		{
		TRequestStatus* selfStatus = &iStatus;
		User::RequestComplete(selfStatus, KBoundedWaitExpired);
		}
	}

void CBoundedCompletionWait::DoCancel()
	{
	// Unlike proper AOs we cannot cancel the service we're waiting upon; our caller
	// must do that. So here we complete ourself.
	TRequestStatus* selfStatus = &iStatus;
	User::RequestComplete(selfStatus, KErrCancel);
	if (iTimer)
		iTimer->Cancel();
	}


CSelfPopScheduler* CSelfPopScheduler::CreateLC()
	{
	CSelfPopScheduler* self = new(ELeave)CSelfPopScheduler;
	CleanupStack::PushL(self);
	CActiveScheduler::Install(self);
	return self;
	}

CSelfPopScheduler::operator TCleanupItem()
	{
	return TCleanupItem(Cleanup, this);
	}

void CSelfPopScheduler::Cleanup(TAny* aItem)
	{
	CActiveScheduler::Install(NULL);
	delete reinterpret_cast<CSelfPopScheduler*>(aItem);
	}

