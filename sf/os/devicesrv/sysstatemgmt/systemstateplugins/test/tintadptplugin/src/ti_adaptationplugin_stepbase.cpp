// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ti_adaptationplugin_stepbase.h"

#include <e32debug.h>
#include <e32property.h>

_LIT (KExeToDefineTestPS, "\\sys\\bin\\definetestps.exe");

static TInt StopScheduler(TAny* aTestAdaptStep)
	{
	// We would stop the sceduler once all the required transitions are completed
	RDebug::Print(_L("Inside Stop Scheduler..."));
	CTestAdaptStep*  testAdaptStep = static_cast<CTestAdaptStep*>(aTestAdaptStep);
	testAdaptStep->StopActiveScheduler();
	return KErrNone;
	}

void CTestAdaptStep::StopActiveScheduler()
	{
	// We would stop the sceduler once all the required transitions are completed
	if (iAdptTransitionCompleted)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		}
	}

void CTestAdaptStep::SetActivationCompleted()
	{
	iAdptTransitionCompleted = ETrue;
	}

TVerdict CTestAdaptStep::doTestStepPreambleL()
	{
	RProcess processHandle;
	CleanupClosePushL(processHandle);
	
	//Start the test exe which defines property keys for loading reference plugins
	    
	TInt err = processHandle.Create(KExeToDefineTestPS, KNullDesC);
	INFO_PRINTF2(_L("Process creation returned : %d"), err);
	User::LeaveIfError(err);
	processHandle.Resume();
	
    // wait for the newly created process to rendezvous
    TRequestStatus status;
    processHandle.Rendezvous(status);
    User::WaitForRequest(status);
    TInt retVal = status.Int();
    INFO_PRINTF2(_L("iStatus.Int() returned : %d"), retVal);
    TEST(KErrNone == retVal);
    CleanupStack::PopAndDestroy();
    
	// Set all the property keys
	err = RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	err = RProperty::Set(KPropertyCategory, KStateAdaptationPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	err = RProperty::Set(KPropertyCategory, KRtcAdaptationPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	err = RProperty::Set(KPropertyCategory, KSimPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	err = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("Starting the scheduler in CTestEmergencyAdaptStep::doTestStepPreambleL ..."));
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, this);
	iAsyncStopScheduler->Set(stop);
	
	return CTestStep::doTestStepPreambleL();
	}

/** */
TVerdict CTestAdaptStep::doTestStepPostambleL()
	{
	//Unset all the PandS keys
	TInt err = RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 0); 
	TEST(KErrNone == err);
	
	err = RProperty::Set(KPropertyCategory, KStateAdaptationPluginPropertyKey, 0);
	TEST(KErrNone == err);
	
	err = RProperty::Set(KPropertyCategory, KRtcAdaptationPluginPropertyKey, 0);
	TEST(KErrNone == err);
	
	err = RProperty::Set(KPropertyCategory, KSimPluginPropertyKey, 0);
	TEST(KErrNone == err);

	err = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 0);
	TEST(KErrNone == err);
	
	
	return CTestStep::doTestStepPostambleL();
	}
