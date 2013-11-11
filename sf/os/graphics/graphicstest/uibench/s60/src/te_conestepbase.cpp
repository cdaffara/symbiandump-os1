// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "te_conestepbase.h"


_LIT(KTestStepThread, "TestStepConeThread");


TVerdict CTe_ConeStepBase::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	
	TESTNOERRORL(iConeThread.Create(KTestStepThread, CTe_ConeStepBase::ConeThreadFunction,
	        KDefaultStackSize, NULL, this));
	
	iInitUISemaphore.CreateLocal(0);
	iConeThread.Resume(); // start the cone thread
	
	// wait for InitUIL() method to finish, that means test can start now
	iInitUISemaphore.Wait();
	
	return TestStepResult();
	}

TVerdict CTe_ConeStepBase::doTestStepPostambleL()
	{
	iProfiler->FreeResultsMemory(); // cleanup memory in the baseclass (otherwise heaptest panics)
	
	iConeThread.RequestComplete(iTestCompletionStatus, KErrNone); // informs conestopper
    
	TRequestStatus status;
	iConeThread.Rendezvous(status); // asynchronous, completes amongst others when thread terminates
	User::WaitForRequest(status); // waits for Rendevous() to finish
	iConeThread.Close();
	iInitUISemaphore.Close();
	
	return CTe_graphicsperformanceSuiteStepBase::doTestStepPostambleL();
	}


/**
 * Starts the CONE thread.
 */
TInt CTe_ConeStepBase::ConeThreadFunction(TAny* aParams)
     {
     CTe_ConeStepBase* testStep = static_cast<CTe_ConeStepBase*>(aParams);
     return testStep->StartControlEnvironment();     
     }

CCoeEnv* CTe_ConeStepBase::ConstructControlEnvironment()
    {
    CEikonEnv* eikonEnv = new CEikonEnv();
    if (eikonEnv)
        {
        TRAPD(err, eikonEnv->ConstructL());
        if (err != KErrNone)
            {
            delete eikonEnv;
            return NULL;
            }
        }    
    return eikonEnv;
    }

TInt CTe_ConeStepBase::StartControlEnvironment()
    {
    TInt err = KErrNoMemory;
    CConeThreadStopper* stopper = new CConeThreadStopper(*this);
    if (stopper)
        {
        CCoeEnv* coeEnv = ConstructControlEnvironment();
        if (coeEnv)
            {
            TRAP(err, InitUIL(coeEnv));
            if (err == KErrNone)
                {
                stopper->Start(iTestCompletionStatus);
                iInitUISemaphore.Signal();
                coeEnv->ExecuteD(); // runs until its terminated by the CConeThreadStopper
                coeEnv = NULL;
                }       
            delete coeEnv;
            }            
        delete stopper;
        }
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        iInitUISemaphore.Signal(); // also signal if an error occurs, otherwise test would be blocked
        }
    return err;
    }


void CTe_ConeStepBase::StopControlEnvironment()
   {
   CActiveScheduler::Stop();
   }



CConeThreadStopper::CConeThreadStopper(CTe_ConeStepBase& aTestStep) :
       CActive(EPriorityStandard), iTestStep(aTestStep)
   {
   // empty
   }

CConeThreadStopper::~CConeThreadStopper()
    {
    Cancel();
    }

void CConeThreadStopper::Start(TRequestStatus *&aStopStatus)
   {
   CActiveScheduler::Add(this);
   aStopStatus = &iStatus;
   iStatus = KRequestPending;
   SetActive();
   }

void CConeThreadStopper::DoCancel()
    {
    // empty
    }

void CConeThreadStopper::RunL()
   {
   iTestStep.StopControlEnvironment(); // stops the active scheduler of the CONE thread
   }

