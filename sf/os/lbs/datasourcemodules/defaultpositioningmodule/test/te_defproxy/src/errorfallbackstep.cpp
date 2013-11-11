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
 @file ErrorFallbackStep.cpp
 @internalTechnology
*/
#include "errorfallbackstep.h"
#include "te_defproxysuitedefs.h"

CErrorFallbackStep::~CErrorFallbackStep()
/**
 * Destructor
 */
	{
	}

CErrorFallbackStep::CErrorFallbackStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KErrorFallbackStep);
	}

TVerdict CErrorFallbackStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CErrorFallbackStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();

    TRequestStatus status;

    // psy6 must return a code
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    // 1. KPositionPartialUpdate & Dummy 3
    ToggleModuleL(KUidLcfPsy1, EFalse); // don't use Dummy1 this time

    // - list: PSY6, Dummy3            -> KPositionPartialUpdate from PSY6
    SetPsy6Result(*genInfo, KPositionPartialUpdate);
    PositionRequestWithCheck(*genInfo, KPositionPartialUpdate, KUidLcfPsy6);

    // - list: Dummy3, PSY6            -> KErrNone from Dummy3
    SetModulePriorityL(KUidLcfPsy3, 0);
    SetPsy6Result(*genInfo, KPositionPartialUpdate);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy3);

    // 2. KPositionQualityLoss & Dummy 1
    ToggleModuleL(KUidLcfPsy3, EFalse); // don't use Dummy3 this time

    // - list: PSY6                    -> KPositionQualityLoss from Undefined
    genInfo->ClearPositionData();
    SetPsy6Result(*genInfo, KPositionQualityLoss);
    iPositioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KPositionQualityLoss, KWrongRequestResult);
    CheckExpectedResult(genInfo->ModuleId(), KNullUid, KWrongModuleIdReturned);

    // - list: PSY6, Dummy1            -> KErrNone from Dummy1
    SetModulePriorityL(KUidLcfPsy6, 0);
    SetupModuleL(KUidLcfPsy1, ETrue, 1);  // now use Dummy1
    SetPsy6Result(*genInfo, KPositionQualityLoss);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy1);

    // - list: Dummy1, PSY6            -> KErrNone from Dummy1
    SetModulePriorityL(KUidLcfPsy1, 0);
    SetPsy6Result(*genInfo, KPositionQualityLoss);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy1);

    // 3. All modules and PSY6 still Loses Quality
    ToggleModuleL(KUidLcfPsy3, ETrue);

    // - list: Dummy1, Dummy3, PSY6    -> KErrNone from Dummy1
    SetModulePriorityL(KUidLcfPsy1, 0);
    SetModulePriorityL(KUidLcfPsy3, 1);
    SetPsy6Result(*genInfo, KPositionQualityLoss);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy1);

    // - list: Dummy1, PSY6, Dummy3    -> KErrNone from Dummy1
    SetModulePriorityL(KUidLcfPsy6, 1);
    SetPsy6Result(*genInfo, KPositionQualityLoss);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy1);

    // - list: PSY6, Dummy3, Dummy1    -> KErrNone from Dummy3
    SetModulePriorityL(KUidLcfPsy6, 0);
    SetModulePriorityL(KUidLcfPsy3, 1);
    SetPsy6Result(*genInfo, KPositionQualityLoss);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy3);

    // - list: PSY6, Dummy1, Dummy3    -> KErrNone from Dummy1
    SetModulePriorityL(KUidLcfPsy1, 1);
    SetPsy6Result(*genInfo, KPositionQualityLoss);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy1);

    // cleanup
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
	return TestStepResult();
	}



TVerdict CErrorFallbackStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
