// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_ut_ecamserverteststepbase.h"

// ====================================================================
// CAsyncTestActiveScheduler
// 
CAsyncTestActiveScheduler::CAsyncTestActiveScheduler(CTe_ut_ecam_serverSuiteStepBase& aTestStep):
	iTestStep (aTestStep)
	{	
	}

// when Error() is called, we want to stop the test
void CAsyncTestActiveScheduler::Error(TInt /*aError*/) const
	{
	//User::Leave(aError);
	CActiveScheduler::Stop();
	}

// ====================================================================
// CTe_ut_ecam_serverSuiteStepBase
// 
CTe_ut_ecam_serverSuiteStepBase::CTe_ut_ecam_serverSuiteStepBase()
	{
	}

CTe_ut_ecam_serverSuiteStepBase::~CTe_ut_ecam_serverSuiteStepBase()
	{
	}

TVerdict CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** doTestStepPreambleL *****"));
	iScheduler = new (ELeave) CAsyncTestActiveScheduler(*this);	
	CActiveScheduler::Install(iScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("***** doTestStepPostambleL *****"));
	delete iScheduler;
	iScheduler = NULL;
	
	return TestStepResult();
	}

void CTe_ut_ecam_serverSuiteStepBase::PostKickOffTestL(TInt /*aReason*/, TVerdict /*aResult*/)
	{
	
	}

void CTe_ut_ecam_serverSuiteStepBase::ChangeState()
	{
	
	}

void CTe_ut_ecam_serverSuiteStepBase::CreateWindowL()
	{
	// ++ Setting up window server ++  //
	//
	// Connect to a WS Session.
	User::LeaveIfError(iWsSession.Connect());
	// Create a Window Group.
	iWindowGroup = RWindowGroup(iWsSession);
	User::LeaveIfError(iWindowGroup.Construct(iWindowHandle++));
	// Create the Screen device for the WServ session.
	iWsSd = new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iWsSd->Construct());
	//
	//done in different class   
	//
    iWindow = new (ELeave) RWindow(iWsSession);
    User::LeaveIfError(iWindow->Construct(iWindowGroup, iWindowHandle));	    
	//
    //done in different class   
    //
	iGc = new (ELeave) CWindowGc(iWsSd);
    User::LeaveIfError(iGc->Construct());		
	// Reset the screen mode.
    if(iWsSd->CurrentScreenMode() != 0)
	    {
	    iWsSd->SetAppScreenMode(0);
	    iWsSd->SetScreenMode(0);
	    }
  
	iWindow->Activate();
	iGc->Activate(*iWindow); 
	iGc->Clear(); 
	iWindow->SetVisible(ETrue);
	iWsSession.Flush();
	// -- Setting up window server --  //
	}

void CTe_ut_ecam_serverSuiteStepBase::CreateDependencyFileL()
	{
	// enforce dependency
	// create the file the plugin depends on.
	User::LeaveIfError(iFs.Connect());
	RFile file;
	TInt err = file.Replace(iFs, KMMCameraPluginName, EFileShareAny);
  	file.Close();
  	if(err != KErrNone)
  		{
	    User::LeaveIfError(KErrNotSupported);
  		}
	}

void CTe_ut_ecam_serverSuiteStepBase::RemoveDependencyFile()
	{
	iFs.Delete(KMMCameraPluginName);
	iFs.Close();
	}


// ====================================================================
// CAsyncTestShutdown
// 
CAsyncTestShutdown* CAsyncTestShutdown::NewL(CTe_ut_ecam_serverSuiteStepBase* aAsyncTestStep)
	{
	CAsyncTestShutdown* self = new(ELeave) CAsyncTestShutdown(aAsyncTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CAsyncTestShutdown::CAsyncTestShutdown(CTe_ut_ecam_serverSuiteStepBase* aAsyncTestStep) 
: CTimer(EPriorityUserInput), iAsyncTestStep(aAsyncTestStep)
	{
	CActiveScheduler::Add(this);
	}
	
void CAsyncTestShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}
	
void CAsyncTestShutdown::Start(TTimeIntervalMicroSeconds32 aInterval, TInt aReason, TVerdict aResult)
	{
	iReason = aReason;
	iResult = aResult;
	After(aInterval);
	}

void CAsyncTestShutdown::RunL()
	{
	TRAPD(error, iAsyncTestStep->PostKickOffTestL(iReason, iResult));
	if (error != KErrNone)
		{
		User::Leave(error);
		}
	}

// RetrieveTimer Class
CRetrieveTimer* CRetrieveTimer::NewL(CTe_ut_ecam_serverSuiteStepBase* aTestStep)
	{
	CRetrieveTimer* self = new(ELeave) CRetrieveTimer(aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CRetrieveTimer::CRetrieveTimer(CTe_ut_ecam_serverSuiteStepBase* aTestStep) 
: CTimer(EPriorityUserInput), iTestStep(aTestStep)
	{
	CActiveScheduler::Add(this);
	}
	
void CRetrieveTimer::ConstructL()
	{
	CTimer::ConstructL();
	}
	
void CRetrieveTimer::Start(TTimeIntervalMicroSeconds32 aInterval)
	{
	iInterval = aInterval;
	After(iInterval);
	}

void CRetrieveTimer::RunL()
	{
	iTestStep->ChangeState();
	Start(iInterval);
	}
