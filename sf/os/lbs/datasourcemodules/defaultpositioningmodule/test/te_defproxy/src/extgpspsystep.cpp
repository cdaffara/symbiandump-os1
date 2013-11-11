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
 @file ExtGpsPsyStep.cpp
 @internalTechnology
*/
#include "ExtGpsPsyStep.h"
#include "Te_defproxySuiteDefs.h"

CExtGpsPsyStep::~CExtGpsPsyStep()
/**
 * Destructor
 */
	{
	}

CExtGpsPsyStep::CExtGpsPsyStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KExtGpsPsyStep);
	}

TVerdict CExtGpsPsyStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CExtGpsPsyStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(6, KIntGpsPsy1, 1, KIntGpsPsy2, 1, 
        KNetworkPsy1, 0, KExtGpsPsy1, 1, KExtGpsPsy2, 0,
        KNetworkPsy2, 1);
        
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;

    //1. If a internal GPS PSY always return a fix. After certain time,
    //Default porxy shall try external GPS PSY. And if it can give a fix,
    //after that external GPS PSY shall be used.
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
            if(interval.Int()<60)
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

    //Next location request shall be completed from External GPS PSY
    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    

    //2. If there is no location request on going, default proxy shall not check 
    //fix state of external GPS PSY.
    ConfigPsyL(KExtGpsPsy1, 1,
        KConfigLRErrNoMemory 
        );
        
    //Move Int GPS1 to the top of the list
    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);
    //Check location request is not completed from ExtGpsPsy1
    if(posInfo.ModuleId()==KExtGpsPsy1)
        {
        ERR_PRINTF2(KLocationRequestStillCompleted, KErrCorrupt);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    

    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);
    //Check location request is not completed from ExtGpsPsy1
    if(posInfo.ModuleId()==KExtGpsPsy1)
        {
        ERR_PRINTF2(KLocationRequestStillCompleted, KErrCorrupt);
        SetTestStepResult(EFail);
        return TestStepResult();
        }

    StandardCleanup();
    return TestStepResult();
	}



TVerdict CExtGpsPsyStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
