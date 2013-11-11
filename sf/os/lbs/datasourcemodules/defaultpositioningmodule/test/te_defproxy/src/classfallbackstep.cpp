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
 @file ClassFallbackStep.cpp
 @internalTechnology
*/
#include "classfallbackstep.h"
#include "te_defproxysuitedefs.h"

CClassFallbackStep::~CClassFallbackStep()
/**
 * Destructor
 */
	{
	}

CClassFallbackStep::CClassFallbackStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KClassFallbackStep);
	}

TVerdict CClassFallbackStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CClassFallbackStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();

    TRequestStatus status;
    TPositionSatelliteInfo posSatInfo;

    // all enabled

    iPositioner.NotifyPositionUpdate(posSatInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNone, KFailedPositionRequest);
    CheckExpectedResult(posSatInfo.ModuleId(), KUidLcfPsy3, KWrongModuleIdReturned);

    // disable the only capable - Dummy3

    ToggleModuleL(KUidLcfPsy3, EFalse);
    User::After(KDelay);

    iPositioner.NotifyPositionUpdate(posSatInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrArgument, KWrongRequestResult);

    // disable all
    DisableAllModulesL();
    User::After(KDelay);

    iPositioner.NotifyPositionUpdate(posSatInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNotFound, KWrongRequestResult);

    StandardCleanup();
	return TestStepResult();
	}



TVerdict CClassFallbackStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
