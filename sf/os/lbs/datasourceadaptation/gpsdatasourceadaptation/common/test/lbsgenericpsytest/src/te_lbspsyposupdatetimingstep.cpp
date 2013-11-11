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
// te_lbspsyposupdatettimingstep.cpp
// // The test step testing timing issues of notify position update scenarios.
// 
//

#include <e32base.h>
#include <lbs/epos_cpositioner.h>

#include "te_lbspsyposupdatetimingstep.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsyrequest.h"
  

EXPORT_C CTe_LbsNotifyPositionUpdateTimingStep* CTe_LbsNotifyPositionUpdateTimingStep::NewL(
		MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsNotifyPositionUpdateTimingStep* self = 
		new(ELeave) CTe_LbsNotifyPositionUpdateTimingStep(aPsyStaticData);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTe_LbsNotifyPositionUpdateTimingStep::CTe_LbsNotifyPositionUpdateTimingStep(
		MTe_LbsPsyStaticData* aPsyStaticData) : CTe_PsyBaseStep(aPsyStaticData)
	{
	SetTestStepName(KLbsNotifyPositionUpdateTimingStep);
	}

EXPORT_C CTe_LbsNotifyPositionUpdateTimingStep::~CTe_LbsNotifyPositionUpdateTimingStep()
	{
	}

void CTe_LbsNotifyPositionUpdateTimingStep::ReallyDoTestStepImpL()
	{
	INFO_PRINTF1(_L("CTe_LbsNotifyPositionUpdateTimingStep::ReallyDoTestStepL() start"));
		
	StartSchedulerWaitL(1000000*KOldPosUpdateAge + KUsualTimeout, 0);
	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	//HANDLING THE OLD POSITION UPDATE (Scenario 2.0)
	INFO_PRINTF1(_L("HANDLING THE OLD POSITION UPDATE (Scenario 2.0) START"));
	INFO_PRINTF1(_L("Create positioner and the position update object"));
	
	CTe_PsyPositioner* positioner = MakePositionerLC(); 
	CTe_PsyRequest* updateRequest = CTe_PsyRequest::NewLC(*this);
	
	updateRequest->NotifyPsyRequest();
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
	positioner->NotifyPositionUpdate();
	
	StartSchedulerWaitL(KUsualTimeout, 1);
	updateRequest->ExpectDefaultUpdateRequest();
	updateRequest->UpdateRequest().NewClient() = ETrue;
	updateRequest->CheckPosUpdateOrReqReceivedL();
		
	INFO_PRINTF1(_L("Set an old position update on the bus ALM->PSY"));
	CTe_PsyResponse* outdatedUpdate = CTe_PsyResponse::IssueOutdatedPosUpdateLC();
		
	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 0);
	INFO_PRINTF1(_L("The active scheduler stopped"));
		
	positioner->CheckPosUpdateNotReceivedL();
	
	updateRequest->NotifyPsyRequest();
	positioner->Cancel();
	
	WaitCheckCancelReceivedL(*updateRequest);
	
	INFO_PRINTF1(_L("HANDLING THE OLD POSITION UPDATE (Scenario 2.0) END"));
	
	//temporarily commented as the production code is not implemented properly yet 
	
	//SETTING MAXAGE AND USING CACHED UPDATE (Scenario 2.1)
	INFO_PRINTF1(_L("SETTING MAXAGE AND USING CACHED UPDATE (Scenario 2.1) START"));
	INFO_PRINTF1(_L("Create positioner and the position update object"));
	
	TTime someTimeAgo;
	someTimeAgo.UniversalTime();
	someTimeAgo-=(TTimeIntervalSeconds(KOldPosUpdateAge+KUsualTimeout));
	positioner->SetMaxAge(someTimeAgo);
	
	updateRequest->NotifyPsyRequest();
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
	positioner->NotifyPositionUpdate();
	
	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("The active scheduler stopped"));
	
	positioner->CheckPosUpdateReceivedL(*outdatedUpdate);
	updateRequest->CheckNothingReceivedL();
	updateRequest->Cancel();
	CleanupStack::PopAndDestroy(outdatedUpdate);
	positioner->SetMaxAge(0);
	INFO_PRINTF1(_L("SETTING MAXAGE AND USING CACHED UPDATE (Scenario 2.1) END"));
	
	
	//SETTING MAXAGE, NO CACHED UPDATE (Scenario 2.2)
	INFO_PRINTF1(_L("SETTING MAXAGE, NO CACHED UPDATE (Scenario 2.2) START"));
	TTime now;
	now.UniversalTime();
	positioner->SetMaxAge(now);
		
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
	updateRequest->NotifyPsyRequest();
	positioner->NotifyPositionUpdate();
	StartSchedulerWaitL(KUsualTimeout, 1);
		
	updateRequest->ExpectDefaultUpdateRequest();
	updateRequest->CheckPosUpdateOrReqReceivedL();
		
	CTe_PsyResponse* posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(
				TPositionModuleInfo::ETechnologyTerminal);
	StartSchedulerWaitL(KUsualTimeout, 1);
		
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	CleanupStack::PopAndDestroy(posUpdate);
	positioner->SetMaxAge(0);
	INFO_PRINTF1(_L("SETTING MAXAGE, NO CACHED UPDATE (Scenario 2.2) END"));
		

	//SIMPLE TRACKING SCENARIO (Scenario 2.3)
	INFO_PRINTF1(_L("SIMPLE TRACKING SCENARIO (Scenario 2.3) START"));
	TESTL(positioner->TrackingOverridden());
	
	positioner->StartTrackingL(KTrackingInterval);
	
	StartSchedulerWaitL(KTrackingInterval-KUsualTimeout, 0);
	positioner->NotifyPositionUpdate();
	StartSchedulerWaitL(2*KUsualTimeout, 1);
	
	INFO_PRINTF1(_L("Set a position update on the bus ALM->PSY"));
	CTe_PsyResponse* simpleUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(TPositionModuleInfo::ETechnologyTerminal);
	
	StartSchedulerWaitL(KUsualTimeout, 1);
	positioner->CheckPosUpdateReceivedL(*simpleUpdate);	
	positioner->StopTracking();
	
	CleanupStack::PopAndDestroy(simpleUpdate);
	INFO_PRINTF1(_L("SIMPLE TRACKING SCENARIO (Scenario 2.3) END"));
	
	
	//TRACKING SCENARIO WITH LATE REQUEST (Scenario 2.4)
	INFO_PRINTF1(_L("TRACKING SCENARIO WITH LATE REQUEST (Scenario 2.4) START"));
	positioner->StartTrackingL(KTrackingInterval);
	StartSchedulerWaitL(KTrackingInterval-KUsualTimeout, 0);
	positioner->NotifyPositionUpdate();
	StartSchedulerWaitL(2*KUsualTimeout, 1);
	INFO_PRINTF1(_L("Set a position update on the bus ALM->PSY"));
	simpleUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(TPositionModuleInfo::ETechnologyTerminal);
	StartSchedulerWaitL(KUsualTimeout, 1);
	positioner->CheckPosUpdateReceivedL(*simpleUpdate);
	CleanupStack::PopAndDestroy(simpleUpdate);
	StartSchedulerWaitL(KTrackingInterval+KUsualTimeout, 0);
	positioner->NotifyPositionUpdate();
	StartSchedulerWaitL(KTrackingInterval, 0);
	simpleUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(TPositionModuleInfo::ETechnologyTerminal);
	StartSchedulerWaitL(KUsualTimeout, 1);
	positioner->CheckPosUpdateReceivedL(*simpleUpdate);
	positioner->StopTracking();
	CleanupStack::PopAndDestroy(simpleUpdate);
	INFO_PRINTF1(_L("TRACKING SCENARIO WITH LATE REQUEST (Scenario 2.4) END"));
			
	
	CleanupStack::PopAndDestroy(updateRequest);
	CleanupStack::PopAndDestroy(positioner);
		
	INFO_PRINTF1(_L("CTe_LbsNotifyPositionUpdateTimingStep::ReallyDoTestStepL() end"));
		
	if(TestStepResult()!=EPass)
		{
		User::Leave(EFail);
		}
	}

