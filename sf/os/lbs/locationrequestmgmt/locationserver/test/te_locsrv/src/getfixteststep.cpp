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
 @file GetFixTestStep.cpp
 @internalTechnology
*/
#include "getfixteststep.h"
#include "te_locsrvsuitedefs.h"

CGetFixTestStep::~CGetFixTestStep()
/**
 * Destructor
 */
	{
	}

CGetFixTestStep::CGetFixTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KGetFixTestStep);
	}

TVerdict CGetFixTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_locsrvSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

extern TInt SetAuthenticRequestor(RPositioner& aPositioner);

TVerdict CGetFixTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    
    RPositionServer server;
    if(KErrNone != server.Connect())
        {
        ERR_PRINTF1(KFailedConnectServer);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(server);

    RPositioner positioner;
    if(KErrNone != positioner.Open(server))
        {
        ERR_PRINTF1(KFailedOpenPositioner);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    TPositionInfo posInfo;
    TRequestStatus reqStatus;

    //
    // 1. No modules, no last known fix

    DisableAllModulesL();

    // ask for a fix
    positioner.NotifyPositionUpdate(posInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult(reqStatus.Int(), KErrNotFound, KWrongResult);

    // ask for last known pos
    TRequestStatus dbclear;
    server.EmptyLastKnownPositionStore(dbclear);
    User::WaitForRequest(dbclear);
    positioner.GetLastKnownPosition(posInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    if (reqStatus.Int() != KErrUnknown)
        {
   		ERR_PRINTF2(_L("GetLastKnownPosition returned %d"),reqStatus.Int());
        SetTestStepResult(EFail);
        }
    
    //
    // 2. Some modules, last known fix exists

    // first, create LastKnownPos
    const TUid KPsy1Uid = {KLcfPsy1UidValue};
    TogglePsyL(KPsy1Uid, ETrue);

    positioner.NotifyPositionUpdate(posInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult(reqStatus.Int(), KErrNone, KWrongResult);

    // now last known pos exists, ask for it
    positioner.GetLastKnownPosition(posInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult(reqStatus.Int(), KErrNone, KWrongResult);
    
    //
    // 3. No modules, last known fix exists
    
    TogglePsyL(KPsy1Uid, EFalse);

    // ask for a fix
    positioner.NotifyPositionUpdate(posInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult(reqStatus.Int(), KErrNotFound, KWrongResult);

    // now last known pos exists, ask for it
    positioner.GetLastKnownPosition(posInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult(reqStatus.Int(), KErrNone, KWrongResult);

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
	}



TVerdict CGetFixTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
