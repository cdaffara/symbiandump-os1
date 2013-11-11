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
 @file ThreePsysStep.cpp
 @internalTechnology
*/
#include "ThreePsysStep.h"
#include "Te_defproxySuiteDefs.h"

CThreePsysStep::~CThreePsysStep()
/**
 * Destructor
 */
	{
	}

CThreePsysStep::CThreePsysStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KThreePsysStep);
	}

TVerdict CThreePsysStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CThreePsysStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(3, KNetworkPsy1, 1, KIntGpsPsy1, 1, KNetworkPsy2, 1);

    ConfigPsyL(KNetworkPsy1, 8,
        KConfigLRNoError,        //Step1. no error
        KConfigLRErrNoMemory,    //Step2. KErrNoMemory in 1s
        KConfigLRErrNoMemory,    //Step3. KErrNoMemory in 1s
        KConfigLRErrNoMemory,    //Step4. KErrNoMemory in 1s
        KConfigLRNoError20s,     //Step5. no error in 20 seconds
        KConfigLRNoError45s,     //Step6. no error in 45 seconds
        KConfigLRNoError45s,     //Step7. no error in 45 seconds
        KConfigLRNoError35s      //Step8. no error in 35 seconds
        );
        
    ConfigPsyL(KIntGpsPsy1, 7, 
        KConfigLRNoError,       //Step 2. no error
        KConfigLRErrGeneral1s,  //Step 3. KErrGeneral in 1s
        KConfigLRErrGeneral1s,  //Step 4. KErrGeneral in 1s
        KConfigLRNoError,       //Step 5. no error
        KConfigLRNoError35s,    //Step 6. no error
        KConfigLRNoError32s,    //Step 7. no error
        KConfigLRErrGeneral1s   //Step 8. KErrGeneral in 1s
        );

    ConfigPsyL(KNetworkPsy2, 5,
        KConfigLRNoError,        //Step3. no error
        KConfigLRErrGeneral1s,   //Step4. KErrGeneral in 1s
        KConfigLRNoError,        //Step6. no error
        KConfigLRNoError10s,     //Step7. no error in 10s
        KConfigLRNoError35s      //Step7. no error in 35s
        );

    User::After(KSecond*7); //Delay is needed after every time PSY is configured.
    
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    
    //1. When PSY1 is able to give a fix. Make location request, 
    //it shall be completed from IntGpsPsy1
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(1)
    
    //2. PSY1: error code. PSY2 is used
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(2)
    
    User::After(KSecond*7);
        
    //3. PSY1: error. PSY2 error. PSY3 is used.
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy2);
    CHECK_TIME(3)
    
    User::After(KSecond*7);

    //4. All PSY returns error code, error from PSY1 is used.
    SET_TIME
    iPositioner.NotifyPositionUpdate(posInfo,status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNoMemory, KWrongRequestResult);
    CHECK_TIME(3)

    User::After(KSecond*7);
    
    //5. PSY1 does not response, PSY2 is used
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(13) //timeshift for network1 is 12, PSY2 response time is 1s
    
    User::After(KSecond*7);
    
    //6. PSY1, PSY2 does not response, PSY3 is used
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy2);
    CHECK_TIME(12+30+1) //timeshift network1: 12, IntGpsPsy1:30, Network2 response 1
    
    User::After(KSecond*7);
    
    //7 PS1, PSY2 PSY3 does not response, then PSY2 returns a fix. Fix from
    //PSY2 will be used.
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(12+32) //timeshift network1: 12, IntGpsPsy1 response:32
    
    User::After(KSecond*7);

    //8 PSY1 does not response, PSY2 error code, PSY3 does not response. 
    //After a whiel fix from PSY1 is receivd, the this fix is used.
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(35) //PSY1 response time: 35s
    
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CThreePsysStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
