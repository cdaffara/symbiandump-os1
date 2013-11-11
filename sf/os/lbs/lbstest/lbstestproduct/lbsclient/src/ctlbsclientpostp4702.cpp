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
#include "ctlbsclientpostp4702.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4702::CT_LbsClientPosTp4702(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP4702 - Default Proxy fallback-Active\\Ready Status2");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4702::~CT_LbsClientPosTp4702()
    {
    }

// ---------------------------------------------------------
// CPosTp4702::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4702::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CPosTp4702::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4702::StartL()
    {   
    ConnectL();
   
    SetupProxyPSYsL();

    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP4702");
    iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KServiceName);

    //
	// Request 1
	//
    TInt request = 4702;
    TPositionInfo posInfo;
    RequestL(posInfo, request, KEspectedErrorCodePSY1);    

    VerifyPositionFromL(posInfo, iUidTestProxyPsy1);
    VerifyRequestTimeLessThanL(7500000);    
    
    /*Default proxy fallbacks to testproxypsy2 because of time-shift,fix will 
      be given by TestProxyPsy1 first.*/
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);

    VerifyPositionL(posInfo, 5, 15, 25);
    }
 
// ---------------------------------------------------------
// CPosTp4702::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4702::SetupProxyPSYsL()
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
            (*prioList)[i] != iUidTestProxyPsy2)
            {
            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
            }
        }
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestProxyPsy1, *moduleUpdate);
    db->UpdateModuleL(iUidTestProxyPsy2, *moduleUpdate);
    
    db->SetModulePriorityL(iUidTestProxyPsy1, 0);
    db->SetModulePriorityL(iUidTestProxyPsy2, 1);
    
    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    } 

//  End of File
