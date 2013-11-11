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
 @file HGenericInfoStep.cpp
 @internalTechnology
*/
#include "HGenericInfoStep.h"
#include "Te_defproxySuiteDefs.h"

CHGenericInfoStep::~CHGenericInfoStep()
/**
 * Destructor
 */
	{
	}

CHGenericInfoStep::CHGenericInfoStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KHGenericInfoStep);
	}

TVerdict CHGenericInfoStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHGenericInfoStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    StandardPrepareL();
    InitPsySettingsL(2, 
        KExtGpsPsy1, 1, 
        KIntGpsPsy1, 1);
        
    TRequestStatus status;
    TPositionInfo posInfo;
    TPositionUpdateOptions options;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    //Set requested data
    genInfo->SetRequestedField(EPositionFieldComment);
    genInfo->SetRequestedField(EPositionFieldHorizontalSpeed);
    genInfo->SetRequestedField(EPositionFieldMediaLinks);
    genInfo->SetRequestedField(EPositionFieldMediaLinksStart);
    genInfo->SetRequestedField(EPositionFieldSatelliteNumInView);
    genInfo->SetRequestedField(EPositionFieldSatelliteTime);
    //These two are not supported by the PSY
    genInfo->SetRequestedField(EPositionFieldTrueCourse);
    genInfo->SetRequestedField(EPositionFieldStreet);
    

    //1. When PSY returns KErrPositionBufferOverflow, it shall be returned 
    //to the client
    ConfigPsyL(KExtGpsPsy1, 1,
        KConfigLRErrBufferOverflow            //KErrPositionBufferOverflow 1s
        );
        
    //In error case, the module ID is not set by Location Server
    PositionRequestWithCheck(*genInfo, KErrPositionBufferOverflow, KNullUid);
    
    //2. PSY2 return a fix which is not requested at different time
    ConfigPsyL(KExtGpsPsy1, 3,
        KConfigStatusActive,          //Status active
        KConfigLRPartial5s,           //Partial in 5s
        KConfigLRNoError45s
        );
        
    ConfigPsyL(KIntGpsPsy1, 1,
        KConfigLRHGeneric10s          //Status active
        );

    InitPsyListInDefaultProxyL();
    User::After(KSecond * 7); //Delay after configuration
    
    SET_TIME
    PositionRequestWithCheck(*genInfo, KPositionPartialUpdate, KExtGpsPsy1);
    CHECK_TIME(5)
    
    PositionRequestWithCheck(*genInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(35) //timeshift 2s + reponse time 10s - first request(5s)
    
    //3. Check satellite info
    ConfigPsyL(KExtGpsPsy1, 3,
        KConfigStatusActive,          //Status active
        KConfigLRPartial5s,           //Partial in 5s
        KConfigLRNoError45s
        );
        
    ConfigPsyL(KIntGpsPsy1, 1,
        KConfigLRNoError10s          //Status active
        );

    InitPsyListInDefaultProxyL();
    User::After(KSecond * 7); //Delay after configuration
    
    TPositionSatelliteInfo satInfo;
    
    SET_TIME
    PositionRequestWithCheck(satInfo, KPositionPartialUpdate, KExtGpsPsy1);
    CHECK_TIME(5)
    
    PositionRequestWithCheck(satInfo, KErrNone, KIntGpsPsy1);
    CHECK_TIME(35) //timeshift 2s + reponse time 10s - first request(5s)
    
    
    
    CleanupStack::PopAndDestroy(genInfo);
    StandardCleanup();
    return TestStepResult();
	}



TVerdict CHGenericInfoStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
