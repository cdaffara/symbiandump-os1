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
 @file Te_LbsLoggingSuiteStepBase.cpp
*/

#include "Te_LbsLoggingSuiteStepBase.h"
#include "Te_LbsLoggingSuiteDefs.h"

// Device driver constants

TVerdict CTe_LbsLoggingSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	//create active schedule
	iSched=NULL;
	iSched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(iSched);
	CActiveScheduler::Install(iSched);

	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	CleanupStack::PushL(iTimer);
	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	
	return TestStepResult();
	}

TVerdict CTe_LbsLoggingSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.

	CleanupStack::PopAndDestroy();// iSched;
	CleanupStack::PopAndDestroy();// iTimer;
	SetTestStepResult(EPass);  // or EFail

	return TestStepResult();
	}

TInt CTe_LbsLoggingSuiteStepBase::StopScheduler(TAny* /*aPtr*/)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}

CTe_LbsLoggingSuiteStepBase::~CTe_LbsLoggingSuiteStepBase()
	{
	}

CTe_LbsLoggingSuiteStepBase::CTe_LbsLoggingSuiteStepBase()
	{
	}

// -------------------------------------------------------
/* test Logging active object */

CLbsLogTestActive::CLbsLogTestActive(TInt aPriority)
:   CActive(aPriority)
    {
    CActiveScheduler::Add(this);
    iDelayTime=0;
    }

CLbsLogTestActive::~CLbsLogTestActive()
    {
    Cancel();
    }

void CLbsLogTestActive::DoCancel()
    {
    TRequestStatus* s=&iStatus;
    User::RequestComplete(s, KErrNone);
    }

void CLbsLogTestActive::StartL()
    {
    iDelayCompletion=EFalse;
    iDelayTime=0;
    iStatus = KRequestPending;
    SetActive();
    }

void CLbsLogTestActive::StartL(TInt aDelay)
    {
    iDelayCompletion=ETrue;
    iDelayTime=aDelay;
    iStatus = KRequestPending;
    SetActive();
    }

void CLbsLogTestActive::RunL() 
    {
    if(iDelayCompletion && iDelayTime)
        {
        // Wait for events in other threads to have a go....
        User::After(iDelayTime);
        iDelayTime=0;
        iStoredStatus=iStatus;
        SetActive();
        TRequestStatus* s=&iStatus;
        User::RequestComplete(s, KErrNone);
        }
    else
        {
		if(!(iStatus.Int() == KErrNone || iStatus.Int() == KErrAlreadyExists || iStatus.Int() == KErrCancel))
			{
			User::Leave(iStatus.Int());
			}

        if(iDelayCompletion)
            iStatus=iStoredStatus;

	    CActiveScheduler::Stop();
        }
    }
    
