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
// The test step testing simple notify position update scenarios for the AGPS PSY.
// 
//

#include "te_lbsagpspsyposupdatestep.h"
#include "te_lbspsyrequest.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsystaticdata.h"

CTe_LbsAgpsPsyPosUpdateStep* CTe_LbsAgpsPsyPosUpdateStep::NewL(MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsAgpsPsyPosUpdateStep* self = new(ELeave) CTe_LbsAgpsPsyPosUpdateStep(aPsyStaticData);
	return self;
	}
	
CTe_LbsAgpsPsyPosUpdateStep::CTe_LbsAgpsPsyPosUpdateStep(MTe_LbsPsyStaticData* aPsyStaticData) :
CTe_LbsNotifyPositionUpdateStep(aPsyStaticData)
	{
	}

void CTe_LbsAgpsPsyPosUpdateStep::ReallyDoTestStepImpL()
	{
	
	CTe_LbsNotifyPositionUpdateStep::ReallyDoTestStepImpL();
	
	//POSITION UPDATE WITH REF POS (Scenario 4.0)
	INFO_PRINTF1(_L("POSITION UPDATE WITH REF POS (Scenario 4.0) START"));
	INFO_PRINTF1(_L("Create positioner and the position update object"));
	CTe_PsyPositioner* positioner = MakePositionerLC(); 
	CTe_PsyRequest* updateRequest = CTe_PsyRequest::NewLC(*this);
	
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
	updateRequest->NotifyPsyRequest();
	positioner->NotifyPositionUpdate();
	StartSchedulerWaitL(KUsualTimeout, 1);
			
	updateRequest->ExpectDefaultUpdateRequest();
	updateRequest->UpdateRequest().NewClient() = ETrue;
	updateRequest->CheckPosUpdateOrReqReceivedL();
	updateRequest->NotifyPsyRequest();
			
	INFO_PRINTF1(_L("Set a reference position update on the bus ALM->PSY"));
	CTe_PsyResponse* posUpdate = 
		CTe_PsyResponse::IssueRefPosUpdateLC(TPositionModuleInfo::ETechnologyNetwork);
	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("The active scheduler stopped"));
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	updateRequest->CheckNothingReceivedL();
	CleanupStack::PopAndDestroy(posUpdate);
			
	positioner->NotifyPositionUpdate();
	posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(TPositionModuleInfo::ETechnologyTerminal|
				TPositionModuleInfo::ETechnologyAssisted);
	StartSchedulerWaitL(KUsualTimeout, 1);
			
	updateRequest->ExpectDefaultUpdateRequest();
	updateRequest->CheckPosUpdateOrReqReceivedL();		
			
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	INFO_PRINTF1(_L("POSITION UPDATE WITH REF POS START (Scenario 4.0) END"));
	
	//To make power stanby timer work
	StartSchedulerWaitL(2*KUsualTimeout, 1);
	
	//SETTING MAXAGE AND USING CACHED REF POS UPDATE (Scenario 4.1)
	INFO_PRINTF1(_L("SETTING MAXAGE AND USING CACHED REF POS UPDATE (Scenario 4.1) START"));
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
		
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	updateRequest->CheckNothingReceivedL();
	updateRequest->Cancel();
	CleanupStack::PopAndDestroy(posUpdate);
	positioner->SetMaxAge(0);
	INFO_PRINTF1(_L("SETTING MAXAGE AND USING CACHED REF POS UPDATE (Scenario 4.1) END"));
			
		
	//POSITION UPDATE WITH REF POS & FINAL NET POS (Scenario 4.2)
	INFO_PRINTF1(_L("POSITION UPDATE WITH REF POS & FINAL NET POS (Scenario 4.2) START"));
	INFO_PRINTF1(_L("Create positioner and the position update object"));
				
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
	updateRequest->NotifyPsyRequest();
	positioner->NotifyPositionUpdate();
	
	WaitCheckNotifyReqReceivedL(*updateRequest);
				
	INFO_PRINTF1(_L("Set a reference position update on the bus ALM->PSY"));
	posUpdate = CTe_PsyResponse::IssueRefPosUpdateLC(TPositionModuleInfo::ETechnologyNetwork);
	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("The active scheduler stopped"));
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	updateRequest->CheckNothingReceivedL();
	CleanupStack::PopAndDestroy(posUpdate);
				
	positioner->NotifyPositionUpdate();
	posUpdate = CTe_PsyResponse::IssueFinalNetPosUpdateLC(TPositionModuleInfo::ETechnologyNetwork|
				TPositionModuleInfo::ETechnologyAssisted);
	StartSchedulerWaitL(KUsualTimeout, 1);
				
	updateRequest->ExpectDefaultCancelRequest();
	updateRequest->CheckPosUpdateOrReqReceivedL();		
				
	positioner->CheckPosUpdateReceivedL(*posUpdate);
				
	INFO_PRINTF1(_L("POSITION UPDATE WITH REF POS & FINAL NET POS (Scenario 4.2) END"));
	
	//To make power stanby timer work
	StartSchedulerWaitL(2*KUsualTimeout, 1);
	
	//SETTING MAXAGE AND USING CACHED FN POS (Scenario 4.3) UPDATE
	INFO_PRINTF1(_L("SETTING MAXAGE AND USING CACHED FN POS UPDATE (Scenario 4.3) START"));
	INFO_PRINTF1(_L("Create positioner and the position update object"));
			
	someTimeAgo.UniversalTime();
	someTimeAgo-=(TTimeIntervalSeconds(KOldPosUpdateAge+KUsualTimeout));
	positioner->SetMaxAge(someTimeAgo);
			
	updateRequest->NotifyPsyRequest();
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
	positioner->NotifyPositionUpdate();
			
	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("The active scheduler stopped"));
			
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	updateRequest->CheckNothingReceivedL();
	updateRequest->Cancel();
	CleanupStack::PopAndDestroy(posUpdate);
	positioner->SetMaxAge(0);
	INFO_PRINTF1(_L("SETTING MAXAGE AND USING CACHED FN POS UPDATE (Scenario 4.3) END"));
		
	
	CleanupStack::PopAndDestroy(updateRequest);
	CleanupStack::PopAndDestroy(positioner);
		
	INFO_PRINTF1(_L("CTe_LbsAgpsPsyPosUpdateStep::ReallyDoTestStepL() end"));
		
	if(TestStepResult()!=EPass)
		{
		User::Leave(EFail);
		}
	}
	
