// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



//  INCLUDES
#include "ctlbsclientpostp4706.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4706::CT_LbsClientPosTp4706(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP4706 - Default Proxy fallback-partial update2");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4706::~CT_LbsClientPosTp4706()
    {
    }

// ---------------------------------------------------------
// CPosTp4706::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4706::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CPosTp4706::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4706::StartL()
    {
    ConnectL();

    SetupProxyPSYsL();

    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP4706");
    iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KServiceName);

    // Issue a pre position request
    TInt request=100;
    TPositionInfo posInfo;
    RequestL(posInfo, request, KEspectedErrorCodePSY1);
    
    //
	// Request 1
	//
    // Check that partial update is not supported    
    TPositionUpdateOptions updateOptions;
    updateOptions.SetAcceptPartialUpdates(ETrue);
    iPositioner.SetUpdateOptions(updateOptions);
         
    request = 4706;
    RequestL(posInfo, request, KEspectedErrorCodePSY2);  
      
    VerifyPositionFromL(posInfo, iUidPartialUpdate2);
    VerifyRequestTimeLessThanL(5001000);
    
    VerifyPositionL(posInfo, 55, 65, 75);
    }
 
// ---------------------------------------------------------
// CPosTp4706::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4706::SetupProxyPSYsL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    
    CPosModuleIdList* prioList = db->ModuleIdListLC();
	
    // Disable all PSY:s except PSYs used by this test case
    
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] != iUidTestPsyPartialUpdate &&
            (*prioList)[i] != iUidPartialUpdate2)
            {
            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
            }
        }
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestPsyPartialUpdate, *moduleUpdate);
    db->UpdateModuleL(iUidPartialUpdate2, *moduleUpdate);
    
    db->SetModulePriorityL(iUidTestPsyPartialUpdate, 0);
    db->SetModulePriorityL(iUidPartialUpdate2, 1);
    
    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    } 

//  End of File
