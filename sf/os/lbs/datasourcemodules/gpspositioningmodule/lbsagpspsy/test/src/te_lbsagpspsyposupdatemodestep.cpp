// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The test step testing notify position update scenarios with gps + agps for the AGPS PSY.
// Module caps are configured in the script. Default Gps Mode is set in admin. 
// 2 positioners are opened; the gps one and the agps one and each does a request.
// we verify that the correct gps mode is set in the request. See table in design doc/arch spec for details
// 
//

#include <lbs/lbsadmin.h>
#include "lbsextendmoduleinfo.h"

#include "te_lbsagpspsyposupdatemodestep.h"
#include "te_lbspsyutils.h"
#include "te_lbspsyrequest.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsystaticdata.h"

// constants/lits
_LIT(KCurrMode, "CurrMode");
_LIT(KNewMode, "NewMode");
_LIT(KModeToManager, "NewModeToManager");
_LIT(KExpectError, "ExpectError");

// lrm TODO - this is ugly, step shouldn't need to know about suite, try to think of alternative:
//const TUid  KServerUid = {0x10285ACB};

const TInt KPosAGPSPSYImpl = 0x101fe98c;
const TInt KPosGPSPSYImpl = 0x101fe98a;


CTe_LbsAgpsPsyPosUpdateModeStep* CTe_LbsAgpsPsyPosUpdateModeStep::NewL(MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsAgpsPsyPosUpdateModeStep* self = new(ELeave) CTe_LbsAgpsPsyPosUpdateModeStep(aPsyStaticData);
	return self;
	}
	
CTe_LbsAgpsPsyPosUpdateModeStep::CTe_LbsAgpsPsyPosUpdateModeStep(MTe_LbsPsyStaticData* aPsyStaticData) :
CTe_PsyBaseStep(aPsyStaticData)
	{
	SetTestStepName(KLbsNotifyPositionUpdateModeStep);
	}

