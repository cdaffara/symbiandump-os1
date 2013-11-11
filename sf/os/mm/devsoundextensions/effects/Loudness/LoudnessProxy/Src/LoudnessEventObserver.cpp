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

#include 	"LoudnessEventObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLoudnessEventObserver::CLoudnessEventObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLoudnessEventObserver::CLoudnessEventObserver()
    :   CActive(CActive::EPriorityStandard),
    	iStopped(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CLoudnessEventObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLoudnessEventObserver::ConstructL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MLoudnessCallback& aCallback )
    {
    CActiveScheduler::Add(this);
	iMessageHandler = aMessageHandler;
	iCustomCommand = &aCustomCommand;
	iCallback = &aCallback;
	}

// -----------------------------------------------------------------------------
// CLoudnessEventObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLoudnessEventObserver* CLoudnessEventObserver::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MLoudnessCallback& aCallback )
    {
    CLoudnessEventObserver* self = new(ELeave) CLoudnessEventObserver();
    CleanupStack::PushL(self);
    self->ConstructL(aMessageHandler, aCustomCommand, aCallback);
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CLoudnessEventObserver::~CLoudnessEventObserver
// Destructor
// -----------------------------------------------------------------------------
//
CLoudnessEventObserver::~CLoudnessEventObserver()
    {
    // We should not have to cancel the outstanding request because the message
    // handler will complete our request with KErrCancel in its destructor.
    Cancel();
    }

// -----------------------------------------------------------------------------
// CLoudnessEventObserver::Start
// Kickoff the event observer by issuing the first observation message.
// -----------------------------------------------------------------------------
//
void CLoudnessEventObserver::Start()
    {
	if( !iStopped && !IsActive() )
		{
    	iCustomCommand->CustomCommandAsync(iMessageHandler, (TInt)ELefObserve, KNullDesC8, KNullDesC8, iDataPckgFrom, iStatus);
    	iStopped = EFalse;
    	SetActive();
		}
    }

// -----------------------------------------------------------------------------
// CLoudnessEventObserver::Stop
// -----------------------------------------------------------------------------
//
void CLoudnessEventObserver::Stop()
    {
	iStopped = ETrue;
    }

// -----------------------------------------------------------------------------
// CLoudnessEventObserver::RunL
// Invoke by the active scheduler when a request completes, In this case, our
// observation message has completed.
// The proxy is notified. Afterwards, reissue the request to continue observation.
// -----------------------------------------------------------------------------
//
void CLoudnessEventObserver::RunL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessEventObserver::RunL()\n"));
#endif

	if( iStatus == KErrNone )
		{
		iCallback->LoudnessEvent(iDataPckgFrom);
		Start();
		}
	else
		{
		iStopped = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CLoudnessEventObserver::DoCancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CLoudnessEventObserver::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessEventObserver::DoCancel()\n"));
#endif
    iStopped = ETrue;
    }

// End of file

