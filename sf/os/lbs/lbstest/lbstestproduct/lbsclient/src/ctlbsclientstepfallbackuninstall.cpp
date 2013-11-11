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
#include "ctlbsclientstepfallbackuninstall.h"
#include "ctlbsclientdefs.h"
#include <badesca.h>
#include <epos_cposmodules.h>
#include <epos_cposmoduleupdate.h>
#include <epos_cposmoduleidlist.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <swi/launcher.h>
#include "tctlbsclientmodulesobserver.h"
#include "ctlbsinstallui.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientStepFallbackUninstall::CT_LbsClientStepFallbackUninstall(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP301 - Error on Uninstall");
	SetTestStepName(KTestName); 
	}


// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientStepFallbackUninstall::~CT_LbsClientStepFallbackUninstall()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientStepFallbackUninstall::StartL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepFallbackUninstall::StartL()
    {
    iError = EFalse;
       
    // Make sure the psy is uninstalled to begin with
	TInt appId;
	GetIntFromConfig(ConfigSection(),KLbsSisInstallAppId,appId);
	SISUninstallPsyL(TUid::Uid(appId));	// may return an error, but that's ok
	User::After(2000000);
	
    // Register as an observer to listen for events
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    db->SetObserverL(*this);
	
    // Make sure that only the delayed response (integrated, non-gps) test psy is enabled:
    SetupPsyL(iUidDelayedResponsePsy);
    
	////////////////////////////////////
	// Install test psy:
	TPtrC sispath;
	GetStringFromConfig(ConfigSection(),KLbsSisInstallSisLocation,sispath);
	TInt err = SISInstallPsyL(sispath);	
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Failing: SISInstallPsyL returned %d\n"), err);
		SetTestStepResult(EFail);
		}
	iExpectedEvent = EPosModulesEventModuleInstalled;
	INFO_PRINTF1(_L("Wait for EPosModulesEventModuleInstalled event\n"));
	// Wait for event
	DoEventHandlingL();	
	////////////////////////////////////
	
	////////////////////////////////////
	// Set priorities with installed psy at top (so psy is in use when uninstalled):
    db->SetModulePriorityL(iUidInstallableTrackingPsy, 0);
    db->SetModulePriorityL(iUidDelayedResponsePsy, 1);    
    ////////////////////////////////////
    
	////////////////////////////////////
	// Make a periodic request to default proxy and verify it comes from the installed psy:
    RPositionServer	posServer;
    CleanupClosePushL(posServer);
	err = posServer.Connect();
	_LIT(KConnectErr, "Error when connecing to EPos server,  %d");
	AssertTrueL(err == KErrNone, KConnectErr, err);  	
	
    RPositioner positioner;
    CleanupClosePushL(positioner);
    err = positioner.Open(posServer); 
	_LIT(KOpenErr, "Error when opening positioner,  %d");
	AssertTrueL(err == KErrNone, KOpenErr, err);
	
    TInt normalInterval = 5000000;	// 5 seconds
	TTimeIntervalMicroSeconds interval = TTimeIntervalMicroSeconds(normalInterval);	
	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(interval);
	
	err = positioner.SetUpdateOptions(posOption);
	_LIT(KUpdateErr, "Error when setting update interval,  %d");
	AssertTrueL(err == KErrNone, KUpdateErr, err);	
	INFO_PRINTF2(_L("Update Interval set to %d\n"), interval.Int64());
	
	TPositionInfo positionInfo; 	
	TRequestStatus status;
    positioner.NotifyPositionUpdate(positionInfo, status);
    INFO_PRINTF1(_L("Wait for NotifyPositionUpdate request to complete\n"));
    User::WaitForRequest(status);
    
    VerifyPositionFromL(positionInfo, iUidInstallableTrackingPsy);
    
    if(status != KErrNone)
    	{
    	INFO_PRINTF2(_L("Failing: NotifyPositionUpdate returned %d\n"), status.Int());
    	}
	////////////////////////////////////
    	
    INFO_PRINTF1(_L("Issue another NotifyPositionUpdate() request (target time will be in the future)\n"));
    positioner.NotifyPositionUpdate(positionInfo, status);
    
