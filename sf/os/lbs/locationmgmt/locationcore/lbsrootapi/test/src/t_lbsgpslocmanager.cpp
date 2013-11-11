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
#include "t_lbsgpslocmanager.h"





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
	User::SetJustInTime(EFalse);	
    CTLbsGpsLocManager* agpsManager = 0;
    TRAPD(err, agpsManager = CTLbsGpsLocManager::NewL());

	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}

	User::SetJustInTime(ETrue);	
	if (KErrNone == err)
		{
		delete agpsManager;
		}
	
	delete cleanup;
	delete scheduler;
	
	__UHEAP_MARKEND;
    
	return err;
    }


CTLbsGpsLocManager::CTLbsGpsLocManager() 
: CActive(EPriorityStandard)
    {
	
    }
CTLbsGpsLocManager::~CTLbsGpsLocManager()
    {
    delete iCloseDownRequestDetector;
    }
    
CTLbsGpsLocManager* CTLbsGpsLocManager::NewL()
    {
    CTLbsGpsLocManager* self = new(ELeave) CTLbsGpsLocManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return(self);
    }
    
void CTLbsGpsLocManager::ConstructL()
    {
    CActiveScheduler::Add(this);
    // Create the monitor which detects a closedown signal from
    // the LBS Root Process.
    iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, RProcess().SecureId());
    
    iPanicRequest.Attach(KLbsTestProcessUid, KTestLbsPanicRequestKey);
    iPanicRequest.Subscribe(iStatus);
    SetActive();
    
    RProcess::Rendezvous(KErrNone);
    }

void CTLbsGpsLocManager::OnProcessCloseDown()
    {
    Cancel();
    iPanicRequest.Close();
    delete iCloseDownRequestDetector;
    iCloseDownRequestDetector = 0;
	CActiveScheduler::Stop();
    }
    
//
// From MLbsProcessPanic
//    
void CTLbsGpsLocManager::PanicMe()
    {
    _LIT(KTLbsGpsLocManagerPanic, "Test GPS Manager");
	User::Panic(KTLbsGpsLocManagerPanic, 1);
    }
    
void CTLbsGpsLocManager::RunL()
    {
	PanicMe();
    }

void CTLbsGpsLocManager::DoCancel()
    {
    iPanicRequest.Cancel();
    }
    
