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
 @file TwoPsysActiveStep.cpp
 @internalTechnology
*/
#include "TwoPsysActiveStep.h"
#include "Te_defproxySuiteDefs.h"

CTwoPsysActiveStep::~CTwoPsysActiveStep()
/**
 * Destructor
 */
	{
	}

CTwoPsysActiveStep::CTwoPsysActiveStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTwoPsysActiveStep);
	}

TVerdict CTwoPsysActiveStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTwoPsysActiveStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(2, KIntGpsPsy1, 1, KNetworkPsy1, 1);

    ConfigPsyL(KIntGpsPsy1, 8,
        KConfigStatusReady,     //Step 0. Set device status Ready 
        KConfigLRNoError,       //Step 1. no error
        KConfigLRErrGeneral1s,  //Step 2. KErrGeneral
        KConfigLRNoError35s,    //Step 3. no error in 35s
        KConfigLRErrGeneral1s,  //Step 4. KErrGeneral in 1 seconds
        KConfigLRNoError5s,    //Step 5. no error in 35s
        KConfigLRNoError5s,    //Step 6. no error in 35s
        KConfigLRErrGeneral5s  //Step 7. KErrGeneral in 35s.
        );

    ConfigPsyL(KNetworkPsy1, 6, 
        KConfigLRNoError,        //Step2. no error
        KConfigLRNoError,        //Step3. no error
        KConfigLRErrNoMemory,    //Step4. KErrNoMemory in 1 seconds
        KConfigLRNoError35s,     //Step5. no error in 35 seconds
        KConfigLRErrNoMemory,    //Step6. KErrNoMemory in 1 seconds
        KConfigLRNoError10s      //Step7. no error in 35s
        );
        
    User::After(KSecond*7); //Delay is needed after every time PSY is configured.
    
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    
    //1. When PSY1 is able to give a fix. Make location request, 
    //it shall be completed from IntGpsPsy1
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(1)
    
    //2. When PSY1 returns a error code, PSY2 shall return KErrNone
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(2)
    
    //Reset the priority of default proxy. Delay shall be used every time
    //PSY1 gives a error code
    User::After(KSecond*7); 
    
    //3. When PSY1 does not response anything within timeshift, PSY2 shall
    //return KErrNone
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(31) //2s is timeshfift value min(2*TTFF(1),10)+1

    User::After(KSecond*7); 
    
    //4. When both PSY returns error code, the error code from the
    //first PSY be returned
    //If the completion code is not KErrNone or KPositionPartialUpdate,
    //Location Server will not return module ID.
    SET_TIME
    iPositioner.NotifyPositionUpdate(posInfo,status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrGeneral, KWrongRequestResult);
    CHECK_TIME(2)
    
    //Reset the priority of default proxy. Delay shall be used every time
    //PSY1 gives error code
    User::After(KSecond*7); 
    
    //5. PSY1 does not give a fix within timeshift, PSY2 is tried. Before PSY2
    //returns a fix, PSY1 gives a fix, then fix from PSY1 shall be used.
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(5)
    
    //6. PSY1 does not give a fix within timeshift, PSY2 is tried. But PSY2 failed
    //to give a fix, default proxy will wait until PSY1 give a fix
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(5)
    
    //7. PSY1 does not give a fix within timeshift, PSY2 is tried. If PSY1 then 
    //give a error code, then fix from PSY2 will be used.
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(40) //Timeshift time 2s + response time 10s
    
    //Do everhing again when device status is Active
    
    ConfigPsyL(KIntGpsPsy1, 8,
        KConfigStatusActive,     //Step 0. Set device status Ready 
        KConfigLRNoError,       //Step 1. no error
        KConfigLRErrGeneral1s,  //Step 2. KErrGeneral
        KConfigLRNoError35s,    //Step 3. no error in 35s
        KConfigLRErrGeneral1s,  //Step 4. KErrGeneral in 1 seconds
        KConfigLRNoError5s,    //Step 5. no error in 35s
        KConfigLRNoError5s,    //Step 6. no error in 35s
        KConfigLRErrGeneral5s  //Step 7. KErrGeneral in 35s.
        );

    ConfigPsyL(KNetworkPsy1, 6, 
        KConfigLRNoError,        //Step2. no error
        KConfigLRNoError,        //Step3. no error
        KConfigLRErrNoMemory,    //Step4. KErrNoMemory in 1 seconds
        KConfigLRNoError35s,     //Step5. no error in 35 seconds
        KConfigLRErrNoMemory,    //Step6. KErrNoMemory in 1 seconds
        KConfigLRNoError10s      //Step7. no error in 35s
        );
        
    User::After(KSecond*7); //Delay is needed after every time PSY is configured.
    
    //1. When PSY1 is able to give a fix. Make location request, 
    //it shall be completed from IntGpsPsy1
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(1)
    
    //2. When PSY1 returns a error code, PSY2 shall return KErrNone
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(2)
    
    //Reset the priority of default proxy. Delay shall be used every time
    //PSY1 gives a error code
    User::After(KSecond*7); 
    
    //3. When PSY1 does not response anything within timeshift, PSY2 shall
    //return KErrNone
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(31) //2s is timeshfift value min(2*TTFF(1),10)+1

    User::After(KSecond*7); 
    
    //4. When both PSY returns error code, the error code from the
    //first PSY be returned
    //If the completion code is not KErrNone or KPositionPartialUpdate,
    //Location Server will not return module ID.
    SET_TIME
    iPositioner.NotifyPositionUpdate(posInfo,status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrGeneral, KWrongRequestResult);
    CHECK_TIME(2)
    
    //Reset the priority of default proxy. Delay shall be used every time
    //PSY1 gives error code
    User::After(KSecond*7); 
    
    //5. PSY1 does not give a fix within timeshift, PSY2 is tried. Before PSY2
    //returns a fix, PSY1 gives a fix, then fix from PSY1 shall be used.
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(5)
    
    //6. PSY1 does not give a fix within timeshift, PSY2 is tried. But PSY2 failed
    //to give a fix, default proxy will wait until PSY1 give a fix
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(5)
    
    //7. PSY1 does not give a fix within timeshift, PSY2 is tried. If PSY1 then 
    //give a error code, then fix from PSY2 will be used.
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(40) //Timeshift time 2s + response time 10s
    
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CTwoPsysActiveStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