#ifndef	__WINSCW__ // doesn't work on the emulator (where cannot uninstall an in-use dll)    
	////////////////////////////////////
	// UnInstall test psy while last request is outstanding:
    INFO_PRINTF1(_L("Uninstall the PSY\n"));
	err = SISUninstallPsyL(TUid::Uid(appId));	
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Failing: SISUninstallPsyL returned %d\n"), err);
		SetTestStepResult(EFail);
		}	
	iExpectedEvent = EPosModulesEventModuleRemoved;
	INFO_PRINTF1(_L("Wait for EPosModulesEventModuleRemoved event\n"));
	// Wait for event
	DoEventHandlingL();		
	////////////////////////////////////
#endif	// not winscw
	
	INFO_PRINTF1(_L("Wait for second NotifyPositionUpdate() request to complete\n"));	
    User::WaitForRequest(status);   
	
    INFO_PRINTF2(_L("NotifyPositionUpdate() request completed with %d\n"), status.Int());  
    
	if(status != KErrNone)
		{
		INFO_PRINTF1(_L("Failing: On emulator NotifyPositionUpdate expected to completed with KErrNone\n"));
		SetTestStepResult(EFail);
		}  
#ifdef	__WINSCW__	
	VerifyPositionFromL(positionInfo, iUidInstallableTrackingPsy);	
	// make sure the psy is unloaded before trying to uninstall it on the emulator
	CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);	
    // Emulator will lock the psy plugin dll for a period of time when we can't uninstall it:
    User::After(30000000);    
	// cleanup - uninstall the psy:
	err = SISUninstallPsyL(TUid::Uid(appId));	
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Failing: SISUninstallPsyL returned %d\n"), err);
		SetTestStepResult(EFail);
		}	
#else	// on arm the request should come from the ROM-based psy
	INFO_PRINTF1(_L("Verify the update came from the ROM-based PSY\n"));
	VerifyPositionFromL(positionInfo, iUidDelayedResponsePsy);
	INFO_PRINTF1(_L("Verify the uninstalled PSY is unloaded\n"));	
	VerifyPsyUnloadedL(iUidInstallableTrackingPsy);
	CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);	
#endif
    

    
	db->RemoveObserver();
	CleanupStack::PopAndDestroy(db);

    }

TInt CT_LbsClientStepFallbackUninstall::SISUninstallPsyL(const TUid& appId)
	{
	CUISisAdaptor* ui = CUISisAdaptor::NewLC(Logger());

	TInt err = Swi::Launcher::Uninstall(*ui, appId);
	
	CleanupStack::PopAndDestroy(ui);
	
	INFO_PRINTF2(_L("Launcher::UnInstall returned %d\n"), err);
	
	return err;
	}

TInt CT_LbsClientStepFallbackUninstall::SISInstallPsyL(const TDesC& aSisPath)
	{
	CUISisAdaptor *ui = CUISisAdaptor::NewLC(Logger());
	CInstallPrefs* prefs = CInstallPrefs::NewLC();

	TInt err = Swi::Launcher::Install(*ui, aSisPath, *prefs);

	CleanupStack::PopAndDestroy(prefs);
	CleanupStack::PopAndDestroy(ui);

	INFO_PRINTF2(_L("Launcher::Install returned %d\n"), err);
	
	return err;
	}

// ---------------------------------------------------------
// CT_LbsClientStepFallbackUninstall::DoEventHandlingL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepFallbackUninstall::DoEventHandlingL()
    {
	CActiveScheduler::Start();
    CheckErrorL();
    }

// ---------------------------------------------------------
// CT_LbsClientStepFallbackUninstall::CheckErrorL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepFallbackUninstall::CheckErrorL()
    {
    if (iError)
        {
        LogErrorAndLeaveL(iErrorText);
        }
    }

// ---------------------------------------------------------
// CT_LbsClientStepFallbackUninstall::HandleDatabaseChangeL
//
// ---------------------------------------------------------
void CT_LbsClientStepFallbackUninstall::HandleSettingsChangeL(TPosModulesEvent aModuleEvent)
    {
    INFO_PRINTF2(_L("Event received %d\n"), aModuleEvent);    
    CActiveScheduler::Stop();
    if (aModuleEvent.iType != iExpectedEvent)
        {
        INFO_PRINTF2(_L("Unexpected Event received %d\n"), aModuleEvent);
    	SetTestStepResult(EFail);
        }
    }

