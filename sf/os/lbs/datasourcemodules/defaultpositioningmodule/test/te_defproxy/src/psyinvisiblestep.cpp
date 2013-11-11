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
 @file PsyInvisibleStep.cpp
 @internalTechnology
*/
#include "PsyInvisibleStep.h"
#include "Te_defproxySuiteDefs.h"

CPsyInvisibleStep::~CPsyInvisibleStep()
/**
 * Destructor
 */
	{
	}

CPsyInvisibleStep::CPsyInvisibleStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPsyInvisibleStep);
	}

TVerdict CPsyInvisibleStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CPsyInvisibleStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(3, KNetworkPsy1, 1, KIntGpsPsy1, 1, KNetworkPsy2, 1);

    ConfigPsyL(KNetworkPsy1, 1,
        KConfigLRNoError10s      //Step1. no error in 10s
        );
        
    ConfigPsyL(KIntGpsPsy1, 2, 
        KConfigLRNoError,       //Step 1. no error
        KConfigLRNoError10s     //Step 2. no error in 10s
        );

    ConfigPsyL(KNetworkPsy2, 1,
        KConfigLRNoError         //Step2. no error
        );
    
    User::After(KSecond*7);

    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    
    //1. Set PSY1 invisible while location request is on going. LR shall 
    //fallback to PSY2 and then fix from PSY2 is used.
    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::After(KTinyDelay);
    SET_TIME
    ToggleModuleVisibleL(KNetworkPsy1, EFalse);
    User::WaitForRequest(status);
    //CHECK_TIME(1)
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(posInfo.ModuleId(), KIntGpsPsy1, KWrongModuleIdReturned);
    
    ToggleModuleVisibleL(KNetworkPsy1, ETrue);
    //Config PSY1 again since it was unloaded when it was invisible.
    ConfigPsyL(KNetworkPsy1, 1,
        KConfigLRNoError20s      //Step2. no error in 20s
        );

    User::After(KSecond*7);
    
    //2. Set PSY2 invisible while location request ongoing, the request shall
    //fallbck to PSY3
    SET_TIME
    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::After(KSecond*15); //after 15s disable PSY2
    ToggleModuleVisibleL(KIntGpsPsy1, EFalse);
    User::WaitForRequest(status);
    CHECK_TIME(16) //After PSY2 is disabled in 15 seconds, PSY3 is used. 
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(posInfo.ModuleId(), KNetworkPsy2, KWrongModuleIdReturned);
    ToggleModuleVisibleL(KIntGpsPsy1, ETrue);
    
    // cleanup
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CPsyInvisibleStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
