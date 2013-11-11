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

#include 	"AudioEqualizerEventObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::CAudioEqualizerEventObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAudioEqualizerEventObserver::CAudioEqualizerEventObserver()
    :   CActive(CActive::EPriorityStandard),
    	iStopped(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerEventObserver::ConstructL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MAudioEqualizerCallback& aCallback )
    {
    CActiveScheduler::Add(this);
	iMessageHandler = aMessageHandler;
	iCustomCommand = &aCustomCommand;
	iCallback = &aCallback;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAudioEqualizerEventObserver* CAudioEqualizerEventObserver::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MAudioEqualizerCallback& aCallback )
    {
    CAudioEqualizerEventObserver* self = new(ELeave) CAudioEqualizerEventObserver();
    CleanupStack::PushL(self);
    self->ConstructL(aMessageHandler, aCustomCommand, aCallback);
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::~CAudioEqualizerEventObserver
// Destructor
// -----------------------------------------------------------------------------
//
CAudioEqualizerEventObserver::~CAudioEqualizerEventObserver()
    {
    // We should not have to cancel the outstanding request because the message
    // handler will complete our request with KErrCancel in its destructor.
    Cancel();
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::Start
// Kickoff the event observer by issuing the first observation message.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerEventObserver::Start()
    {
	if( !iStopped && !IsActive() )
		{
    	iCustomCommand->CustomCommandAsync(iMessageHandler, (TInt)EAefObserve, KNullDesC8, KNullDesC8, iDataPckgFrom, iStatus);
    	iStopped = EFalse;
    	SetActive();
		}
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::Stop
// -----------------------------------------------------------------------------
//
void CAudioEqualizerEventObserver::Stop()
    {
	iStopped = ETrue;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::RunL
// Invoke by the active scheduler when a request completes, In this case, our
// observation message has completed.
// The proxy is notified. Afterwards, reissue the request to continue observation.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerEventObserver::RunL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerEventObserver::RunL()\n"));
#endif

	if( iStatus == KErrNone )
		{
		iCallback->AudioEqualizerEventL(iDataPckgFrom);
		Start();
		}
	else
		{
		iStopped = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerEventObserver::DoCancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerEventObserver::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerEventObserver::DoCancel()\n"));
#endif
    iStopped = ETrue;
    }

// End of file

