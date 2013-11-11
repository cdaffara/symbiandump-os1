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
#include "t_lbsnetworkgateway.h"





GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
		
    CTLbsNetworkGateway* networkGateway = 0;
    TRAPD(err, networkGateway = CTLbsNetworkGateway::NewL());

	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}

	if (KErrNone == err)
		{
		delete networkGateway;
		}
	
	delete cleanup;
	delete scheduler;
	
	__UHEAP_MARKEND;
    
	return err;
    }


CTLbsNetworkGateway::CTLbsNetworkGateway() 
: CActive(EPriorityStandard),
  iRequestCloseDownOnCloseDown(EFalse)
    {
	
    }
CTLbsNetworkGateway::~CTLbsNetworkGateway()
    {
    Cancel();
	iSecondCloseDownRequest.Close();
    delete iCloseDownRequestDetector;
    iLbsRootProcessApi.Close();
    }
    
CTLbsNetworkGateway* CTLbsNetworkGateway::NewL()
    {
    CTLbsNetworkGateway* self = new(ELeave) CTLbsNetworkGateway();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return(self);
    }
    
void CTLbsNetworkGateway::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    iLbsRootProcessApi.OpenL(KLbsTestProcessUid);

    // Create the monitor which detects a closedown signal from
    // the LBS Root Process.
    iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, RProcess().SecureId());
    
    iSecondCloseDownRequest.Attach(KLbsTestProcessUid, KTestLbsSecondCloseDownRequestKey);
    iSecondCloseDownRequest.Subscribe(iStatus); 
    SetActive();
    
    RProcess::Rendezvous(KErrNone);
    }

void CTLbsNetworkGateway::OnProcessCloseDown()
    {
    // For the case when we want to test two closedown requests coming in at 
    // the same time, use the flag to request a closedown when we've just been
    // told to close down. Carry on with the initial closedown anyway.
    if(iRequestCloseDownOnCloseDown)
        {
//    	iLbsRootProcessApi.RequestSystemCloseDown(); not for now - the test is implemented by multiple close requests from the test harness
        }
	CActiveScheduler::Stop();
    }
    
//
// From MLbsProcessPanic
//    
void CTLbsNetworkGateway::PanicMe()
    {
//    User::Invariant(); Not for the moment - leave it to the Gps Loc Manager to panic if one is needed.
    }
    
void CTLbsNetworkGateway::RunL()
    {
	iRequestCloseDownOnCloseDown = ETrue;
    }
void CTLbsNetworkGateway::DoCancel()
    {
    iSecondCloseDownRequest.Cancel();
    }
