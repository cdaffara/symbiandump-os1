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

#include <e32debug.h>
#include "lbsdevloggermacros.h"
#include "lbsrootapi.h"
#include "privacyshutdown.h"


// P&S Key we check how many outstanding requests there are
const TInt EPrivacyNumberOfRequest = 0x1028720F;


/**
Static public constructor
*/
CPrivacyShutdown* CPrivacyShutdown::NewL()
	{
	LBSLOG(ELogP1, "CPrivacyShutdown::NewL() Begin\n");
	CPrivacyShutdown* self = new (ELeave) CPrivacyShutdown();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	LBSLOG(ELogP1, "CPrivacyShutdown::NewL() End\n");
	return self;
	}


/** 
Default constructor, set the timer to standard priority by default and
add timer to active scheduler.
*/	
CPrivacyShutdown::CPrivacyShutdown() : CTimer(CActive::EPriorityStandard),
	iState(EShutdownStateIdle),
	iTimerCount(0)
	{
	LBSLOG(ELogP1, "CPrivacyShutdown::CPrivacyShutdown() Begin\n");
	CActiveScheduler::Add(this);
	LBSLOG(ELogP1, "CPrivacyShutdown::CPrivacyShutdown() End\n");
	}


/** 
Default destructor, cancel any outstanding request of timer active object
*/	
CPrivacyShutdown::~CPrivacyShutdown()
	{
	LBSLOG(ELogP1, "CPrivacyShutdown::~CPrivacyShutdown() Begin\n");
	Cancel();
	LBSLOG(ELogP1, "CPrivacyShutdown::~CPrivacyShutdown() End\n");
	}


/** 
Call CTimer base class to construct the shut down timer
*/	
void CPrivacyShutdown::ConstructL()
	{
	LBSLOG(ELogP1, "CPrivacyShutdown::ConstructL() Begin\n");
	// call the base class version
	CTimer::ConstructL();
	LBSLOG(ELogP1, "CPrivacyShutdown::ConstructL() End\n");
	}


/** 
Start the timer by specifying the shut down delay to issue a time request 

@param aDelay Time interval in micro seconds
*/	
void CPrivacyShutdown::Start(const TTimeIntervalMicroSeconds32 aDelay)
	{
	LBSLOG(ELogP1, "CPrivacyShutdown::Start() Begin\n");
	iState = EShutdownStateTimerStarted;
	iTimerCount = aDelay;
	After(aDelay);
	LBSLOG(ELogP1, "CPrivacyShutdown::Start() End\n");
	}

/**
Cancel the outstanding timeout request.
*/
void CPrivacyShutdown::DoCancel()
	{
	LBSLOG(ELogP1, "CPrivacyShutdown::DoCancel() Begin\n");
	iState = EShutdownStateIdle;
	LBSLOG(ELogP1, "CPrivacyShutdown::DoCancel() End\n");
	}
	

/** 
Inherited from CActive, called when timer expires.
Shutdown LBS.
*/	
void CPrivacyShutdown::RunL()
	{
	LBSLOG(ELogP1, "CPrivacyShutdown::RunL() Begin\n");

	// Check if there are any notifications outstanding
	TInt outstandingRequests = 0;
    RProperty::Get(KUidSystemCategory, EPrivacyNumberOfRequest, outstandingRequests);
    if (outstandingRequests > 0)
        {
        LBSLOG(ELogP1, "CPrivacyShutdown::RunL() - Restarting Timer\n")
        Start(iTimerCount);
        }
    else
        {
        iState = EShutdownStateShutdownRequested;
    
        RLbsSystemController systemController;
        RProcess process;
        systemController.OpenL(process.SecureId());
        CleanupClosePushL(systemController);
        
        systemController.RequestSystemCloseDown(ETrue);
        
        CleanupStack::PopAndDestroy(&systemController);
        }
	
	LBSLOG(ELogP1, "CPrivacyShutdown::RunL() End\n");
	}

/** Return the current state of the shutdown timer.
*/
CPrivacyShutdown::TShutdownState CPrivacyShutdown::State() const
	{
	return iState;
	}
