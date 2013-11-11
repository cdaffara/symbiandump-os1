// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file Te_LbsDebugSuiteStepBase.cpp
 @internalTechnology
*/

#include "Te_LbsDebugSuiteStepBase.h"
#include "Te_LbsDebugSuiteDefs.h"

// Device driver constants
const TTimeIntervalMicroSeconds32 KDebugTimeout = 500000; //(500 ms)

CTinyTestClass* CTinyTestClass::NewL(MLbsTestStepObserver* aOwner)
    {
	CTinyTestClass* self = new (ELeave)CTinyTestClass(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
    }

CTinyTestClass::CTinyTestClass(MLbsTestStepObserver* aOwner)
: CActive(EPriorityStandard),
  iOwner(aOwner),
  iState(EWaitingToStart),
  iFinished(EFalse)
    { 
	 
    }
void CTinyTestClass::ConstructL()
    {
	CActiveScheduler::Add(this);
	iTimer = CLbsCallbackTimer::NewL(*this);
    TRequestStatus* pStat = &iStatus;	
  	User::RequestComplete(pStat, KErrNone);
  	SetActive();
    }
    
void CTinyTestClass::RunL()
	{
	switch(iState)
		{
		case EWaitingToStart:
			{
			iState = EWaitingForFinish;
			SetActive();
			TRAPD(err, iOwner->StartLbsTestStepL(iStatus));
			if(err != KErrNone)
				{
    	       	Cancel();
				iOwner->SetResult(EFail);
				}

			break;
			};
    	    
		case EWaitingForFinish:	
			{
			//SetActive();
			iFinished = iOwner->DoLbsTestStepL(iStatus);
			
			if (iFinished)
				{
				iOwner->FinishLbsTestStep(iStatus);
				CActiveScheduler::Stop(); // finished the test  
				}

			else
				iTimer->EventAfter(KDebugTimeout, 1);
        	break;
            };
        }
    }
    
void CTinyTestClass::DoCancel()
    {
	delete iTimer;
	}
    
    
TInt CTinyTestClass::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	iOwner->SetResult(EFail);
	return aError;
	}
	
	
void CTinyTestClass::OnTimerEventL(TInt /*aTimerId*/)
	{
    TRequestStatus* pStat = &iStatus;	
  	User::RequestComplete(pStat, KErrNone);
	SetActive();
	}
    
    


void CTe_LbsDebugSuiteStepBase::StartLbsTestStepL(TRequestStatus& /* aStatus */)
    {
    }
    
    
TBool CTe_LbsDebugSuiteStepBase::DoLbsTestStepL(TRequestStatus& /*aStatus*/)
	{
	return ETrue;
	}
    
void CTe_LbsDebugSuiteStepBase::FinishLbsTestStep(TRequestStatus& /*aStatus*/)
    {    	    
    }

void CTe_LbsDebugSuiteStepBase::SubscribeToOomBus(TRequestStatus& aStatus)
	{
	iOomRequest.OomTestNotifiyUpdates(aStatus);	
	}
	
void CTe_LbsDebugSuiteStepBase::SetResult(TVerdict aResult)
	{
	SetTestStepResult(aResult);	
	}

TVerdict CTe_LbsDebugSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	rootScheduler = new CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	
    iCounter = 0;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsDebugSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	delete rootScheduler;
	return TestStepResult();
	}

CTe_LbsDebugSuiteStepBase::~CTe_LbsDebugSuiteStepBase()
	{
	}

CTe_LbsDebugSuiteStepBase::CTe_LbsDebugSuiteStepBase()
	{
	}
	
	
