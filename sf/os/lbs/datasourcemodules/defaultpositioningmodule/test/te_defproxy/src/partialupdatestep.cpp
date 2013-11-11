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
 @file PartialUpdateStep.cpp
 @internalTechnology
*/
#include "PartialUpdateStep.h"
#include "Te_defproxySuiteDefs.h"

CPartialUpdateStep::~CPartialUpdateStep()
/**
 * Destructor
 */
	{
	}

CPartialUpdateStep::CPartialUpdateStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPartialUpdateStep);
	}

TVerdict CPartialUpdateStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CPartialUpdateStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(3, 
        KIntGpsPsy1, 1,  
        KExtGpsPsy1, 1, 
        KNetworkPsy2, 0);
        
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    
    //1. IntGpsPsy always returns partial update, DP shall fallback to
    //ExtGpsPsy1 after timeshift value
    ConfigPsyL(KIntGpsPsy1, 1,
        KConfigLRPartial 
        );
    TTime startTime;
    startTime.UniversalTime();
    for(;;)
        {
        iPositioner.NotifyPositionUpdate(posInfo, status);
        User::WaitForRequest(status);
        TTime now;
        now.UniversalTime();
        TTimeIntervalSeconds interval;
        now.SecondsFrom(startTime,interval);
        if(posInfo.ModuleId()==KExtGpsPsy1)
            {
            if(interval.Int()<30)
                {
                //This can't be true since the external GPS PSY checking time is
                //60 seconds
                ERR_PRINTF2(KLocationRequestSucceededTooSoon, KErrCorrupt);
                SetTestStepResult(EFail);
                return TestStepResult();
                }
            break;
            }
        if(interval.Int()>70)
            {
            //If the external GPS PSY is not used in 70 seconds the test failed
            ERR_PRINTF2(KLocationRequestStillNotCompleted, KErrGeneral);
            SetTestStepResult(EFail);
            return TestStepResult();
            }
        }
        
    //2. If PSY1 returns partial update location request to sencond PSY 
    //shall be canceled after cleanup timeout
    ConfigPsyL(KIntGpsPsy1, 3,
        KConfigStatusReady,
        KConfigLRPartial5s,
        KConfigLRPartial5s 
        );

    ConfigPsyL(KExtGpsPsy1, 2,
        KConfigLRNoError10s,
        KConfigLRNoError10s 
        );
    
    InitPsyListInDefaultProxyL();
    
    //The location request first made to PSY1. After 2 seconds, DP 
    //fallback to PSY2. 
    //the location request shall be completed from IntGpsPsy1
    PositionRequestWithCheck(posInfo, KPositionPartialUpdate, KIntGpsPsy1);
    
    //After 15 seconds make another location request.
    User::After(KSecond * 15);
    
    //It shall still be partial from the PSY1. 
    PositionRequestWithCheck(posInfo, KPositionPartialUpdate, KIntGpsPsy1);
    
    //3. If PSY2 give a fix before cleanup timeout, it shall be moved to 
    //the front of the list
    ConfigPsyL(KIntGpsPsy1, 2,
        KConfigLRPartial5s,
        KConfigLRPartial5s 
        );

    ConfigPsyL(KExtGpsPsy1, 1,
        KConfigLRNoError5s
        );
        
    User::After(KSecond * 7);
        
    InitPsyListInDefaultProxyL();
    
    PositionRequestWithCheck(posInfo, KPositionPartialUpdate, KIntGpsPsy1);
    
    User::After(KSecond * 55);//2s fallback, 5s PSY1 partial, 7s PSY2 fix, 10s cleanup timeout

    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    
    User::After(KSecond * 5);
    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    
    User::After(KSecond * 12);
    
    
//    Unclear test, removed pending doc lookup.
//    //4. If PSY2 gives a fullfix before cleanup timeout and there is location request 
//    //pending, this fix will be given to the client
//    
//    ConfigPsyL(KIntGpsPsy1, 2,
//        KConfigLRPartial5s,
//        KConfigLRPartial5s 
//        );
//
//    ConfigPsyL(KExtGpsPsy1, 1,
//        KConfigLRNoError5s
//        );
//    
//    //User::After(KSecond * 7);
//    
//    InitPsyListInDefaultProxyL();
//    
//    PositionRequestWithCheck(posInfo, KPositionPartialUpdate, KIntGpsPsy1);
//    
//    //User::After(KSecond * 1);//2s fallback, 5s PSY1 partial, 7s PSY2 fix, 10s cleanup timeout
//
//    SET_TIME
//    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
//    CHECK_TIME(0)
//    
//    //User::After(KSecond * 5);
//    //PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    
    // cleanup
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CPartialUpdateStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
