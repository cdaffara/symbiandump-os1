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
 @file CancelTrackingStep.cpp
 @internalTechnology
*/
#include "canceltrackingstep.h"
#include "te_defproxysuitedefs.h"

CCancelTrackingStep::~CCancelTrackingStep()
/**
 * Destructor
 */
	{
	}

CCancelTrackingStep::CCancelTrackingStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KCancelTrackingStep);
	}

TVerdict CCancelTrackingStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CCancelTrackingStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();

    // setup order : PSY6
    ToggleModuleL(KUidLcfPsy3, EFalse);
    ToggleModuleL(KUidLcfPsy1, EFalse);

    TRequestStatus status;
    TPositionInfo posInfo;

    const TTimeIntervalMicroSeconds KPSYDelay = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds KInterval = 3 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds32 KCancelIntervalDelay = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds32 KCancelPsyDelay = 4 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds KTimeOut  = 8 * KSecondsToMicro;

    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(KInterval);
    updateOptions.SetUpdateTimeOut(KTimeOut);
    if(KErrNone != iPositioner.SetUpdateOptions(updateOptions))
        {
        ERR_PRINTF1(KFailedStartTracking);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    // psy6 must delay
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    // warm up 1st tracking
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // 2nd request, cancel during interval
    iPositioner.NotifyPositionUpdate(*genInfo, status);
    User::After(KCancelIntervalDelay);
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrCancel, status.Int(), KFailedCancelInterval);

    // 3rd request, warm-up new tracking session
    updateOptions.SetUpdateInterval(0);
    if(KErrNone != iPositioner.SetUpdateOptions(updateOptions))
        {
        ERR_PRINTF1(KFailedStopTracking);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    updateOptions.SetUpdateInterval(KInterval);
    if(KErrNone != iPositioner.SetUpdateOptions(updateOptions))
        {
        ERR_PRINTF1(KFailedStartTracking2);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // 4th request, cancel during psy delay
    SetPsy6Delay(*genInfo, KPSYDelay);
    iPositioner.NotifyPositionUpdate(*genInfo, status);
    User::After(KCancelPsyDelay);
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrCancel, status.Int(), KFailedCancelPsyDelay);

    // cleanup
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
	return TestStepResult();
	}



TVerdict CCancelTrackingStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
