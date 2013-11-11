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
#include "ctlbsclientpostp269.h"
#include <lbssatellite.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <e32std.h>

// CONSTANTS                                

const TInt KEspectedErrorCodePSY1Request6 = KErrDied;
const TInt KEspectedErrorCodePSY1Request8 = KErrBadPower;
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp269::CT_LbsClientPosTp269(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "Tp269 - Fallback in Default Proxy (CR0526)");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp269::~CT_LbsClientPosTp269()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp269::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp269::StartL()
    {    
    ConnectL();

    SetupProxyPSYsL();

    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueSecL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP269");
    iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KServiceName);

    TUint request = 0;
    
    TPositionInfo posInfo;
    //
	// Request 1
	//    
    RequestL(posInfo, ++request, KEspectedErrorCodePSY2);
   
    // Used test PSYs reports ready when loaded, inactive when unloaded
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);

    //
	// Request 2
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY1);
    VerifyPsyLoadedL(iUidTestProxyPsy1);

    //
	// Request 3
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY3);
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);
    VerifyPsyLoadedL(iUidTestProxyPsy3);
    
    //
	// Request 4
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY3);
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);
    VerifyPsyLoadedL(iUidTestProxyPsy3);

    //
	// Request 5
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY2);
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);
    
    //
	// Request 6
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY1Request6);
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);
    VerifyPsyLoadedL(iUidTestProxyPsy3);

    // Switch priority
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    
    db->SetModulePriorityL(iUidTestProxyPsy3, 1);
    db->SetModulePriorityL(iUidTestProxyPsy2, 2);

    //
	// Request 7
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY3);
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy3);

    //
	// Request 8
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY1Request8);
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);
    VerifyPsyLoadedL(iUidTestProxyPsy3);

    // Switch back priority
    db->SetModulePriorityL(iUidTestProxyPsy2, 1);
    db->SetModulePriorityL(iUidTestProxyPsy3, 2);

    CleanupStack::PopAndDestroy(db);
    
    //
	// Request 9
	//
    TPositionSatelliteInfo satInfo;
    RequestL(satInfo, ++request, KErrArgument);

    //
	// Request 10
	//
    TPositionCourseInfo courseInfo;
    RequestL(courseInfo, ++request, KEspectedErrorCodePSY3);
      
    VerifyPsyLoadedL(iUidTestProxyPsy3);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp269::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp269::CloseTest()
    {
    iPositioner.Close();
    iPosServer.Close();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp269::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp269::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }
   
// ---------------------------------------------------------
// CT_LbsClientPosTp269::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp269::SetupProxyPSYsL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    
    CPosModuleIdList* prioList = db->ModuleIdListLC();
	
    // Disable all PSY:s except PSYs used by this TP269, TP270
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] != iUidTestProxyPsy1 &&
            (*prioList)[i] != iUidTestProxyPsy2 &&
            (*prioList)[i] != iUidTestProxyPsy3)
            {
            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
            }
        }
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestProxyPsy1, *moduleUpdate);
    db->UpdateModuleL(iUidTestProxyPsy2, *moduleUpdate);
    db->UpdateModuleL(iUidTestProxyPsy3, *moduleUpdate);

    db->SetModulePriorityL(iUidTestProxyPsy1, 0);
    db->SetModulePriorityL(iUidTestProxyPsy2, 1);
    db->SetModulePriorityL(iUidTestProxyPsy3, 2);

    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    } 

//  End of File

