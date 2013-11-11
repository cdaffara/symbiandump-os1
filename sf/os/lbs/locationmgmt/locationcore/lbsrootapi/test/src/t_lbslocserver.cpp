/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
/**
@file
@internalTechnology
@released
*/
#include <e32base.h>
#include <e32test.h>

#include "lbsrootapi.h"
#include "t_lbslocserver.h"





GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

    User::SetJustInTime(EFalse);

	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
		
    CTLbsLocServer* networkGateway = 0;
    TRAPD(err, networkGateway = CTLbsLocServer::NewL());

	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}

    User::SetJustInTime(ETrue);
    
	if (KErrNone == err)
		{
		delete networkGateway;
		}
	
	delete cleanup;
	delete scheduler;
	
	__UHEAP_MARKEND;
    
	return err;
    }


CTLbsLocServer::CTLbsLocServer() 
: CActive(EPriorityStandard)
    {
	
    }
CTLbsLocServer::~CTLbsLocServer()
    {
    iTimer->Cancel();
    delete iTimer;
    iLbsRootProcessApi.Close();
    Cancel();
    }
    
CTLbsLocServer* CTLbsLocServer::NewL()
    {
    CTLbsLocServer* self = new(ELeave) CTLbsLocServer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return(self);
    }
    
void CTLbsLocServer::ConstructL()
    {
    CActiveScheduler::Add(this);
        
    
    RProcess::Rendezvous(KErrNone);
    
	iTimer = CLbsCallbackTimer::NewL(*this);
	
	iLbsRootProcessApi.OpenL(RProcess().SecureId());
    iLbsRootProcessApi.RequestSystemStartup(iStatus);
    SetActive();
    }

    
void CTLbsLocServer::RunL()
    {
    // System started up
	const TTimeIntervalSeconds KTimerTenSec = 10;
	iTimer->EventAfter(KTimerTenSec, 1);
	
    }
    
void CTLbsLocServer::DoCancel()
    {
    }

void CTLbsLocServer::OnTimerEventL(TInt /*aTimerId*/)
    {
    // Timer expired. Panic
    User::Invariant();    	
    }
    
    
TInt CTLbsLocServer::OnTimerError(TInt /*aTimerId*/, TInt aError)
    {
	return(aError);
    }
