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
 @file MultipleClientsStep.cpp
 @internalTechnology
*/
#include "MultipleClientsStep.h"
#include "Te_defproxySuiteDefs.h"

CMultipleClientsStep::~CMultipleClientsStep()
/**
 * Destructor
 */
	{
	}

CMultipleClientsStep::CMultipleClientsStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KMultipleClientsStep);
	}

TVerdict CMultipleClientsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CMultipleClientsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(3, 
        KExtGpsPsy1, 1, 
        KIntGpsPsy1, 1,  
        KNetworkPsy2, 1);
        
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    
    TRequestStatus status2;
    TPositionInfo posInfo2;
    TPositionUpdateOptions options2;

    RPositioner positioner2;
    User::LeaveIfError(positioner2.Open(iServer));
    CleanupClosePushL(positioner2);
    User::LeaveIfError(positioner2.SetRequestor(
            CRequestor::ERequestorService,
            CRequestor::EFormatApplication,
            KTdDefProxyTestName));

    //1. Make location request from both PSYs. All should succeed from ExtGpsPsy1
    positioner2.NotifyPositionUpdate(posInfo2,status2);
    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    User::WaitForRequest(status2);
    CheckExpectedResult(status2.Int(), KErrNone, KWrongRequestResult);                
    CheckExpectedResult(posInfo2.ModuleId(), KExtGpsPsy1, KWrongModuleIdReturned);    \
    
    //2. When all PSYs returns error code, both PSYs shall 
    //get error code from the first PSY
    ConfigPsyL(KExtGpsPsy1, 1,
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );

    ConfigPsyL(KIntGpsPsy1, 1,
        KConfigLRErrGeneral1s           //KErrGeneral 1s
        );
    
    ConfigPsyL(KNetworkPsy2, 1,
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );
        
    ConfigPsyL(positioner2, KExtGpsPsy1, 1,
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );

    ConfigPsyL(positioner2, KIntGpsPsy1, 1,
        KConfigLRErrGeneral1s           //KErrGeneral 1s
        );
    
    ConfigPsyL(positioner2, KNetworkPsy2, 1,
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );

    InitPsyListInDefaultProxyL();
    
    iPositioner.NotifyPositionUpdate(posInfo, status);
    positioner2.NotifyPositionUpdate(posInfo2, status2);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNoMemory, KWrongRequestResult);                
    User::WaitForRequest(status2);
    CheckExpectedResult(status2.Int(), KErrNoMemory, KWrongRequestResult);                
    
    //3. When PSY1 returns error code to to client 1. Location request 
    //from PSY2 shall fallback imediately
    ConfigPsyL(KExtGpsPsy1, 1,
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );

    ConfigPsyL(KIntGpsPsy1, 1,
        KConfigLRErrGeneral1s           //KErrGeneral 1s
        );
    
    ConfigPsyL(KNetworkPsy2, 1,
        KConfigLRNoError1s            //No error 1s
        );
        
    ConfigPsyL(positioner2, KExtGpsPsy1, 1,
        KConfigLRNoError10s            //No error in 10s
        );

    ConfigPsyL(positioner2, KIntGpsPsy1, 1,
        KConfigLRNoError10s           //KErrGeneral 1s
        );
    
    ConfigPsyL(positioner2, KNetworkPsy2, 1,
        KConfigLRNoError1s            //KErrNoMemory 1s
        );
    
    InitPsyListInDefaultProxyL();
    User::After(KSecond * 7); //Delay after configuration
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KNetworkPsy2);
    INFO_PRINTF1(_L("1 3"));
    CHECK_TIME(3)
    
    //LR from second PSY
    positioner2.NotifyPositionUpdate(posInfo2, status2);
    User::WaitForRequest(status2);
    INFO_PRINTF1(_L("2 1"));
    CHECK_TIME(1) //PSY1 and PSY2 fallback imediately, LR completed from PSY3 in 1s
    CheckExpectedResult(status2.Int(), KErrNone, KWrongRequestResult);                
    CheckExpectedResult(posInfo2.ModuleId(), KNetworkPsy2, KWrongModuleIdReturned);
    
    //4. PSY state change beause of LR for client1 shall affect LR for client2 as well
    //
    ConfigPsyL(KExtGpsPsy1, 1,
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );

    ConfigPsyL(positioner2, KExtGpsPsy1, 1,
        KConfigLRNoError10s            //No error in 10s
        );
        
    InitPsyListInDefaultProxyL();
    User::After(KSecond * 7); //Delay after configuration

    SET_TIME
    positioner2.NotifyPositionUpdate(posInfo2, status2);
    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    INFO_PRINTF1(_L("3 1"));
    CHECK_TIME(1);
    User::WaitForRequest(status2);
    CheckExpectedResult(status2.Int(), KErrNone, KWrongRequestResult);                
    CheckExpectedResult(posInfo2.ModuleId(), KIntGpsPsy1, KWrongModuleIdReturned);  
    INFO_PRINTF1(_L("4 0"));
    CHECK_TIME(0);
    
    //5. Dynamic list change caused by PSY1 shall not affect PSY list orders of client2
    //if location request is already made.
    ConfigPsyL(KExtGpsPsy1, 1,
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );
    
    ConfigPsyL(KIntGpsPsy1, 2,
        KConfigLRNoError1s,             //No error in 10s
        KConfigLRErrNoMemory            //KErrNoMemory 1s
        );
        
    ConfigPsyL(positioner2, KIntGpsPsy1, 2,
        KConfigLRNoError1s,           //No error in 10s
        KConfigLRNoError20s            //No error in 10s
        );

    ConfigPsyL(positioner2, KExtGpsPsy1, 1,
        KConfigLRNoError45s            //No error in 45s
        );

    InitPsyListInDefaultProxyL();
    User::After(KSecond * 7); //Delay after configuration

    PositionRequestWithCheck(posInfo, KErrNone, KIntGpsPsy1);
    
    //Location request from client 2 shall be completed imediatelly, since 
    //IntGpsPsy shall be the first on the dynamic list
    SET_TIME
    positioner2.NotifyPositionUpdate(posInfo2, status2);
    User::WaitForRequest(status2);
    CheckExpectedResult(status2.Int(), KErrNone, KWrongRequestResult);                
    CheckExpectedResult(posInfo2.ModuleId(), KIntGpsPsy1, KWrongModuleIdReturned);  
    INFO_PRINTF1(_L("5 1"));
    CHECK_TIME(1);
    
    //Make location request from client 2
    positioner2.NotifyPositionUpdate(posInfo2, status2);
    SET_TIME
    PositionRequestWithCheck(posInfo, KErrNone, KExtGpsPsy1);
    INFO_PRINTF1(_L("6 1"));
    CHECK_TIME(1)
    User::WaitForRequest(status2);
    CheckExpectedResult(status2.Int(), KErrNone, KWrongRequestResult);                
    CheckExpectedResult(posInfo2.ModuleId(), KNetworkPsy2, KWrongModuleIdReturned);  
    INFO_PRINTF1(_L("7 0"));
    CHECK_TIME(0)
    

    CleanupStack::PopAndDestroy(&positioner2);
    //Cleanup
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CMultipleClientsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
