// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ctlbsclientpostp271.h"

#include <lbssatellite.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <e32std.h>

// CONSTANTS                                

const TInt KUpdateInterval  = 1000;
const TInt KUpdateInterval2 = 2000;

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp271::CT_LbsClientPosTp271(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{ 
	_LIT(KTestName,"Tp271-Fallback in Default Proxy,Perodic Update");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp271::~CT_LbsClientPosTp271()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp271::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp271::StartL()
    {   
    ConnectL();

    SetupProxyPSYsL();
    
    iModuleEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventAll);

    TInt err = OpenPositioner();
    _LIT(KOpenErr, "Error when opening positioner, %d");
    AssertTrueSecL(err == KErrNone, KOpenErr, err);
    
    _LIT(KServiceName, "TP271");
    iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KServiceName);

    TUint request = 20;    
    TPositionInfo posInfo;
    //
	// Request 1
	//    
    RequestL(posInfo, ++request, KEspectedErrorCodePSY2);
   
    // Used test PSYs reports ready when loaded, inactive when unloaded
    VerifyPsyLoadedL(iUidTestProxyPsy1);
    VerifyPsyLoadedL(iUidTestProxyPsy2);

    // Set periodic update
    TPositionUpdateOptions updateOptions;
	updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(KUpdateInterval));

	err = iPositioner.SetUpdateOptions(updateOptions);
    _LIT(KUpdateErr, "Error when setting update interval,  %d");
	AssertTrueL(err == KErrNone, KUpdateErr, err);    
    
    //
	// Request 2
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY3);
   
    
    //
	// Request 3
	//
    RequestL(posInfo, ++request, KEspectedErrorCodePSY2);

    VerifyPsyStartedTrackingL(iUidTestProxyPsy1); // Verify that still loaded, latest reported event
    VerifyPsyStartedTrackingL(iUidTestProxyPsy2); // should still be the same
    VerifyPsyStartedTrackingL(iUidTestProxyPsy3); 

    //
	// Request 4
	//    
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(KUpdateInterval2));
    err = iPositioner.SetUpdateOptions(updateOptions);
	AssertTrueL(err == KErrNone, KUpdateErr, err);    

    RequestL(posInfo, ++request, KEspectedErrorCodePSY2);
    
    VerifyPsyStartedTrackingL(iUidTestProxyPsy1);
    VerifyPsyStartedTrackingL(iUidTestProxyPsy2);
   
    //
	// Request 5
	//
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(0));
    err = iPositioner.SetUpdateOptions(updateOptions);
	AssertTrueL(err == KErrNone, KUpdateErr, err);    
    
    VerifyPsyStoppedTrackingL(iUidTestProxyPsy1);
    VerifyPsyStoppedTrackingL(iUidTestProxyPsy2);

    RequestL(posInfo, ++request, KEspectedErrorCodePSY3);
 
    VerifyPsyStoppedTrackingL(iUidTestProxyPsy1); // Verifies that still loaded,
    VerifyPsyStoppedTrackingL(iUidTestProxyPsy2); // Verifies that still loaded,

    //
	// Request 6
	//
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(KUpdateInterval));
	err = iPositioner.SetUpdateOptions(updateOptions);
	AssertTrueL(err == KErrNone, KUpdateErr, err);    
    
    RequestL(posInfo, ++request, KEspectedErrorCodePSY1);
            
    iPositioner.Close();
    
    VerifyPsyUnloadedL(iUidTestProxyPsy1);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp271::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp271::CloseTest()
    {
    iPositioner.Close();
    iPosServer.Close();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp271::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp271::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }
   
// ---------------------------------------------------------
// CT_LbsClientPosTp271::SetupProxyPSYsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp271::SetupProxyPSYsL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    
    CPosModuleIdList* prioList = db->ModuleIdListLC();

    // Disable all PSY:s except PSYs used by this TP271, TP270
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

// ---------------------------------------------------------
// CT_LbsClientPosTp271::VerifyPsyStartedUnloadedL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp271::VerifyPsyStartedTrackingL(TPositionModuleId& aUid)                           
    {
    _LIT(KPSYStatus,"Verifies if correct PSY is loaded by getting its status");
    INFO_PRINTF1(KPSYStatus);

    _LIT(KDeviceStatusErr, "The PSY has not reported expected Device Status and is not notified about start tracking"); 
    
    TPositionModuleStatus moduleStatus;
    iPosServer.GetModuleStatus(moduleStatus, aUid); 

    AssertTrueL(TPositionModuleStatus::EDeviceInitialising == moduleStatus.DeviceStatus(), KDeviceStatusErr, KErrGeneral);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp271::VerifyPsyStoppedTrackingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp271::VerifyPsyStoppedTrackingL(TPositionModuleId& /*aUid*/, TRequestStatus& aStatus)
    {
    _LIT(KPSYStatus,"Verifies that correct PSY has stopped tracking and is unloaded");
    INFO_PRINTF1(KPSYStatus);
    
    _LIT(KStoppedTrackingErr, "The PSY has not reported expected device status and has not stopped tracking"); 
    
    TPositionModuleStatus moduleStatus;
   
    User::WaitForRequest(aStatus); 
             
    iModuleEvent.GetModuleStatus(moduleStatus);
   
    AssertTrueL(TPositionModuleStatus::EDeviceStandBy == moduleStatus.DeviceStatus(), 
                KStoppedTrackingErr, 
                KErrGeneral);

   
    }
// ---------------------------------------------------------
// CT_LbsClientPosTp271::VerifyPsyStoppedTrackingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp271::VerifyPsyStoppedTrackingL(TPositionModuleId& aUid)
    {
    _LIT(KPSYStatus,"Verifies that correct PSY has stopped tracking and is unloaded");
    INFO_PRINTF1(KPSYStatus);
    
    _LIT(KStoppedTrackingErr, "The PSY has not reported expected device status and has not stopped tracking"); 
    
    TPositionModuleStatus moduleStatus;
   
    iPosServer.GetModuleStatus(moduleStatus, aUid); 
   
    AssertTrueL(TPositionModuleStatus::EDeviceStandBy == moduleStatus.DeviceStatus(), 
                KStoppedTrackingErr, 
                KErrGeneral);
    }

//  End of File
