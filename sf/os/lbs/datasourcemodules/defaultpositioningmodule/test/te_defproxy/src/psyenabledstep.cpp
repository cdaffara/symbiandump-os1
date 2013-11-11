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
// Example CTestStep derived implementation
// 
//

/**
 @file PsyEnabledStep.cpp
 @internalTechnology
*/
#include "PsyEnabledStep.h"
#include "Te_defproxySuiteDefs.h"

CPsyEnabledStep::~CPsyEnabledStep()
/**
 * Destructor
 */
	{
	}

CPsyEnabledStep::CPsyEnabledStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPsyEnabledStep);
	}

TVerdict CPsyEnabledStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CPsyEnabledStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(3, KNetworkPsy1, 0, KIntGpsPsy1, 1, KNetworkPsy2, 1);

    ConfigPsyL(KIntGpsPsy1, 3, 
        KConfigLRNoError5s,        //Step 1. no error in 5s
        KConfigLRErrGeneral1s,     //Step 2. error in 1s
        KConfigLRErrGeneral1s      //Step 3. error in 1s
        );

    User::After(KSecond*7);

    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    
    //1. Enable PSY1 shall not affect location request
    SET_TIME
    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::After(KTinyDelay);
    ToggleModuleL(KNetworkPsy1, ETrue);
    User::WaitForRequest(status);
    CHECK_TIME(5)
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(posInfo.ModuleId(), KIntGpsPsy1, KWrongModuleIdReturned);
    

    ToggleModuleL(KNetworkPsy2, EFalse);
    
    ConfigPsyL(KNetworkPsy1, 2,
        KConfigLRNoError35s,      //Step2. no error in 35s
        KConfigLRErrNoMemory      //Step 3. error in 1s
        );

    User::After(KSecond*7);

    //2. Enable PSY3 during fallback shall has no effect 
    SET_TIME
    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::After(KSecond*5);
    ToggleModuleL(KNetworkPsy2, ETrue);
    User::WaitForRequest(status);
    CHECK_TIME(35)  //PSY1 response time
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(posInfo.ModuleId(), KNetworkPsy1, KWrongModuleIdReturned);
    
    //3. Enabled PSY3 shall be used in next LR
    SET_TIME
    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);
    CHECK_TIME(2)  //PSY1 and PSY2 response time
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(posInfo.ModuleId(), KNetworkPsy2, KWrongModuleIdReturned);
    
    // cleanup
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CPsyEnabledStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
