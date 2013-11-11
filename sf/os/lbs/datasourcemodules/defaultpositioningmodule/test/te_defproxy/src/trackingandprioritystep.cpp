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
 @file TrackingAndPriorityStep.cpp
 @internalTechnology
*/
#include "trackingandprioritystep.h"
#include "te_defproxysuitedefs.h"

CTrackingAndPriorityStep::~CTrackingAndPriorityStep()
/**
 * Destructor
 */
	{
	}

CTrackingAndPriorityStep::CTrackingAndPriorityStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTrackingAndPriorityStep);
	}

TVerdict CTrackingAndPriorityStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTrackingAndPriorityStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();

    // setup order : Dummy1, PSY6
    ToggleModuleL(KUidLcfPsy3, EFalse);

    TRequestStatus status;
    TPositionInfo posInfo;

    const TTimeIntervalMicroSeconds KInterval = 1 * KSecondsToMicro;
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(KInterval);
    if(KErrNone != iPositioner.SetUpdateOptions(updateOptions))
        {
        ERR_PRINTF1(KFailedSetOptions);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    
    // psy6 must return event log
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    // first 2 requests
    PositionRequestWithCheck(posInfo, KErrNone, KUidLcfPsy1);
    PositionRequestWithCheck(posInfo, KErrNone, KUidLcfPsy1);

    // change priority : PSY6, Dummy1
    SetModulePriorityL(KUidLcfPsy6, 0); // highest

    // next 2 requests
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // stop tracking
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(0));
    if(KErrNone != iPositioner.SetUpdateOptions(updateOptions))
        {
        ERR_PRINTF1(KFailedSetOptions);
        SetTestStepResult(EFail);
        return TestStepResult();
        }

    genInfo->SetRequestedField(KPSY6FieldEventLog);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // Analyze event log from PSY6
    TQueryEvent* eventArray = GetPsy6EventLogL(*genInfo);

    TInt32 logSize;
    if(KErrNone != genInfo->GetValue(KPSY6FieldEventLogSize, logSize))
        {
        ERR_PRINTF1(KFailedReadLogsize);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    
    TQueryEvent* events = GetPsy6EventLogL(*genInfo);

    CheckExpectedResult((events + 0)->iEventType, ETrackingStart, KWrongEventType);
    CheckExpectedResult((events + 1)->iEventType, EUpdate, KWrongEventType);
    CheckExpectedResult((events + 2)->iEventType, EUpdate, KWrongEventType);
    CheckExpectedResult((events + 3)->iEventType, ETrackingStop, KWrongEventType);
    CheckExpectedResult((events + 4)->iEventType, EUpdate, KWrongEventType);

    CheckExpectedResult(logSize, 5, KWrongAmountOfEvent);

    // cleanup
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
	return TestStepResult();
	}



TVerdict CTrackingAndPriorityStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