void CTe_LbsAgpsPsyPosUpdateModeStep::ReallyDoTestStepImpL()
	{
	INFO_PRINTF1(_L("CTe_LbsAgpsPsyPosUpdateModeStep::ReallyDoTestStepImpL()"));
	// Close and re-define the internal buses and quality profile so that device caps are refreshed:

	CTe_SystemStarter::DeleteLbsPropertiesL();
	CTe_SystemStarter::DefineLbsPropertiesL();
	
	// Get settings from config
	TPositionModuleInfo::TTechnologyType currMode = 0, newMode, newModeToManager = 0;
	TInt expectedError = KErrNone;
	GetIntFromConfig(ConfigSection(), KCurrMode, reinterpret_cast<TInt&>(currMode));
	User::LeaveIfError(GetIntFromConfig(ConfigSection(), KNewMode, reinterpret_cast<TInt&>(newMode)));
	GetIntFromConfig(ConfigSection(), KModeToManager, reinterpret_cast<TInt&>(newModeToManager));
	GetIntFromConfig(ConfigSection(), KExpectError, reinterpret_cast<TInt&>(expectedError));

	CTe_PsyPositioner* gpsPositionerCurr = NULL;
	CTe_PsyPositioner* gpsPositionerNew = NULL;
	CTe_PsyPositioner* aGpsPositionerCurr = NULL;
	CTe_PsyPositioner* aGpsPositionerNew = NULL;
	CTe_PsyRequest* updateRequest = CTe_PsyRequest::NewLC(*this);
	
	// Create the connection to LbsAdmin to set the default gpsmode
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);
	
	// maps technology type to admin setting we (may) need to achieve it:
	CLbsAdmin::TGpsMode defaultGpsModesTable[] = {CLbsAdmin::EGpsModeUnknown, CLbsAdmin::EGpsAutonomous, CLbsAdmin::EGpsModeUnknown, 
												  CLbsAdmin::EGpsModeUnknown, CLbsAdmin::EGpsModeUnknown, CLbsAdmin::EGpsPreferTerminalBased, 
												  CLbsAdmin::EGpsPreferTerminalAssisted};
	
	// Outstanding request
	if(currMode)
		{
		INFO_PRINTF2(_L("First request mode = %d."), currMode);
		__ASSERT_DEBUG(defaultGpsModesTable[currMode] != CLbsAdmin::EGpsModeUnknown, User::Invariant());
		// just some logging:
		switch(defaultGpsModesTable[currMode])
			{
			case CLbsAdmin::EGpsAutonomous:
				INFO_PRINTF1(_L("Set default gps mode to EGpsAutonomous in admin"));
				break;
			case CLbsAdmin::EGpsPreferTerminalBased:
				INFO_PRINTF1(_L("Set default gps mode to EGpsPreferTerminalBased in admin"));
				break;		
			case CLbsAdmin::EGpsPreferTerminalAssisted:
				INFO_PRINTF1(_L("Set default gps mode to EGpsPreferTerminalAssisted in admin"));
				break;				
			}
		admin->Set(KLbsSettingHomeGpsMode, defaultGpsModesTable[currMode]);
		admin->Set(KLbsSettingRoamingGpsMode, defaultGpsModesTable[currMode]);	
		
		// get notification of request to the agps manager:			
		updateRequest->NotifyPsyRequest();
		
		if( currMode == TPositionModuleInfo::ETechnologyTerminal)	// want autonomous request
			{
			// Make a request using gps positioner:
			INFO_PRINTF1(_L("Make NPUD request from gps positioner1"));
			gpsPositionerCurr = MakePositionerLC(KPosGPSPSYImpl);
			gpsPositionerCurr->NotifyPositionUpdate();			
			}
		else	
			{
			// Make a request using agps positioner:
			INFO_PRINTF1(_L("Make NPUD request from agps positioner1"));
			aGpsPositionerCurr = MakePositionerLC(KPosAGPSPSYImpl);
			aGpsPositionerCurr->NotifyPositionUpdate();			
			}
	
		StartSchedulerWaitL(KUsualTimeout, 1);
		INFO_PRINTF2(_L("Expect request to manager with request method = %d"), currMode);	
		updateRequest->ExpectUpdateRequestWithRequestMethod(currMode);
		updateRequest->UpdateRequest().NewClient() = ETrue;
		updateRequest->CheckPosUpdateReqReceivedWithMethodL();	
		}
	
	// New request
	__ASSERT_DEBUG(newMode, User::Invariant());		
	INFO_PRINTF2(_L("New request mode = %d."), newMode);
	// just some logging:
	switch(defaultGpsModesTable[newMode])
		{
		case CLbsAdmin::EGpsAutonomous:
			INFO_PRINTF1(_L("Set default gps mode to EGpsAutonomous in admin"));
			break;
		case CLbsAdmin::EGpsPreferTerminalBased:
			INFO_PRINTF1(_L("Set default gps mode to EGpsPreferTerminalBased in admin"));
			break;		
		case CLbsAdmin::EGpsPreferTerminalAssisted:
			INFO_PRINTF1(_L("Set default gps mode to EGpsPreferTerminalAssisted in admin"));
			break;				
		}	
	admin->Set(KLbsSettingHomeGpsMode, defaultGpsModesTable[newMode]);
	admin->Set(KLbsSettingRoamingGpsMode, defaultGpsModesTable[newMode]);
	
	// get notification of request to the agps manager:			
	updateRequest->NotifyPsyRequest();
	
	if( newMode == TPositionModuleInfo::ETechnologyTerminal)	// want autonomous request
		{
		// Make a request
		gpsPositionerNew = MakePositionerLC(KPosGPSPSYImpl);
		INFO_PRINTF1(_L("Make NPUD request from agps positioner2"));
		gpsPositionerNew->NotifyPositionUpdate();		
		}
	else
		{
		// Make a request
		aGpsPositionerNew = MakePositionerLC(KPosAGPSPSYImpl);
		INFO_PRINTF1(_L("Make NPUD request from agps positioner2"));
		aGpsPositionerNew->NotifyPositionUpdate();		
		}
	
	StartSchedulerWaitL(KUsualTimeout, 1);	
	
	if(expectedError != KErrNone)
		{
		INFO_PRINTF2(_L("Expect Error %d"), expectedError);
		updateRequest->CheckPosUpdRequestNotReceivedL();
		if(gpsPositionerNew)
			{
			__ASSERT_DEBUG(!aGpsPositionerNew, User::Invariant());	// we should never have both of these
			gpsPositionerNew->CheckPosUpdateFailedL(expectedError);
			}
		else
			{
			aGpsPositionerNew->CheckPosUpdateFailedL(expectedError);
			}
		if(gpsPositionerCurr || aGpsPositionerCurr)
			{
			CTe_PsyResponse* posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(newModeToManager);
			StartSchedulerWaitL(KUsualTimeout, 1);
			if(gpsPositionerCurr)
				{
				gpsPositionerCurr->CheckPosUpdateReceivedL(*posUpdate);
				}
			else if(aGpsPositionerCurr)
				{
				aGpsPositionerCurr->CheckPosUpdateReceivedL(*posUpdate);
				}	
			CleanupStack::PopAndDestroy(posUpdate);	
			}
		}
	else
		{
		if(newModeToManager)
			{
			INFO_PRINTF2(_L("Expect request to manager with request method = %d"), newModeToManager);	
			updateRequest->ExpectUpdateRequestWithRequestMethod(newModeToManager);
			updateRequest->UpdateRequest().NewClient() = ETrue;
			updateRequest->CheckPosUpdateReqReceivedWithMethodL();	
			}
		else
			{
			INFO_PRINTF1(_L("Not expecting new request to manager"));
			updateRequest->CheckPosUpdRequestNotReceivedL();
			}
		
		// respond to request and verify received by client:
		CTe_PsyResponse* posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(newModeToManager);
		StartSchedulerWaitL(KUsualTimeout, 1);
		if(gpsPositionerCurr)
			{
			gpsPositionerCurr->CheckPosUpdateReceivedL(*posUpdate);
			}
		else if(aGpsPositionerCurr)
			{
			aGpsPositionerCurr->CheckPosUpdateReceivedL(*posUpdate);
			}
		if(gpsPositionerNew)
			{
			gpsPositionerNew->CheckPosUpdateReceivedL(*posUpdate);
			}
		else
			{
			__ASSERT_DEBUG(aGpsPositionerNew, User::Invariant());	// should always be one of these 2 positioners
			aGpsPositionerNew->CheckPosUpdateReceivedL(*posUpdate);
			}
		CleanupStack::PopAndDestroy(posUpdate);		
		}
	
	if(gpsPositionerNew)
		CleanupStack::PopAndDestroy(gpsPositionerNew);	
	else
		CleanupStack::PopAndDestroy(aGpsPositionerNew);	
	if(gpsPositionerCurr)
		CleanupStack::PopAndDestroy(gpsPositionerCurr);
	else if(aGpsPositionerCurr)
		CleanupStack::PopAndDestroy(aGpsPositionerCurr);
	
	CleanupStack::PopAndDestroy(admin);
	CleanupStack::PopAndDestroy(updateRequest);	
	
	
	INFO_PRINTF1(_L("CTe_LbsAgpsPsyPosUpdateModeStep::ReallyDoTestStepL() end"));
	
	if(TestStepResult()!=EPass)
		{
		User::Leave(EFail);
		}
	}
	
