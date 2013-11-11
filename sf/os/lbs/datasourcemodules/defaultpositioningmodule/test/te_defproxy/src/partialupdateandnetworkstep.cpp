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
 @file PartialUpdateAndNetworkStep.cpp
 @internalTechnology
*/
#include "PartialUpdateAndNetworkStep.h"
#include "Te_defproxySuiteDefs.h"

CPartialUpdateAndNetworkStep::~CPartialUpdateAndNetworkStep()
/**
 * Destructor
 */
	{
	}

CPartialUpdateAndNetworkStep::CPartialUpdateAndNetworkStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPartialUpdateAndNetworkStep);
	}

TVerdict CPartialUpdateAndNetworkStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CPartialUpdateAndNetworkStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(2, 
        KIntGpsPsy1, 1,  
        KNetworkPsy1, 1);
        
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
    
    //1: If PSY returns always partial update, location request will
    //be forwared to PSY2. Because of PSY2 is a network PSY. Before PSY2 
    //response the fix, no more partial update will be received from 
    //PSY1 any more
    ConfigPsyL(KIntGpsPsy1, 1,
        KConfigLRPartial  //Alwyas partial update
        );
        
    ConfigPsyL(KNetworkPsy1, 1,
        KConfigLRNoError10s  //Returns fix after 10s
        );
    User::After(KSecond*7);
    
    //First 30 seconds only partial update
    SET_TIME
    for ( TInt i = 0; i<27; i++ )
        {
        PositionRequestWithCheck(*genInfo, KPositionPartialUpdate, KIntGpsPsy1);
        CHECK_TIME(1)
        }

    User::After(KSecond*4); //wait 4 seconds

    SET_TIME
    PositionRequestWithCheck(*genInfo, KErrNone, KNetworkPsy1);
    CHECK_TIME(10) //PSY2 response time 10s
    
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CPartialUpdateAndNetworkStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
