/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of the active event observer.
*
*/




// INCLUDE FILES
#ifdef _DEBUG
#include 	<e32svr.h>
#endif

#include 	"ListenerOrientationEventObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::CListenerOrientationEventObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CListenerOrientationEventObserver::CListenerOrientationEventObserver()
    :   CActive(CActive::EPriorityStandard),
    	iStopped(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CListenerOrientationEventObserver::ConstructL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MListenerOrientationCallback& aCallback )
    {
    CActiveScheduler::Add(this);
	iMessageHandler = aMessageHandler;
	iCustomCommand = &aCustomCommand;
	iCallback = &aCallback;
	}

// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CListenerOrientationEventObserver* CListenerOrientationEventObserver::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MListenerOrientationCallback& aCallback )
    {
    CListenerOrientationEventObserver* self = new(ELeave) CListenerOrientationEventObserver();
    CleanupStack::PushL(self);
    self->ConstructL(aMessageHandler, aCustomCommand, aCallback);
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::~CListenerOrientationEventObserver
// Destructor
// -----------------------------------------------------------------------------
//
CListenerOrientationEventObserver::~CListenerOrientationEventObserver()
    {
    // We should not have to cancel the outstanding request because the message
    // handler will complete our request with KErrCancel in its destructor.
    Cancel();
    }

// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::Start
// Kickoff the event observer by issuing the first observation message.
// -----------------------------------------------------------------------------
//
void CListenerOrientationEventObserver::Start()
    {
	if( !iStopped && !IsActive() )
		{
    	iCustomCommand->CustomCommandAsync(iMessageHandler, (TInt)ELofObserve, KNullDesC8, KNullDesC8, iDataPckgFrom, iStatus);
    	iStopped = EFalse;
    	SetActive();
		}
    }

// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::Stop
// -----------------------------------------------------------------------------
//
void CListenerOrientationEventObserver::Stop()
    {
	iStopped = ETrue;
    }

// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::RunL
// Invoke by the active scheduler when a request completes, In this case, our
// observation message has completed.
// The proxy is notified. Afterwards, reissue the request to continue observation.
// -----------------------------------------------------------------------------
//
void CListenerOrientationEventObserver::RunL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationEventObserver::RunL()\n"));
#endif

	if( iStatus == KErrNone )
		{
		iCallback->ListenerOrientationEvent(iDataPckgFrom);
		Start();
		}
	else
		{
		iStopped = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CListenerOrientationEventObserver::DoCancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CListenerOrientationEventObserver::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationEventObserver::DoCancel()\n"));
#endif
    iStopped = ETrue;
    }

// End of file

