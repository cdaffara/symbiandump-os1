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

#include "EnvironmentalReverbEventObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::CEnvironmentalReverbEventObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEnvironmentalReverbEventObserver::CEnvironmentalReverbEventObserver()
    :   CActive(CActive::EPriorityStandard),
    	iStopped(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbEventObserver::ConstructL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MEnvironmentalReverbCallback& aCallback )
    {
    CActiveScheduler::Add(this);
	iMessageHandler = aMessageHandler;
	iCustomCommand = &aCustomCommand;
	iCallback = &aCallback;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEnvironmentalReverbEventObserver* CEnvironmentalReverbEventObserver::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MEnvironmentalReverbCallback& aCallback )
    {
    CEnvironmentalReverbEventObserver* self = new(ELeave) CEnvironmentalReverbEventObserver();
    CleanupStack::PushL(self);
    self->ConstructL(aMessageHandler, aCustomCommand, aCallback);
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::~CEnvironmentalReverbEventObserver
// Destructor
// -----------------------------------------------------------------------------
//
CEnvironmentalReverbEventObserver::~CEnvironmentalReverbEventObserver()
    {
    // We should not have to cancel the outstanding request because the message
    // handler will complete our request with KErrCancel in its destructor.
    Cancel();
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::Start
// Kickoff the event observer by issuing the first observation message.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbEventObserver::Start()
    {
	if( !iStopped && !IsActive() )
		{
    	iCustomCommand->CustomCommandAsync(iMessageHandler, (TInt)EErfObserve, KNullDesC8, KNullDesC8, iDataPckgFrom, iStatus);
    	iStopped = EFalse;
    	SetActive();
		}
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::Stop
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbEventObserver::Stop()
    {
	iStopped = ETrue;
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::RunL
// Invoke by the active scheduler when a request completes, In this case, our
// observation message has completed.
// The proxy is notified. Afterwards, reissue the request to continue observation.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbEventObserver::RunL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbEventObserver::RunL()\n"));
#endif

	if( iStatus == KErrNone )
		{
		iCallback->EnvironmentalReverbEvent(iDataPckgFrom);
		Start();
		}
	else
		{
		iStopped = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbEventObserver::DoCancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbEventObserver::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbEventObserver::DoCancel()\n"));
#endif
    iStopped = ETrue;
    }

// End of file

