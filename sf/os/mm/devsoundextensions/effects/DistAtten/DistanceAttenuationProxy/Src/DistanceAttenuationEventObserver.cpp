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

#include 	"DistanceAttenuationEventObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::CDistanceAttenuationEventObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDistanceAttenuationEventObserver::CDistanceAttenuationEventObserver()
    :   CActive(CActive::EPriorityStandard),
    	iStopped(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationEventObserver::ConstructL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MDistanceAttenuationCallback& aCallback )
    {
    CActiveScheduler::Add(this);
	iMessageHandler = aMessageHandler;
	iCustomCommand = &aCustomCommand;
	iCallback = &aCallback;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDistanceAttenuationEventObserver* CDistanceAttenuationEventObserver::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	MDistanceAttenuationCallback& aCallback )
    {
    CDistanceAttenuationEventObserver* self = new(ELeave) CDistanceAttenuationEventObserver();
    CleanupStack::PushL(self);
    self->ConstructL(aMessageHandler, aCustomCommand, aCallback);
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::~CDistanceAttenuationEventObserver
// Destructor
// -----------------------------------------------------------------------------
//
CDistanceAttenuationEventObserver::~CDistanceAttenuationEventObserver()
    {
    // We should not have to cancel the outstanding request because the message
    // handler will complete our request with KErrCancel in its destructor.
    Cancel();
    }

// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::Start
// Kickoff the event observer by issuing the first observation message.
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationEventObserver::Start()
    {
	if( !iStopped && !IsActive() )
		{
    	iCustomCommand->CustomCommandAsync(iMessageHandler, (TInt)EDaefObserve, KNullDesC8, KNullDesC8, iDataPckgFrom, iStatus);
    	iStopped = EFalse;
    	SetActive();
		}
    }

// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::Stop
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationEventObserver::Stop()
    {
	iStopped = ETrue;
    }

// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::RunL
// Invoke by the active scheduler when a request completes, In this case, our
// observation message has completed.
// The proxy is notified. Afterwards, reissue the request to continue observation.
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationEventObserver::RunL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationEventObserver::RunL()\n"));
#endif

	if( iStatus == KErrNone )
		{
		iCallback->DistanceAttenuationEvent(iDataPckgFrom);
		Start();
		}
	else
		{
		iStopped = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CDistanceAttenuationEventObserver::DoCancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationEventObserver::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationEventObserver::DoCancel()\n"));
#endif
    iStopped = ETrue;
    }

// End of file

