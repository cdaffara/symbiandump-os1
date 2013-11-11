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
 @file TwoInternalGpsPsyStep.cpp
 @internalTechnology
*/
#include "TwoInternalGpsPsyStep.h"
#include "Te_defproxySuiteDefs.h"

CTwoInternalGpsPsyStep::~CTwoInternalGpsPsyStep()
/**
 * Destructor
 */
	{
	}

CTwoInternalGpsPsyStep::CTwoInternalGpsPsyStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTwoInternalGpsPsyStep);
	}

TVerdict CTwoInternalGpsPsyStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTwoInternalGpsPsyStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;


    //1.
    InitPsySettingsL(3, 
        KIntGpsPsy1, 1,
        KIntGpsPsy2, 1,
        KNetworkPsy1, 1);
    
    //2. 
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(0)
    

    //3.
    InitPsySettingsL(3, 
        KIntGpsPsy1, 1,
        KIntGpsPsy2, 0,
        KNetworkPsy1, 0);
    
    //4. 
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(0)

    //5.
    InitPsySettingsL(3, 
        KIntGpsPsy1, 0,
        KIntGpsPsy2, 1,
        KNetworkPsy1, 0);
    
    //6. 
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy2);
    CHECK_TIME(0)

    //7.
    InitPsySettingsL(3, 
        KIntGpsPsy1, 1,
        KIntGpsPsy2, 1,
        KNetworkPsy1, 0);
    
    //8. 
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(0)
    
    //9.
    InitPsySettingsL(3, 
        KIntGpsPsy1, 0,
        KIntGpsPsy2, 1,
        KNetworkPsy1, 1);
    
    //10. 
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy2);
    CHECK_TIME(0)
    
    //Unload default proxy and all PSYs
    //CLOSE_AND_OPEN_POSITIONER
    
    
    // cleanup
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CTwoInternalGpsPsyStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
