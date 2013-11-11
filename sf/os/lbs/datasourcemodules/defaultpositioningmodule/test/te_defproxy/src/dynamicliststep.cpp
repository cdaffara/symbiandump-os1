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
 @file DynamicListStep.cpp
 @internalTechnology
*/
#include "DynamicListStep.h"
#include "Te_defproxySuiteDefs.h"

CDynamicListStep::~CDynamicListStep()
/**
 * Destructor
 */
	{
	}

CDynamicListStep::CDynamicListStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KDynamicListStep);
	}

TVerdict CDynamicListStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CDynamicListStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(6, KIntGpsPsy1, 1, KIntGpsPsy2, 1, 
        KExtGpsPsy1, 1, KExtGpsPsy2, 1,
        KNetworkPsy1, 1, KNetworkPsy2, 1);

    ConfigPsyL(KIntGpsPsy1, 1,
        KConfigLRErrNoMemory 
        );

    User::After(KSecond*7);
    
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;


    //1. Dynlist: IntGPS1, ExtGps1, ExtGps2, NetworkGps1, NetworkGps2
    //Only one internal GPS PSY shall be used by Default Proxy. LR shall
    //be completed by ExtGps1
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    CHECK_TIME(1)
    //2. Dynlist: ExtGps1, IntGps1, ExtGps2, NetworkGps1, Network Gps2
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    CHECK_TIME(0)
    
    //3. After default proxy is unloaded, the working GPS psy shall still
    //be valid
    //Dynlist: ExtGps1, IntGps1, ExtGps2, Network1, Network2
    CLOSE_AND_OPEN_POSITIONER
    
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    CHECK_TIME(0)
    
    //4. When ExtGps1 IntGps1 and ExtGps2 returns error code, Default Proxy fallback
    //to Network1. Even when network 1 is able to give a fix, it will not be moved to the 
    //top of the list. 
    ConfigPsyL(KIntGpsPsy1, 3,
        KConfigLRNoError45s,       //Fix in 45 seconds
        KConfigLRNoError45s,       //Fix in 45 seconds
        KConfigLRNoError45s        //Fix in 45 seconds
        );
    ConfigPsyL(KExtGpsPsy1, 3,
        KConfigLRErrGeneral1s,     //Error in 1s
        //KConfigLRErrGeneral1s,      //error
        KConfigLRNoError45s,       //Fix in 45 seconds
        KConfigLRErrGeneral1s      //error
        );
    ConfigPsyL(KExtGpsPsy2, 3,
        KConfigLRErrGeneral1s,     //Error in 1s
        KConfigLRErrGeneral1s,     //Error in 1s
        KConfigLRErrGeneral1s      //Error in 1s
        );
    ConfigPsyL(KNetworkPsy1, 3,
        KConfigLRNoError,          //Fix in 1s
        KConfigLRNoError,          //Fix in 1s
        KConfigLRErrGeneral1s      //Error in 1s
        );
    ConfigPsyL(KNetworkPsy2, 1,
        KConfigLRErrGeneral1s      //Error in 1s
        );
    
    //These configuration request changes PSY state and
    //dynamic list. By changing module status, we
    //update the dynamic list again.
    User::After(KSecond*7);
    ToggleModuleL(KLeavingPsyUid, ETrue);
    ToggleModuleL(KLeavingPsyUid, EFalse);
    
    
    //list: IntGps1, ExtGps1, ExtGps2, Network1, Network2
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(30+1+1+1)
    
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(0+0+0+1) //when PSY gives error code, Default 
                        //Proxy will start fallback imediately

    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(45) //All other PSY gives error code, fix from PSY1 will be used.
        
    
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CDynamicListStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
