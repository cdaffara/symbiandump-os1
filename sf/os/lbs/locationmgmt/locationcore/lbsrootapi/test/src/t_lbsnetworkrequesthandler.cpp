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
#include <e32debug.h>

#include "lbsrootapi.h"
#include "t_lbsnetworkrequesthandler.h"





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
		
    CTLbsNetworkRequestHandler* nrh = 0;
    TRAPD(err, nrh = CTLbsNetworkRequestHandler::NewL());

	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}
    User::SetJustInTime(ETrue);

	if (KErrNone == err)
		{
		delete nrh;
		}
	
	delete cleanup;
	delete scheduler;
	
	__UHEAP_MARKEND;
    
	return err;
    }


CTLbsNetworkRequestHandler::CTLbsNetworkRequestHandler() 
: CActive(EPriorityStandard),
  iPanicOnCloseDown(EFalse)
    {
	
    }
CTLbsNetworkRequestHandler::~CTLbsNetworkRequestHandler()
    {
    Cancel();
    iPanicOnCloseDownRequest.Close();
    iResetFunctionCalled.Close();
    delete iCloseDownRequestDetector;
    }
    
CTLbsNetworkRequestHandler* CTLbsNetworkRequestHandler::NewL()
    {
    CTLbsNetworkRequestHandler* self = new(ELeave) CTLbsNetworkRequestHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return(self);
    }
    
void CTLbsNetworkRequestHandler::ConstructL()
    {
    CActiveScheduler::Add(this);
    // Create the monitor which detects a closedown signal from
    // the LBS Root Process.

	// third parameter means we want to be told about reset commands.
    iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, 
    												RProcess().SecureId(), 
    												this);
    
    iResetFunctionCalled.Attach(KLbsTestProcessUid, KTestLbsResetFunctionCalledKey);
    
    iPanicOnCloseDownRequest.Attach(KLbsTestProcessUid, KTestLbsPanicOnCloseDownRequestKey);
    iPanicOnCloseDownRequest.Subscribe(iStatus); 
    SetActive();
    }

// From MLbsProcessReset
void CTLbsNetworkRequestHandler::OnProcessReset()
    {
    // Don't actually do anything beyond setting a flag property 
    // to indicate that this has been called.
    iResetFunctionCalled.Set(KLbsNetRequestHandlerUid.iUid);
    }
    
// From MLbsProcessCloseDown
void CTLbsNetworkRequestHandler::OnProcessCloseDown()
    {    
    if(iPanicOnCloseDown)
        {
        TRequestStatus* status = &iStatus;
    	User::RequestComplete(status, KErrNone);
    	SetActive();
        }
    else
        {
	    CActiveScheduler::Stop();
        }
    }
    
//
// From MLbsProcessPanic
//    
void CTLbsNetworkRequestHandler::PanicMe()
    {
    User::Invariant(); 
    }
    
void CTLbsNetworkRequestHandler::RunL()
    {
    if(iPanicOnCloseDown)
        {
    	PanicMe();
        }
    else
	    iPanicOnCloseDown = ETrue;
    }

void CTLbsNetworkRequestHandler::DoCancel()
    {
    iPanicOnCloseDownRequest.Cancel();
    }
