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
#include "ctlbsclientpostp4722.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4722::CT_LbsClientPosTp4722(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP4722 - Default Proxy -Prefer working GPS PSY");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp4722::~CT_LbsClientPosTp4722()
    {
    }

// ---------------------------------------------------------
// CPosTp4722::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4722::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CPosTp4722::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4722::StartL()
    {
    ConnectL();

    SetupProxyPSYsL();
	ResetDefaultPSYLastWorkingExtGPSPSYL();
	
    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP4722");
    iPositioner.SetRequestor(CRequestor::ERequestorService, 
    		CRequestor::EFormatApplication, KServiceName);

    TPositionSatelliteInfo posInfo;

    // Pre request
    TInt request = 100;
    RequestL(posInfo, request, KErrNone);
    
    //
	// Request 1
	//
    request = 4722;    
    RequestL(posInfo, request, KErrNone);
    VerifyPositionFromL(posInfo, iUidSatInfoPsy);    
    VerifyRequestTimeLessThanL(4001000);

    VerifyPositionL(posInfo, 40, 50, 60);

	// Default PSY should pre-load SetInfo PSY since it is External and supports GPS
    VerifyPsyLoadedL(iUidSatInfoPsy);
 
    ClosePositioner();
    Disconnect();
    
    const TTimeIntervalMicroSeconds32 KPosWaitServerShutDown( 6000000 );
    User::After(KPosWaitServerShutDown);

    ConnectL();
    SetupProxyPSYsL();
    err = OpenPositioner();
    AssertTrueL(err == KErrNone, KOpenErr, err);

    iPositioner.SetRequestor(CRequestor::ERequestorService, 
    		CRequestor::EFormatApplication, KServiceName);
    
    //
	// Request 2
	//
    RequestL(posInfo, request, KErrNone);    

    VerifyPositionFromL(posInfo, iUidSatInfoPsy);    
    VerifyRequestTimeLessThanL(4001000);

    VerifyPositionL(posInfo, 40, 50, 60);

	// Default PSY won't pre-load Multi PSY since it is Internal
	VerifyPsyUnloadedL(iUidMultiPsy);
	// Default PSY should pre-load SetInfo PSY since it is External and supports GPS
    VerifyPsyLoadedL(iUidSatInfoPsy);
    }

// ---------------------------------------------------------
// CPosTp4722::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp4722::SetupProxyPSYsL()
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

    
