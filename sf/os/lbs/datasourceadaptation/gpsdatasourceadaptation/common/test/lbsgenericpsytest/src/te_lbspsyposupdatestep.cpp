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
// The test step testing simple notify position update scenarios.
// 
//

#include <e32base.h>
#include <lbs/epos_cpositioner.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>

#include "te_lbspsyposupdatestep.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsyrequest.h"

//#include "lbsnetprotocolproxy.h"

EXPORT_C CTe_LbsNotifyPositionUpdateStep* CTe_LbsNotifyPositionUpdateStep::NewL(
		MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsNotifyPositionUpdateStep* self = 
		new(ELeave) CTe_LbsNotifyPositionUpdateStep(aPsyStaticData);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTe_LbsNotifyPositionUpdateStep::CTe_LbsNotifyPositionUpdateStep(
		MTe_LbsPsyStaticData* aPsyStaticData) :
CTe_PsyBaseStep(aPsyStaticData)
	{
	SetTestStepName(KLbsNotifyPositionUpdateStep);
	}

EXPORT_C CTe_LbsNotifyPositionUpdateStep::~CTe_LbsNotifyPositionUpdateStep()
	{
	}

EXPORT_C void CTe_LbsNotifyPositionUpdateStep::ReallyDoTestStepImpL()
	{
	INFO_PRINTF1(_L("CTe_LbsNotifyPositionUpdateStep::ReallyDoTestStepL() start"));
	
	//SIMPLE POSITION UPDATE (Scenario 1.0)
	INFO_PRINTF1(_L("SIMPLE POSITION UPDATE (Scenario 1.0) START"));
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
	
	CTe_PsyResponse* posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(
			TPositionModuleInfo::ETechnologyTerminal);
	StartSchedulerWaitL(KUsualTimeout, 1);
	
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	CleanupStack::PopAndDestroy(posUpdate);
	
	INFO_PRINTF1(_L("SIMPLE POSITION UPDATE (Scenario 1.0) END"));
	
	
	//TWO SIMPLE POSITION UPDATES (Scenario 1.1)
	INFO_PRINTF1(_L("TWO SIMPLE POSITION UPDATES (Scenario 1.1) START"));
	INFO_PRINTF1(_L("Create the second positioner and the position update object"));
	CTe_PsyPositioner* positioner2 = MakePositionerLC();
	
	updateRequest->NotifyPsyRequest();	
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() for the first positioner PSY->ALM"));
	positioner->NotifyPositionUpdate();
	
	WaitCheckNotifyReqReceivedL(*updateRequest);
	
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() for the second positioner PSY->ALM"));
	positioner2->NotifyPositionUpdate();
	
	//Checking that no new request is submitted to the manager
	StartSchedulerWaitL(KUsualTimeout, 0);
	
	INFO_PRINTF1(_L("Set a position update on the bus ALM->PSY"));
	posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(TPositionModuleInfo::ETechnologyTerminal);
	
	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 2);
	INFO_PRINTF1(_L("The active scheduler stopped"));
	positioner->CheckPosUpdateReceivedL(*posUpdate);	
	positioner2->CheckPosUpdateReceivedL(*posUpdate);
	CleanupStack::PopAndDestroy(posUpdate);
	CleanupStack::PopAndDestroy(positioner2);
	INFO_PRINTF1(_L("TWO SIMPLE POSITION UPDATES (Scenario 1.1) END"));	
	
	//CANCEL A POSITION UPDATE (Scenario 1.2)
	INFO_PRINTF1(_L("CANCEL A POSITION UPDATE (Scenario 1.2) START"));
	
	updateRequest->NotifyPsyRequest();
	positioner->NotifyPositionUpdate();
	WaitCheckNotifyReqReceivedL(*updateRequest);
	positioner->CancelNotifyPositionUpdate();
	
	WaitCheckCancelReceivedL(*updateRequest);
		
	positioner->CheckPosUpdateFailedL(KErrCancel); 
	
	//We simulate here that a pos update is received after the cancel.
	//Though there is no immediate checks available here, we check that it does not panic.
	//Also running the following tests, we check that the positioner remained in the correct state.
	posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(TPositionModuleInfo::ETechnologyTerminal);
	User::After(10 * 1000 * 1000);	// added to ensure that this update does not interfere with the next test
	CleanupStack::PopAndDestroy(posUpdate);
	INFO_PRINTF1(_L("CANCEL A POSITION UPDATE (Scenario 1.2) END"));
	
	//PARTIAL UPDATE (Scenario 1.3)
	INFO_PRINTF1(_L("PARTIAL UPDATE (Scenario 1.3) START"));
	positioner->SetAcceptPartialUpdates(ETrue);
	positioner->SetUpdateTimeout(KWarmDownInterval);
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() for the first positioner PSY->ALM"));
	updateRequest->NotifyPsyRequest();
	positioner->NotifyPositionUpdate();
	
	StartSchedulerWaitL(KUsualTimeout, 1);
	updateRequest->ExpectDefaultUpdateRequest();
	updateRequest->UpdateRequest().RequestQuality().SetMaxFixTime(KWarmDownInterval);
	updateRequest->CheckPosUpdateOrReqReceivedL();
		
	INFO_PRINTF1(_L("Set a position update on the bus ALM->PSY"));
	CTe_PsyResponse* partialUpdate = CTe_PsyResponse::IssuePartialPosUpdateLC();
		
	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("The active scheduler stopped"));
	positioner->CheckPosUpdateReceivedL(*partialUpdate);
	CleanupStack::PopAndDestroy(partialUpdate);
	//To make warm down timer work
	StartSchedulerWaitL(KWarmDownInterval, 0);
	
	INFO_PRINTF1(_L("PARTIAL UPDATE (Scenario 1.3) END"));
	
	//TEST METHODS WHICH ARE NEVER CALLED IN THE PRODUCTION CODE (Scenario 1.4)
	INFO_PRINTF1(_L("TEST METHODS WHICH ARE NEVER CALLED IN THE PRODUCTION CODE (Scenario 1.4) START"));
	positioner->SimulateTimerErrorsL();
	INFO_PRINTF1(_L("TEST METHODS WHICH ARE NEVER CALLED IN THE PRODUCTION CODE (Scenario 1.4) END"));
	
	CleanupStack::PopAndDestroy(updateRequest);
	CleanupStack::PopAndDestroy(positioner);
	
	INFO_PRINTF1(_L("CTe_LbsNotifyPositionUpdateStep::ReallyDoTestStepL() end"));
	
	if(TestStepResult()!=EPass)
		{
		User::Leave(EFail);
		}
	
	}
