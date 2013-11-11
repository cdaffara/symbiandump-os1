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
#include "ctlbsclientstepobserveinstall.h"
#include <badesca.h>
#include <epos_cposmodules.h>
#include <epos_cposmoduleupdate.h>
#include <epos_cposmoduleidlist.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <swi/launcher.h>
#include "tctlbsclientmodulesobserver.h"
#include "ctlbsinstallui.h"
#include "ctlbsclientdefs.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientStepObserveInstall::CT_LbsClientStepObserveInstall(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP300 - Observe Install");
	SetTestStepName(KTestName); 
	}


// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientStepObserveInstall::~CT_LbsClientStepObserveInstall()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientStepObserveInstall::StartL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepObserveInstall::StartL()
    {
    iError = EFalse;

    // Make sure the psy is uninstalled to begin with
    INFO_PRINTF1(_L("Make sure the psy isn't already installed\n"));
	TInt appId;
	GetIntFromConfig(ConfigSection(),KLbsSisInstallAppId,appId);
	TInt err = SISUninstallPsyL(TUid::Uid(appId));	// may return an error, but that's ok
	User::After(2000000);
	
    // Register as an observer to listen for events
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    db->SetObserverL(*this);

    // Update the priority of one of the test psys:
    TInt priority = db->PriorityL(iUidTestPsy2);
    if (priority > 0)
        {
        priority = 0;	// 0 is the highest priority
        }
    else
        {
        priority = 1; 	// this assumes that more that one PSY is installed
        }
    db->SetModulePriorityL(iUidTestPsy2, priority);
    
    iExpectedEvent = EPosModulesEventPrioritiesChanged;
    INFO_PRINTF1(_L("Wait for EPosModulesEventPrioritiesChanged event\n"));
    // Wait for event
    DoEventHandlingL();
    
    // make sure the test psy is visible:
    CPosModuleUpdate* visibileUpdate = CPosModuleUpdate::NewLC();
    visibileUpdate->SetUpdateVisibility(ETrue);
    db->UpdateModuleL(iUidTestPsy2, *visibileUpdate);
    CleanupStack::PopAndDestroy(visibileUpdate);
    
    // Update availability of test psy:
    CPosModuleUpdate* availUpdate = CPosModuleUpdate::NewLC();

 	TPositionModuleInfo info;
 	db -> GetModuleInfoL(iUidTestPsy2, info);
 	TBool available = info.IsAvailable();

	availUpdate->SetUpdateAvailability(!available);
	db->UpdateModuleL(iUidTestPsy2, *availUpdate);
	
	CleanupStack::PopAndDestroy(availUpdate);
	
	iExpectedEvent = EPosModulesEventAvailabilityChanged;
	INFO_PRINTF1(_L("Wait for EPosModulesEventAvailabilityChanged event\n"));
	// Wait for event
	DoEventHandlingL();
	
	// Update cost of test psy:
	CPosModuleUpdate* costUpdate = CPosModuleUpdate::NewLC();
 	
	TPositionQuality quality;
	info.GetPositionQuality(quality);
	TPositionQuality::TCostIndicator indicator = quality.CostIndicator();
	switch (indicator)
	{
	 case TPositionQuality::ECostUnknown:
		 indicator = TPositionQuality::ECostZero;
		 break;
	 case TPositionQuality::ECostZero:
		 indicator = TPositionQuality::ECostPossible;
		 break;
	 case TPositionQuality::ECostPossible:
		 indicator = TPositionQuality::ECostCharge;
		 break;
	 case TPositionQuality::ECostCharge:
		 indicator = TPositionQuality::ECostPossible;
		 break;
	 default:
		_LIT(KErrorAndLeave, "quality indicator does not exist");
		 LogErrorAndLeaveL(KErrorAndLeave);
	}

	costUpdate->SetUpdateCost(/*TPositionQuality::ECostCharge*/ indicator);
	db->UpdateModuleL(iUidTestPsy2, *costUpdate);
	CleanupStack::PopAndDestroy(costUpdate);
	
	iExpectedEvent = EPosModulesEventCostChanged;
	INFO_PRINTF1(_L("Wait for EPosModulesEventCostChanged event\n"));
	// Wait for event
	DoEventHandlingL();
	
	////////////////////////////////////
	// Install test psy:
	TPtrC sispath;
	GetStringFromConfig(ConfigSection(),KLbsSisInstallSisLocation,sispath);
	err = SISInstallPsyL(sispath);	
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
	// UnInstall test psy:
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
	
	db->RemoveObserver();
	CleanupStack::PopAndDestroy(db);
    }

TInt CT_LbsClientStepObserveInstall::SISUninstallPsyL(const TUid& appId)
	{
	CUISisAdaptor* ui = CUISisAdaptor::NewLC(Logger());

	TInt err = Swi::Launcher::Uninstall(*ui, appId);
	
	CleanupStack::PopAndDestroy(ui);
	
	INFO_PRINTF2(_L("Swi::Launcher::Uninstall() returned %d\n"), err);
	return err;
	}

TInt CT_LbsClientStepObserveInstall::SISInstallPsyL(const TDesC& aSisPath)
	{
	CUISisAdaptor *ui = CUISisAdaptor::NewLC(Logger());
	CInstallPrefs* prefs = CInstallPrefs::NewLC();

	TInt err = Swi::Launcher::Install(*ui, aSisPath, *prefs);

	CleanupStack::PopAndDestroy(prefs);
	CleanupStack::PopAndDestroy(ui);
	
	return err;
	}

// ---------------------------------------------------------
// CT_LbsClientStepObserveInstall::DoEventHandlingL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepObserveInstall::DoEventHandlingL()
    {
	CActiveScheduler::Start();
    CheckErrorL();
    }

// ---------------------------------------------------------
// CT_LbsClientStepObserveInstall::CheckErrorL
//
// ---------------------------------------------------------
//
void CT_LbsClientStepObserveInstall::CheckErrorL()
    {
    if (iError)
        {
        LogErrorAndLeaveL(iErrorText);
        }
    }

// ---------------------------------------------------------
// CT_LbsClientStepObserveInstall::HandleDatabaseChangeL
//
// ---------------------------------------------------------
void CT_LbsClientStepObserveInstall::HandleSettingsChangeL(TPosModulesEvent aModuleEvent)
    {
    INFO_PRINTF2(_L("HandleSettingsChangeL with event %d\n"), aModuleEvent.iType);
    CActiveScheduler::Stop();
    if (aModuleEvent.iType != iExpectedEvent)
        {
        _LIT(KErrorText, "Unexpected event received");
        iErrorText = KErrorText;
        iError = ETrue;
    	SetTestStepResult(EFail);
        }
    }

