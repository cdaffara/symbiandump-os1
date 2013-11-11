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

#include 	"SourceLocationEventObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::CSourceLocationEventObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSourceLocationEventObserver::CSourceLocationEventObserver()
    :   CActive(CActive::EPriorityStandard),
    	iStopped(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSourceLocationEventObserver::ConstructL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MSourceLocationCallback& aCallback )
    {
    CActiveScheduler::Add(this);
	iMessageHandler = aMessageHandler;
	iCustomCommand = &aCustomCommand;
	iCallback = &aCallback;
	}

// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSourceLocationEventObserver* CSourceLocationEventObserver::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MSourceLocationCallback& aCallback )
    {
    CSourceLocationEventObserver* self = new(ELeave) CSourceLocationEventObserver();
    CleanupStack::PushL(self);
    self->ConstructL(aMessageHandler, aCustomCommand, aCallback);
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::~CSourceLocationEventObserver
// Destructor
// -----------------------------------------------------------------------------
//
CSourceLocationEventObserver::~CSourceLocationEventObserver()
    {
    // We should not have to cancel the outstanding request because the message
    // handler will complete our request with KErrCancel in its destructor.
    Cancel();
    }

// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::Start
// Kickoff the event observer by issuing the first observation message.
// -----------------------------------------------------------------------------
//
void CSourceLocationEventObserver::Start()
    {
	if( !iStopped && !IsActive() )
		{
    	iCustomCommand->CustomCommandAsync(iMessageHandler, (TInt)ESlfObserve, KNullDesC8, KNullDesC8, iDataPckgFrom, iStatus);
    	iStopped = EFalse;
    	SetActive();
		}
    }

// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::Stop
// -----------------------------------------------------------------------------
//
void CSourceLocationEventObserver::Stop()
    {
	iStopped = ETrue;
    }

// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::RunL
// Invoke by the active scheduler when a request completes, In this case, our
// observation message has completed.
// The proxy is notified. Afterwards, reissue the request to continue observation.
// -----------------------------------------------------------------------------
//
void CSourceLocationEventObserver::RunL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationEventObserver::RunL()\n"));
#endif

	if( iStatus == KErrNone )
		{
		iCallback->SourceLocationEvent(iDataPckgFrom);
		Start();
		}
	else
		{
		iStopped = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CSourceLocationEventObserver::DoCancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CSourceLocationEventObserver::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationEventObserver::DoCancel()\n"));
#endif
    iStopped = ETrue;
    }

// End of file

