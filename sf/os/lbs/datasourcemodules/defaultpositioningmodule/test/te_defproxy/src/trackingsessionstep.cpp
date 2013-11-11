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
 @file TrackingSessionStep.cpp
 @internalTechnology
*/
#include "trackingsessionstep.h"
#include "te_defproxysuitedefs.h"

CTrackingSessionStep::~CTrackingSessionStep()
/**
 * Destructor
 */
	{
	}

CTrackingSessionStep::CTrackingSessionStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTrackingSessionStep);
	}

TVerdict CTrackingSessionStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTrackingSessionStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();

    const TInt KInterval = 1 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds KFirstRequestDelay = 200 * 1000; // 0.2 sec
    const TTimeIntervalMicroSeconds KNormalRequestDelay = 200 * 1000; // 0.2 sec
    const TInt KDiff = 200 * 1000; // 0.2 sec
    const TInt KIntervalError = 100 * 1000; // 0.1 sec

    TRequestStatus status;
    TWatch watch;
    TTime secondRequest, stopTracking;
    TInt interval;

    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    // setup : only PSY6
    ToggleModuleL(KUidLcfPsy1, EFalse);
    ToggleModuleL(KUidLcfPsy3, EFalse);
    
    User::After(KSecond * 7);
        
    InitPsyListInDefaultProxyL();

    // make one request to allow default proxy rebuild its database
    // this will guarantee that first request will be as fast as possible
    TPositionCourseInfo courseInfo;
    PositionRequestWithCheck(courseInfo, KErrArgument, KNullUid);

    // setup tracking session
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(KInterval);
    iPositioner.SetUpdateOptions(updateOptions);

    // 1st request
    // first request must be quick
    watch.Tick();
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);
    if(watch.ElapsedFromTick() > KFirstRequestDelay)
        {
        ERR_PRINTF1(KFirstDelayTooLong);
        SetTestStepResult(EFail);
        }
    
    // drop delay and make 2nd and 3rd requests
    genInfo->ClearRequestedFields();
    {
        watch.Tick();
        PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);
        secondRequest.UniversalTime();
        interval = (TInt) watch.ElapsedFromTick().Int64();
        if(Abs(interval - KInterval) > KIntervalError)
            {
            ERR_PRINTF1(KSecondDelayOutOfRange);
            SetTestStepResult(EFail);
            }
        
        watch.Tick();
        PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);
        interval = watch.ElapsedFromTick().Int64();
        if(Abs(interval - KInterval) > KIntervalError)
            {
            ERR_PRINTF1(KThirdDelayOutOfRange);
            SetTestStepResult(EFail);
            }
    }

    // stop tracking
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(0));
    iPositioner.SetUpdateOptions(updateOptions);
    stopTracking.UniversalTime();

    // psy6 must return event log
    genInfo->SetRequestedField(KPSY6FieldEventLog);
    watch.Tick();
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);
    if(watch.ElapsedFromTick() > KNormalRequestDelay)
        {
        ERR_PRINTF1(KNormalDelayTooBig);
        SetTestStepResult(EFail);
        }
    
    // Analyze event log from PSY6
    TQueryEvent* eventArray = GetPsy6EventLogL(*genInfo);

    TInt32 logSize;
    if(KErrNone != genInfo->GetValue(KPSY6FieldEventLogSize, logSize))
        {
        ERR_PRINTF1(KFailedReadLogsize);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    
    TUint expectedEvents[] = {EUpdate, 
                              ETrackingStart,
                              EUpdate,
                              EUpdate,
                              ETrackingStop, 
                              EUpdate};
    TUint numExpectedEvents = sizeof(expectedEvents) / sizeof(TUint);

    CheckExpectedResult(logSize, numExpectedEvents, KWrongEventNumberPSY6);

    // check event types
    for (TInt32 index = 0; index < Min(logSize, numExpectedEvents); index++)
        {
        TQueryEvent& event = *(eventArray + index);
        CheckExpectedResult(event.iEventType, expectedEvents[index], KUnexpectedEvent);
        }

    // check event times
    if(logSize != numExpectedEvents)
        {
        ERR_PRINTF1(KCannotCheckEventTimes);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    
    
    // ( ETrackingStart should have happenned right after first
    // request was completed )
    // Harley: Because of intelligent Default Proxy change, the tracking start
    // shall happen just before the second location request is issued to the PSY.
    TQueryEvent& event = *(eventArray + 1);
    if(Abs(secondRequest.MicroSecondsFrom(event.iTime).Int64()) > KDiff)
        {
        ERR_PRINTF1(KStartEventBeyondExpectendRange);
        SetTestStepResult(EFail);
        }
    
    // ETrackingStop should have happenned right after second
    // SetUpdateOptions
    event = *(eventArray + 4);

    if(Abs(stopTracking.MicroSecondsFrom(event.iTime).Int64()) > KDiff)
        {
        ERR_PRINTF1(KStopEventBeyondExpectendRange);
        SetTestStepResult(EFail);
        }
    // cleanup
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
	return TestStepResult();
	}



TVerdict CTrackingSessionStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
