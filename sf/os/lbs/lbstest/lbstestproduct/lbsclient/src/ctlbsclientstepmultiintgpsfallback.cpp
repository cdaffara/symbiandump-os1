/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


//  INCLUDES
#include "ctlbsclientstepmultiintgpsfallback.h"
#include <epos_cposmodules.h>
#include <epos_cposmoduleupdate.h>
#include <epos_cposmoduleidlist.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientStepMultiIntGpsFallback::CT_LbsClientStepMultiIntGpsFallback(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {
    _LIT(KTestName, "TP304 - Multiple integrated GPS PSYs during fallback");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientStepMultiIntGpsFallback::~CT_LbsClientStepMultiIntGpsFallback()
    {
    }

// ---------------------------------------------------------
// CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientStepMultiIntGpsFallback::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientStepMultiIntGpsFallback::StartL()
    {
    ConnectL();
    SetupProxyPSYsL();
	
    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP304");
    iPositioner.SetRequestor(CRequestor::ERequestorService, 
		CRequestor::EFormatApplication, KServiceName);

    TPositionInfo posInfo;
    TInt request = 304;
    
    // PSYs won't be pre-loaded since they are all internal
    VerifyPsyUnloadedL(iUidTestProxyPsy3);
    VerifyPsyUnloadedL(iUidTestProxyPsy5);
    VerifyPsyUnloadedL(iUidTestProxyPsy2);
    
	/////////////////////////////////////////////
	// Request 
	/////////////////////////////////////////////
    RequestL(posInfo, request, KErrNone);    
      
    VerifyPositionFromL(posInfo, iUidTestProxyPsy2);    
    VerifyRequestTimeLessThanL(1200000);
    
    VerifyPsyLoadedL(iUidTestProxyPsy3);
    VerifyPsyUnloadedL(iUidTestProxyPsy5);
    
    VerifyPositionL(posInfo, 10, 20, 30);
    }

// ---------------------------------------------------------
// SetupProxyPSYsL
//
//	KTestProxyPsy3 	integrated gps. responds with error for this test case
//	KTestProxyPsy5	integrated gps. should not be called during this test case. responds with KerrNone
//	KTestProxyPsy2  integrated non-gps responds with KErrNone
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientStepMultiIntGpsFallback::SetupProxyPSYsL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    
    CPosModuleIdList* prioList = db->ModuleIdListLC();
	
    // Disable all PSY:s except PSYs used by this test case
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] != iUidTestProxyPsy3 &&
            (*prioList)[i] != iUidTestProxyPsy5 && 
            (*prioList)[i] != iUidTestProxyPsy2)
            {
            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
            }
        }
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestProxyPsy3, *moduleUpdate);
    db->UpdateModuleL(iUidTestProxyPsy5, *moduleUpdate);
    db->UpdateModuleL(iUidTestProxyPsy2, *moduleUpdate);
    
    db->SetModulePriorityL(iUidTestProxyPsy3, 0);
    db->SetModulePriorityL(iUidTestProxyPsy5, 1);
    db->SetModulePriorityL(iUidTestProxyPsy2, 2);
    
    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    } 

//  End of File

    
