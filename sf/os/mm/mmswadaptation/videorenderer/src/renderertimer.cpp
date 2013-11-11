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
//

/**
 @file
 @internalComponent
*/

#include "renderertimer.h"
#include "rendererrelay.h"
#include "rendererutil.h"

/** C++ constructor */
CRendererTimer::CRendererTimer(CRendererRelay& aRenderer) 
: CTimer(EPriorityHigh), iRenderer(aRenderer)
	{
	CActiveScheduler::Add(this);
	}

/** Two-phased constructor. */
CRendererTimer* CRendererTimer::NewL(CRendererRelay& aRenderer)
	{
	CRendererTimer* self = new (ELeave) CRendererTimer(aRenderer);
	CleanupStack::PushL(self);
	self->ConstructL(); // this call CTimer::ConstructL
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor */
CRendererTimer::~CRendererTimer()
	{
	// cancel is called by CTimer destructor
	}

/** Start the renderer timer */
void CRendererTimer::Start(TTimeIntervalMicroSeconds32 aDelay)
	{
	DEBUGPRINT2(_L("CRendererTimer::Start entered with aDelay=%d"), aDelay.Int());

	__ASSERT_DEBUG(iRenderer.UpdateSubmitted() == EFalse, User::Panic(_L("CRT::Start"), KErrArgument));
	__ASSERT_DEBUG(IsActive() == EFalse, User::Panic(_L("CRT::Start"), KErrCorrupt));

	HighRes(aDelay);
	}

/** Handle completion */
void CRendererTimer::RunL()
	{
	DEBUGPRINT2(_L("CRendererTimer::RunL entered with status=%d"), iStatus.Int());

	if (iStatus == KErrNone)
		{
		iRenderer.RendererTimerExpired();
		}
	}


/** Two-phased constructor. */
CThreadUndertaker* CThreadUndertaker::NewL(RThread& aSubThread)
	{
	CThreadUndertaker* self;
	self = new (ELeave) CThreadUndertaker(aSubThread);
	return self;
	}

/** Constructor for the class. */
CThreadUndertaker::CThreadUndertaker(RThread& aSubThread) 
: CActive(CActive::EPriorityIdle), iSubThread(aSubThread)
	{
	CActiveScheduler::Add(this);

	iStatus = KRequestPending;
	SetActive();

	//Logon to the thread so we will receive signals
	iSubThread.Logon(iStatus);
	}

/** Destructor for this class. */
CThreadUndertaker::~CThreadUndertaker()
	{
	Cancel();
	}

/** This function is triggered when the thread being monitored terminates. */
void CThreadUndertaker::RunL()
	{
	// If the thread being monitored panic, panic this thread.
	TInt reason = iSubThread.ExitReason();
	TExitCategoryName category = iSubThread.ExitCategory();
	User::Panic(category, reason);
	}

/** Stop monitoring the thread for panics. */
void CThreadUndertaker::DoCancel()
	{
	iSubThread.LogonCancel(iStatus);
	}
