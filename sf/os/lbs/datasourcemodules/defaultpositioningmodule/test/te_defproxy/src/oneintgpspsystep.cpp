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
 @file OneIntGpsPsyStep.cpp
 @internalTechnology
*/
#include "OneIntGpsPsyStep.h"
#include "Te_defproxySuiteDefs.h"

COneIntGpsPsyStep::~COneIntGpsPsyStep()
/**
 * Destructor
 */
	{
	}

COneIntGpsPsyStep::COneIntGpsPsyStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KOneIntGpsPsyStep);
	}

TVerdict COneIntGpsPsyStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict COneIntGpsPsyStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(1, KIntGpsPsy1, 1);
    
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;

    //1. Make location request when PSY response imediately
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(0)
    
    //Unload default proxy and all PSYs
    CLOSE_AND_OPEN_POSITIONER
    
    
    //2. Make location request when PSY response is 10s
    ConfigPsyL(KIntGpsPsy1, 1, KConfigLRNoError10s);
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(10)
    
    //Unload default proxy and all PSYs
    CLOSE_AND_OPEN_POSITIONER    
    
    //3. Make location request when PSY response is 35s
    ConfigPsyL(KIntGpsPsy1, 2, 
        KConfigLRNoError35s, 
        KConfigStatusReady);
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(35)
    
    User::After(KSmallDelay);
    //PSY1 will be in device ready state
    
    //4. Make location request when PSY response imediatley
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(0)
    
    //5. Make location request when PSY response is 1s
    ConfigPsyL(KIntGpsPsy1, 1, KConfigLRNoError1s);
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(1)
    
    //6. Make location request when PSY response is 5s
    ConfigPsyL(KIntGpsPsy1, 1, KConfigLRNoError5s);
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(5)
    
    //Set device status to active
    ConfigPsyL(KIntGpsPsy1, 1, KConfigStatusActive);
    
    //7. Make location request when PSY response imediatley
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(0)
    
    //8. Make location request when PSY response is 1s
    ConfigPsyL(KIntGpsPsy1, 1, KConfigLRNoError1s);
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(1)
    
    //9. Make location request when PSY response is 5s
    ConfigPsyL(KIntGpsPsy1, 1, KConfigLRNoError5s);
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(5)
    
    // cleanup
    StandardCleanup();
    return TestStepResult();
	}



TVerdict COneIntGpsPsyStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
