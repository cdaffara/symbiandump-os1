/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

 
#include "te_lbslocationinfoconvertersuitestepbase.h"


TVerdict CTe_LbsLocationInfoConverterSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
    iScheduler = new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(iScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsLocationInfoConverterSuiteStepBase::doTestStepPostambleL()
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

CTe_LbsLocationInfoConverterSuiteStepBase::~CTe_LbsLocationInfoConverterSuiteStepBase()
	{

	}

CTe_LbsLocationInfoConverterSuiteStepBase::CTe_LbsLocationInfoConverterSuiteStepBase()
	{

	}


TInt CTe_LbsLocationInfoConverterSuiteStepBase::GetTestCaseId()
    {
    TInt testCaseId(KErrNotFound);
    // Ini file key names for test cases
    _LIT(KTestCaseId, "tc_id");
    TInt err = GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId);
    return testCaseId;
    }

TInt CTe_LbsLocationInfoConverterSuiteStepBase::GetTestCaseApi()
    {
    TInt testCaseApi(0);
    // Ini file key names for test cases
    _LIT(KTestCaseApi, "tc_api");
    TInt err = GetIntFromConfig(ConfigSection(), KTestCaseApi, testCaseApi);
    return testCaseApi;
    }

TInt CTe_LbsLocationInfoConverterSuiteStepBase::GetPluginUid()
    {
    TInt pluginUid(KErrNotFound);
    // Ini file key names for test cases
    _LIT(KPluginUid, "plugin_uid");
    TInt err = GetIntFromConfig(ConfigSection(), KPluginUid, pluginUid);
    return pluginUid;
    }
void CTe_LbsLocationInfoConverterSuiteStepBase::InitWaitersL()
	{

	}


void CTe_LbsLocationInfoConverterSuiteStepBase::DeleteWaiters()
	{

	}

