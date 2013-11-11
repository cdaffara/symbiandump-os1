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
 @file BasicUsageStep.cpp
 @internalTechnology
*/
#include "basicusagestep.h"
#include "te_defproxysuitedefs.h"

CBasicUsageStep::~CBasicUsageStep()
/**
 * Destructor
 */
	{
	}

CBasicUsageStep::CBasicUsageStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KBasicUsageStep);
	}

TVerdict CBasicUsageStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CBasicUsageStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    // psy6 will be instructed
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    DisableAllModulesL();

    // 0. No PSYs
    PositionRequestWithCheck(posInfo, KErrNotFound, KNullUid);
    
    // only PSY6 from now on
    ToggleModuleL(KUidLcfPsy6, ETrue);

    // 1. Request and Cancel
    genInfo->SetRequestedField(KPSY6FieldDelayProcessing);
    genInfo->SetValue(KPSY6FieldDelayProcessing,
        TTimeIntervalMicroSeconds(KDelay));

    iPositioner.NotifyPositionUpdate(*genInfo, iStatus);
    User::After(KSmallDelay);
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    User::WaitForRequest(iStatus);
    CheckExpectedResult(iStatus.Int(), KErrCancel, KRequestNotCancelled);

    // check events
    genInfo->ClearRequestedFields();
    genInfo->SetRequestedField(KPSY6FieldEventLog);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    TInt32 logSize;
    if(KErrNone != genInfo->GetValue(KPSY6FieldEventLogSize, logSize))
        {
        ERR_PRINTF1(KFailedReadLogsize);
        CleanupStack::PopAndDestroy(genInfo);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    if (3 == logSize) // update, cancel, update
    {
        TQueryEvent* eventArray = GetPsy6EventLogL(*genInfo);
        CheckExpectedResult((eventArray)->iEventType, EUpdate, KBadFirstEventPSY6);
        CheckExpectedResult((eventArray + 1)->iEventType, ECancel, KBadSecondEventPSY6);
        CheckExpectedResult((eventArray + 2)->iEventType, EUpdate, KBadThirdEventPSY6);
    }
    else
    {
        CheckExpectedResult(logSize, 3, KWrongEventNumberPSY6);
    }

    // 2. Check bad Uid
     const  TInt32 wrongUid = 0x12345678;
    genInfo->ClearRequestedFields();
    {
        genInfo->SetRequestedField(KPSY6FieldRequestedUid);
        genInfo->SetValue(KPSY6FieldRequestedUid, wrongUid);
        iPositioner.NotifyPositionUpdate(*genInfo, iStatus);
        User::WaitForRequest(iStatus);
        CheckExpectedResult(iStatus.Int(), KErrGeneral, KWrongUidAccepted);

        genInfo->SetRequestedField(KPSY6FieldRequestedUid);
        genInfo->SetValue(KPSY6FieldRequestedUid, (TInt32) KNullUid.iUid);
        iPositioner.NotifyPositionUpdate(*genInfo, iStatus);
        User::WaitForRequest(iStatus);
        CheckExpectedResult(iStatus.Int(), KErrGeneral, KWrongNullUidAccepted);
    }
    genInfo->ClearRequestedFields();

    // 3. Check specific results

    // KErrPositionBufferOverflow
    TInt32 result = KErrPositionBufferOverflow;
    SetPsy6Result(*genInfo, result);
    PositionRequestWithCheck(*genInfo, result, KUidLcfPsy6);

    // KErrPositionPartialUpdate
    result = KPositionPartialUpdate;

    // if allowed
    options.SetAcceptPartialUpdates(ETrue);
    iPositioner.SetUpdateOptions(options);
    SetPsy6Result(*genInfo, result);
    PositionRequestWithCheck(*genInfo, result, KUidLcfPsy6);

    // if not allowed
    options.SetAcceptPartialUpdates(EFalse);
    iPositioner.SetUpdateOptions(options);
    SetPsy6Result(*genInfo, result);
    PositionRequestWithCheck(*genInfo, result, KUidLcfPsy6);

    // 4. This is very special case:
    // If a PSY is unloaded due to settings changes and loaded again
    // within tracking session, it should be informed about tracking session
    // via StartTrackingL method.
    // Test also what happens if this method leaves - leave is ignored

    // order : Psy6, Dummy3
    SetupModuleL(KUidLcfPsy6, ETrue, 0);
    SetupModuleL(KUidLcfPsy3, ETrue, 1);

    const TTimeIntervalMicroSeconds KInterval = 1 * KSecondsToMicro;
    options.SetUpdateInterval(KInterval);
    iPositioner.SetUpdateOptions(options);

    // 4.1 StartTrackingL may leave

    genInfo->ClearRequestedFields();

    // 2 requests to "warm up" tracking session
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // disable PSY6
    ToggleModuleL(KUidLcfPsy6, EFalse);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy3);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy3);

    // reenable PSY6
    ToggleModuleL(KUidLcfPsy6, ETrue);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6); // tracking is informed here just before request

    options.SetUpdateInterval(0);
    iPositioner.SetUpdateOptions(options); // stop tracking is called here

    // get event log
    genInfo->SetRequestedField(KPSY6FieldEventLog);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // check log
    TQueryEvent* events = GetPsy6EventLogL(*genInfo);
    if(KErrNone != genInfo->GetValue(KPSY6FieldEventLogSize, logSize))
        {
        ERR_PRINTF1(KFailedReadLogsize);
        CleanupStack::PopAndDestroy(genInfo);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    
    CheckExpectedResult(events->iEventType, ETrackingStart, KWrongEventType);
    CheckExpectedResult((events + 1)->iEventType, EUpdate, KWrongEventType);
    CheckExpectedResult((events + 2)->iEventType, ETrackingStop, KWrongEventType);
    CheckExpectedResult((events + 3)->iEventType, EUpdate, KWrongEventType);

    CheckExpectedResult(logSize, 4, KWrongAmountOfEvent);
	

    // 4.2 disable and renable PSY6 again, and program to leave on StartTrackingL,
    // request should still succeed, StartTrackingL/StopTracking not called
/* This part does not work, because StartTrackingL will be called by DefProxy
   before issuing location request

    ToggleModuleL(KUidLcfPsy6, EFalse);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy3)

    options.SetUpdateInterval(KInterval);
    iPositioner.SetUpdateOptions(options);

    // 1 requests to "warm up" tracking session
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy3)

    ToggleModuleL(KUidLcfPsy6, ETrue);

    genInfo->ClearRequestedFields();
    genInfo->SetRequestedField(KPSY6FieldLeaveOnStartTracking);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6) // starttracking called before request and leaves

    // stop tracking and check log, StopTracking is not called
    options.SetUpdateInterval(0);
    iPositioner.SetUpdateOptions(options);

    genInfo->ClearRequestedFields();
    genInfo->SetRequestedField(KPSY6FieldEventLog);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6)

    // check log
    events = GetPsy6EventLogL(*genInfo);
    LEAVE(KErrNone != genInfo->GetValue(KPSY6FieldEventLogSize, logSize), "Failed to read logsize");

    CHECK_EQUAL(events->iEventType, ETrackingStart, KWrongEventType);
    CHECK_EQUAL((events + 1)->iEventType, EUpdate, KWrongEventType);
    CHECK_EQUAL((events + 2)->iEventType, EUpdate, KWrongEventType);

    CHECK_EQUAL(logSize, 3, KWrongAmountOfEvent);
*/

	
    // 5. Check Leaving PSY
    // KLeavingPsy leaves on construction, DefProxy should switch to another PSY
    DisableAllModulesL();
    SetupModuleL(KLeavingPsy, ETrue, 0);
    PositionRequestWithCheck(posInfo, KErrGeneral, KNullUid);

    SetupModuleL(KUidLcfPsy6, ETrue, 1);
    PositionRequestWithCheck(posInfo, KErrNone, KUidLcfPsy6);

    // cleanup
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CBasicUsageStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
