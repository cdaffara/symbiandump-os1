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
 @file PriorityChangesStep.cpp
 @internalTechnology
*/
#include "prioritychangesstep.h"
#include "te_defproxysuitedefs.h"

CPriorityChangesStep::~CPriorityChangesStep()
/**
 * Destructor
 */
	{
	}

CPriorityChangesStep::CPriorityChangesStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPriorityChangesStep);
	}

TVerdict CPriorityChangesStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CPriorityChangesStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();

    TRequestStatus status;

    // psy6 must have delay
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    // 1st request
    SetPsy6Delay(*genInfo, KDelay);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy1);

    // 2nd request
    SetModulePriorityL(KUidLcfPsy6, 0); // highest

    SetPsy6Delay(*genInfo, KDelay);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy6);

    // 3rd request
    SetPsy6Delay(*genInfo, KDelay);
    iPositioner.NotifyPositionUpdate(*genInfo, status); // genInfo to PSY6
    User::After(KDelay);

    SetModulePriorityL(KUidLcfPsy3, 0);

    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(genInfo->ModuleId(), KUidLcfPsy6, KWrongModuleIdReturned);

    // 4th request
    SetPsy6Delay(*genInfo, KDelay);
    PositionRequestWithCheck(*genInfo, KErrNone, KUidLcfPsy3);

    // cleanup
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
	return TestStepResult();
	}



TVerdict CPriorityChangesStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
