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
 @file AvailabilityFallbackStep.cpp
 @internalTechnology
*/
#include "availabilityfallbackstep.h"
#include "te_defproxysuitedefs.h"

CAvailabilityFallbackStep::~CAvailabilityFallbackStep()
/**
 * Destructor
 */
	{
	}

CAvailabilityFallbackStep::CAvailabilityFallbackStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KAvailabilityFallbackStep);
	}

TVerdict CAvailabilityFallbackStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CAvailabilityFallbackStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    //PSY order: Dummy1, Psy6, Dummy3
    StandardPrepareL();

    TRequestStatus status;
    TPositionInfo posInfo;

    // dummy1 must be disabled
    ToggleModuleL(KUidLcfPsy1, EFalse);

    // psy6 must have delay
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
    SetPsy6Delay(*genInfo, KDelay);

    // 1st
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // 2nd
    SetPsy6Delay(*genInfo, KDelay);
    iPositioner.NotifyPositionUpdate(*genInfo, status);

    User::After(KTinyDelay);
    ToggleModuleL(KUidLcfPsy6, EFalse);

    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(genInfo->ModuleId(), KUidLcfPsy3, KWrongModuleIdReturned);

    // 3rd
    SetPsy6Delay(*genInfo, KDelay);
    iPositioner.NotifyPositionUpdate(*genInfo, status);

    ToggleModuleL(KUidLcfPsy6, ETrue);

    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(genInfo->ModuleId(), KUidLcfPsy3, KWrongModuleIdReturned);

    User::After(KDelay);

    // 4th
    SetPsy6Delay(*genInfo, KDelay);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // 5th
    SetPsy6Delay(*genInfo, KDelay);
    iPositioner.NotifyPositionUpdate(*genInfo, status);

    ToggleModuleL(KUidLcfPsy6, EFalse);

    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(genInfo->ModuleId(), KUidLcfPsy3, KWrongModuleIdReturned);

    // 6th
    DisableAllModulesL();

    iPositioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNotFound, KWrongRequestResult);

    // 7th
    ToggleModuleL(KUidLcfPsy6, ETrue);
    PositionRequestWithCheck(posInfo, KErrNone, KUidLcfPsy6);

    // cleanup
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
	return TestStepResult();
	}



TVerdict CAvailabilityFallbackStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
