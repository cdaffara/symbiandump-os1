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
#include <centralrepository.h>
#include "ctlbsclientpostp4709.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4709::CT_LbsClientPosTp4709(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {
    _LIT(KTestName, "TP4709 - Default Proxy-Configure Cleanup Timeout");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4709::~CT_LbsClientPosTp4709()
    {
    }

// ---------------------------------------------------------
// CPosTp4709::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4709::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CPosTp4709::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4709::StartL()
    {    
    ConnectL();

    SetupProxyPSYsL();

    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP4709");
    iPositioner.SetRequestor(CRequestor::ERequestorService, 
    		CRequestor::EFormatApplication, KServiceName);

    ResetDefaultPSYLastWorkingExtGPSPSYL();
    
    TInt request = 100;
    TPositionInfo posInfo;
    RequestL(posInfo, request, KEspectedErrorCodePSY1);
    
    //
	// Request 1
	//
                                              
    // Check that partial update is not supported
    // Check this with TP/TC document, Fredrik
    //
    TPositionUpdateOptions updateOptions;
    updateOptions.SetAcceptPartialUpdates(ETrue);
    TInt res = iPositioner.SetUpdateOptions(updateOptions);
    User::LeaveIfError(res);
    
    request = 4709;
    RequestL(posInfo, request, KEspectedErrorCodePSY2);    

    VerifyPositionFromL(posInfo, iUidTestPsyPartialUpdate);
    VerifyRequestTimeLessThanL(4400000);
    
    VerifyPositionL(posInfo, 30, 40, 50);
    }

// ---------------------------------------------------------
// CPosTp4709::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4709::SetupProxyPSYsL()
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
