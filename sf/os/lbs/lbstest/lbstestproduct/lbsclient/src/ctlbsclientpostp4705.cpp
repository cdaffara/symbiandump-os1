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
#include "ctlbsclientpostp4705.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4705::CT_LbsClientPosTp4705(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP4705 - Default Proxy fallback-partial update1");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4705::~CT_LbsClientPosTp4705()
    {
    }

// ---------------------------------------------------------
// CPosTp4705::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4705::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CPosTp4705::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4705::StartL()
    {
    ConnectL();

    SetupProxyPSYsL();

    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP4705");
    iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KServiceName);

    //
	// Request 1
	//
    // Check that partial update is not supported
    TPositionUpdateOptions updateOptions;
    updateOptions.SetAcceptPartialUpdates(ETrue);
    iPositioner.SetUpdateOptions(updateOptions);
        
    TInt request = 4705;
    TPositionInfo posInfo;
    RequestL(posInfo, request, KEspectedErrorCodePSY2);    
      
    VerifyPositionFromL(posInfo, iUidTestPsyPartialUpdate);
    VerifyRequestTimeLessThanL(8001000);
          
    /*Default proxy fallbacks to TestPsyPartialUpdate because of time-shift,partial 
      update will be given by TestPsyPartialUpdate.Request to TestProxyPsy1
      gets cancelled after cleanup time.*/
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestPsyPartialUpdate);
    
    VerifyPositionL(posInfo, 30, 40, 50);
    }

// ---------------------------------------------------------
// CPosTp4705::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4705::SetupProxyPSYsL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    
    CPosModuleIdList* prioList = db->ModuleIdListLC();
	
    // Disable all PSY:s except PSYs used by this test case
    
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] != iUidTestProxyPsy1 &&
            (*prioList)[i] != iUidTestPsyPartialUpdate)
            {
            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
            }
        }
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestProxyPsy1, *moduleUpdate);
    db->UpdateModuleL(iUidTestPsyPartialUpdate, *moduleUpdate);
    
    db->SetModulePriorityL(iUidTestProxyPsy1, 0);
    db->SetModulePriorityL(iUidTestPsyPartialUpdate, 1);
    
    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    } 

//  End of File

    
