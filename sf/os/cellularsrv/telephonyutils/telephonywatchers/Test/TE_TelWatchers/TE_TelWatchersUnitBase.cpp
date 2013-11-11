// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TE_WatcherUnitBase.cpp
// Telephony Watchers Unit Test base test code.
// 
//

/**
 @file
*/

#include "TE_TelWatchersUnitBase.h"
#include "TE_TelWatchersUnitWatcherLog.h"
#include "simtsy.h"

CTelWatchersUnitTestStep::CTelWatchersUnitTestStep()
	{
	// NOP
	} // CTelWatchersUnitTestStep::CTelWatchersUnitTestStep


CTelWatchersUnitTestStep::~CTelWatchersUnitTestStep()
	{
	// NOP
	} // CTelWatchersUnitTestStep::~CTelWatchersUnitTestStep


TVerdict CTelWatchersUnitTestStep::doTestStepPreambleL()
	{
	//
	// Mark for memory leaks!!!
	//
	__UHEAP_MARK;

	//
	// Create an Active Scheduler...
	//
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);

	TInt  err;

	//
	// Create the File Server and CWatcherLog objects used by some tests...
	//
  	err = iFs.Connect();
	TESTCHECKL(err, KErrNone);

	// Use the telephony watcher implementation of watcher logger (from UnitTestWatcherLog.cpp)
	// even though the implementation from watcher.lib is expected
	iWatcherLog = new (ELeave) CWatcherLog(Logger());
	
	return TestStepResult();
	} // CTelWatchersUnitTestStep::doTestStepPreambleL


TVerdict CTelWatchersUnitTestStep::doTestStepPostambleL()
	{
	delete iWatcherLog;
	iWatcherLog = NULL;
	
	iFs.Close();
	
	delete iScheduler;
	iScheduler = NULL;
	
	//
	// Check the heap for memory leaks...
	//
	__UHEAP_MARKEND;

	return TestStepResult();
	} // CTelWatchersUnitTestStep::doTestStepPostambleL


void CTelWatchersUnitTestStep::PauseToRunActiveSchedulerL(TInt aSeconds)
	{
	INFO_PRINTF2(_L("   Pausing %d seconds to run Active Scheduler."), aSeconds);

	CTimedStopScheduler*  pauser = new(ELeave) CTimedStopScheduler();
	CleanupStack::PushL(pauser);
	pauser->ConstructL();
	pauser->AfterSeconds(aSeconds);
	CleanupStack::PopAndDestroy(pauser);

	INFO_PRINTF1(_L("   Pause completed."));
	} // CTelWatchersUnitTestStep::PauseToRunActiveSchedulerL

CWatcherObserverPS::CWatcherObserverPS() 
                 :CActive(CActive::EPriorityHigh)
{ 
    iProperty = 0;
    CActiveScheduler::Add(this); 
}

CWatcherObserverPS::~CWatcherObserverPS()
{ 
    Cancel(); 
}

void CWatcherObserverPS::DoCancel() 
{   //-- cancel subscription to the property
    if(iProperty) 
    {
        iProperty->Cancel();     	
    }
}

/**
* Bind an integer property to this observer. Then we can get property value and wait for its change
*
* @param    aProp  - pointer to the property. The property object must be definer and attached to.
*
* @see      WaitForPropertyChangeL  
* @see      GetPropertyValue
* 
*/
void CWatcherObserverPS::BindProperty(RProperty *aProp)
{
	iProperty = aProp;
}


/**
 * Issue request for waiting for a property change. The property must be set by BindProperty method
 *
 * @see  BindProperty
 * @note This function can leave if the property is not set before.
 */
void CWatcherObserverPS::WaitForPropertyChangeL()
{
    _LIT(KPropObserverPanic, "CWatcherObserver::IssueRequest()");
    __ASSERT_ALWAYS(iProperty , User::Panic(KPropObserverPanic, KErrGeneral));
    __ASSERT_ALWAYS(!IsActive(), User::Panic(KPropObserverPanic, KErrGeneral));

   	iProperty->Cancel();
    iProperty->Subscribe(iStatus);
	SetActive();
   	CActiveScheduler::Start();
}

/**
* Get integer property value
*
* @param    aVal - reference to the variable 
*
* @return   standart error codes   
*/
TInt CWatcherObserverPS::GetPropertyValue(TInt &aVal)
	{
	if(iProperty)
		{
		return iProperty->Get(aVal);  	
		}

	return KErrNotFound;
	}


CTimedStopScheduler::CTimedStopScheduler()
  : CTimer(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	} // CTimedStopScheduler::CTimedStopScheduler


CTimedStopScheduler::~CTimedStopScheduler()
	{
	Cancel();
	} // CTimedStopScheduler::~CTimedStopScheduler


void CTimedStopScheduler::ConstructL()
	{
	CTimer::ConstructL();
	} // CTimedStopScheduler::ConstructL


void CTimedStopScheduler::AfterSeconds(TInt aTime)
	{
	After(TTimeIntervalMicroSeconds32(aTime * 1000000));
	CActiveScheduler::Start();
	} // CTimedStopScheduler::AfterSeconds


void CTimedStopScheduler::RunL()
	{
	CActiveScheduler::Stop();
	} // CTimedStopScheduler::RunL

void CTelWatchersUnitTestStep::SetSimTsyTestNumberL(TInt aTestNumber)
  	{	
  	TInt res = RProperty::Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,aTestNumber);
	if (res==KErrNotFound)
	    {
	    User::LeaveIfError(RProperty::Define(KUidPSSimTsyCategory,KPSSimTsyTestNumber, RProperty::EInt, aTestNumber));
	    }
	else
        {
        User::LeaveIfError(res);
        }   
	} // CTelWatchersIntTestStep::SetSimTsyTestNumberL

void CTelWatchersUnitTestStep::CheckSimTsyTestNumberL(TInt aTestNumber)
  	{
	INFO_PRINTF2(_L("Checking SIMTSY config number to be %d..."), aTestNumber);

	// Ensure that the test number has been set properly
	RProperty testNumberInUseProperty;
	User::LeaveIfError(testNumberInUseProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse));
	CleanupClosePushL(testNumberInUseProperty);

	TInt testNumberInUseCheck;
	User::LeaveIfError(testNumberInUseProperty.Get(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse,testNumberInUseCheck));
	if (aTestNumber != testNumberInUseCheck)
		{
		INFO_PRINTF2(_L("Incorrect SIMTSY config number (%d)..."), testNumberInUseCheck);
		User::Leave(KErrNotFound);		
		}

	CleanupStack::PopAndDestroy(&testNumberInUseProperty);
	} // CTelWatchersIntTestStep::CheckSimTsyTestNumberL
