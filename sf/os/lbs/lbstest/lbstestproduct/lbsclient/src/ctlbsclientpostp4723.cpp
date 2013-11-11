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
#include "ctlbsclientpostp4723.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4723::CT_LbsClientPosTp4723(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {
    _LIT(KTestName, "TP4723 - Default Proxy -Configure working GPS PSY");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4723::~CT_LbsClientPosTp4723()
    {
    }

// ---------------------------------------------------------
// CPosTp4723::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4723::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CPosTp4723::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4723::StartL()
    {
    ConnectL();
    SetupProxyPSYsL();
	
	TPositionModuleId modId = iUidSatInfoPsy;
    CRepository* repos = CRepository::NewL( KCRUidDefaultProxyConfiguration );    
    TBuf<40> idBuf;
    idBuf.AppendNumFixedWidth( modId.iUid, EHex, 8);
    TInt error = repos->Set(KDefaultProxyLastWorkingGpsPsy, idBuf);
    delete repos;
    User::LeaveIfError(error);
	
    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP4723");
    iPositioner.SetRequestor(CRequestor::ERequestorService, 
		CRequestor::EFormatApplication, KServiceName);

    TPositionSatelliteInfo posInfo;
    // Request value should be 4722, even for TP4723...
    TInt request = 4722;
    
    // Default PSY won't pre-load Multi PSY since it is Internal
    VerifyPsyUnloadedL(iUidMultiPsy);
    // Default PSY should pre-load SetInfo PSY since it is External and supports GPS
    VerifyPsyLoadedL(iUidSatInfoPsy);
    
	//
	// Request 1
	//
    RequestL(posInfo, request, KEspectedErrorCodePSY1);    
      
    VerifyPositionFromL(posInfo, iUidSatInfoPsy);    
    VerifyRequestTimeLessThanL(1200000);
    
    VerifyPsyLoadedL(iUidSatInfoPsy);
    
    VerifyPositionL(posInfo, 40, 50, 60);
    }

// ---------------------------------------------------------
// CPosTp4723::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4723::SetupProxyPSYsL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    
    CPosModuleIdList* prioList = db->ModuleIdListLC();
	
    // Disable all PSY:s except PSYs used by this test case
    
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] != iUidMultiPsy &&
            (*prioList)[i] != iUidSatInfoPsy)
            {
            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
            }
        }
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidMultiPsy, *moduleUpdate);
    db->UpdateModuleL(iUidSatInfoPsy, *moduleUpdate);
    
    db->SetModulePriorityL(iUidMultiPsy, 0);
    db->SetModulePriorityL(iUidSatInfoPsy, 1);
    
    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    } 

//  End of File

    
