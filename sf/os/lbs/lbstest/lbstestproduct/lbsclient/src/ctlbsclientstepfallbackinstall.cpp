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
#include "ctlbsclientstepfallbackinstall.h"
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

// temp:
// #include <implementationinformation.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientStepFallbackInstall::CT_LbsClientStepFallbackInstall(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP301 - Error on Uninstall");
	SetTestStepName(KTestName); 
	}


// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientStepFallbackInstall::~CT_LbsClientStepFallbackInstall()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientStepFallbackInstall::StartL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepFallbackInstall::StartL()
    {
    iError = EFalse;
    
    // Make sure the psy is uninstalled to begin with
	TInt appId;
	TInt installedpsyId;
	User::LeaveIfError(GetIntFromConfig(ConfigSection(),KLbsSisInstallAppId,appId));
	User::LeaveIfError(GetIntFromConfig(ConfigSection(),KLbsPsyId,installedpsyId));
	SISUninstallPsyL(TUid::Uid(appId));	// may return an error, but that's ok
	User::After(2000000);
	
    // Register as an observer to listen for events
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    db->SetObserverL(*this);
	
    // Make sure that only the delayed response (integrated, non-gps) test psy is enabled:
    SetupPsyL(iUidDelayedResponsePsy);
    
	////////////////////////////////////
	// Make a request using default proxy psy:
    RPositionServer	posServer;
    CleanupClosePushL(posServer);
	TInt err = posServer.Connect();
	_LIT(KConnectErr, "Error when connecing to EPos server, %d");
	AssertTrueL(err == KErrNone, KConnectErr, err);  	
	
    RPositioner positioner;
    CleanupClosePushL(positioner);
    err = positioner.Open(posServer); // default proxy
	_LIT(KOpenErr, "Error when opening positioner,  %d");
	AssertTrueL(err == KErrNone, KOpenErr, err);
		
	// Set a fairly long timeout for the request
	TInt timeout = 60000000;
	TTimeIntervalMicroSeconds maxtime(timeout); 
	TPositionUpdateOptions posOption;
	posOption.SetUpdateTimeOut(maxtime);
	
	err = positioner.SetUpdateOptions(posOption);
	_LIT(KUpdateErr, "Error when setting update interval, %d");
	AssertTrueL(err == KErrNone, KUpdateErr, err);	
	
	// tell the psy to delay its response
	TInt32 delay(10000000);	// 10 secs
	TPositionInfo positionInfo; 
	positionInfo.SetUpdateType(delay);
	TRequestStatus status;
	INFO_PRINTF1(_L("NotifyPositionUpdate()\n"));
    positioner.NotifyPositionUpdate(positionInfo, status);
	////////////////////////////////////
	
	////////////////////////////////////
	// Install another ( the installable tracking) test psy :
	TPtrC sispath;
	GetStringFromConfig(ConfigSection(),KLbsSisInstallSisLocation,sispath);
	err = SISInstallPsyL(sispath);	
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Failing: SISInstallPsyL returned %d\n"), err);
		SetTestStepResult(EFail);
		}
	iExpectedEvent = EPosModulesEventModuleInstalled;
	// Wait for event
	INFO_PRINTF1(_L("Wait for EPosModulesEventModuleInstalled event\n"));
	DoEventHandlingL();	
	////////////////////////////////////
	
	// verify that the new installed psy is in the prioritised list of psys:	
	CPosModuleIdList* prioList = db->ModuleIdListLC();
	TBool	found = EFalse;
	
	for (TInt i = 0 ; i < prioList->Count(); i++)
		{
		TUid psyUid = (*prioList)[i];
		if (psyUid == TUid::Uid(installedpsyId))
			{
			INFO_PRINTF1(_L("Found installed psy in priority list\n"));
			found = ETrue;
			}
		}	
	if(!found)
		{
		INFO_PRINTF2(_L("Failed: New psy with id 0x%x not found in prioritised list\n"), TUid::Uid(installedpsyId));
		SetTestStepResult(EFail);		
		}
	
	// Wait for the update request to complete 
	INFO_PRINTF1(_L("Wait for NotifyPositionUpdate() to complete\n"));
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("NotifyPositionUpdate completed with %d\n"), err);
    err = status.Int(); 
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		}
	// Verify that we got our response from the original test psy (not the installed one)
	else if(positionInfo.ModuleId().iUid != iUidDelayedResponsePsy.iUid)
		{
		INFO_PRINTF2(_L("Received an update from the wrong module 0x%x\n"), positionInfo.ModuleId().iUid );
		SetTestStepResult(EFail);		
		}
	
	CleanupStack::PopAndDestroy(prioList);
	CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);
    
	db->RemoveObserver();
	CleanupStack::PopAndDestroy(db);
	
	// Uninstall the installed psy (tidyup):
	err = SISUninstallPsyL(TUid::Uid(appId));	
			
    }

TInt CT_LbsClientStepFallbackInstall::SISUninstallPsyL(const TUid& appId)
	{
	CUISisAdaptor* ui = CUISisAdaptor::NewLC(Logger());

	TInt err = Swi::Launcher::Uninstall(*ui, appId);
	
	CleanupStack::PopAndDestroy(ui);
	
	INFO_PRINTF2(_L("Launcher::UnInstall returned %d\n"), err);
	
	return err;
	}

TInt CT_LbsClientStepFallbackInstall::SISInstallPsyL(const TDesC& aSisPath)
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
// CT_LbsClientStepFallbackInstall::DoEventHandlingL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepFallbackInstall::DoEventHandlingL()
    {
    iStopped = EFalse;
	CActiveScheduler::Start();
    CheckErrorL();
    }

// ---------------------------------------------------------
// CT_LbsClientStepFallbackInstall::CheckErrorL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepFallbackInstall::CheckErrorL()
    {
    if (iError)
        {
        LogErrorAndLeaveL(iErrorText);
        }
    }

// ---------------------------------------------------------
// CT_LbsClientStepFallbackInstall::HandleDatabaseChangeL
//
// ---------------------------------------------------------
void CT_LbsClientStepFallbackInstall::HandleSettingsChangeL(TPosModulesEvent aModuleEvent)
    {
    if(!iStopped)
    	{
    	CActiveScheduler::Stop();
    	iStopped = ETrue;
    	}
    if (aModuleEvent.iType != iExpectedEvent)
        {
        INFO_PRINTF2(_L("Unexpected Event received %d\n"), aModuleEvent);
    	SetTestStepResult(EFail);
        }
    }

