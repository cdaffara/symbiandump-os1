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
#include "ctlbsclientpostp4708.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4708::CT_LbsClientPosTp4708(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {
    _LIT(KTestName, "TP4708 - Default Proxy-Configure time-shift values");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4708::~CT_LbsClientPosTp4708()
    {
    }

// ---------------------------------------------------------
// CPosTp4708::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4708::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CPosTp4708::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4708::StartL()
    {
    ConnectL();

    SetupProxyPSYsL();

    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP4708");
    iPositioner.SetRequestor(CRequestor::ERequestorService, 
    		CRequestor::EFormatApplication, KServiceName);
        
    //
	// Request 1
	//
    TInt request = 4708;
    TPositionInfo posInfo;
    RequestL(posInfo, request, KEspectedErrorCodePSY1);    
      
    VerifyPositionFromL(posInfo, iUidTestProxyPsy2);    
    VerifyRequestTimeLessThanL(8000000);
    
    // Default proxy fallbacks to testproxypsy2 
    // because of time-shift,fix will 
    // be given by TestProxyPsy2 first.
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);

    VerifyPositionL(posInfo, 10, 20, 30);
    
    //
	// Request 2
	//
    request = 100;    
    RequestL(posInfo, request, KEspectedErrorCodePSY1);

    request = 4708;
    RequestL(posInfo, request, KEspectedErrorCodePSY1);
      
    VerifyPositionFromL(posInfo, iUidTestProxyPsy2);
    VerifyRequestTimeLessThanL(5001000);
    
    VerifyPositionL(posInfo, 10, 20, 30);    
    }

// ---------------------------------------------------------
// CPosTp4708::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4708::SetupProxyPSYsL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    moduleUpdate->SetUpdateAvailability(EFalse);
    	
    // Disable all PSY:s except PSYs used by this test case
    CPosModuleIdList* prioList = db->ModuleIdListLC();
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] !=  iUidTestProxyPsy1 &&
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
