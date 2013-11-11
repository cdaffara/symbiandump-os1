/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
*/

#include "ctlbstestsuitestepbase.h"


TVerdict CTe_TestSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	_LIT(KTestName, "CTe_TestSuiteStepBase::doTestStepPreambleL()");
	INFO_PRINTF1(KTestName);
	
	iScheduler = new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(iScheduler);
    SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_TestSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	return TestStepResult();
	}

CTe_TestSuiteStepBase::~CTe_TestSuiteStepBase()
	{
	delete iScheduler;
	delete iModulesSettings;
	}

CTe_TestSuiteStepBase::CTe_TestSuiteStepBase()
	{
	}



void CTe_TestSuiteStepBase::CheckExpectedResult(TInt aResult, TInt aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        ERR_PRINTF2(KValueReturned, aResult);
        ERR_PRINTF2(KValueExpected, aExpectedResult);
        SetTestStepResult(EFail);
        }
    }


