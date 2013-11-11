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
// te_lbspsyposupdateerrstep.cpp
// The test step testing the notify position update scenarios with errors.
// 
//

#include "te_lbsagpspsyposupdateerrstep.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsyrequest.h"

#include "psypanic.h"

EXPORT_C CTe_LbsAgpsNotifyPositionUpdateErrStep* CTe_LbsAgpsNotifyPositionUpdateErrStep::NewL(
		MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsAgpsNotifyPositionUpdateErrStep* self = new(ELeave) CTe_LbsAgpsNotifyPositionUpdateErrStep(aPsyStaticData);
	return self;
	}
	
CTe_LbsAgpsNotifyPositionUpdateErrStep::CTe_LbsAgpsNotifyPositionUpdateErrStep(MTe_LbsPsyStaticData* aPsyStaticData) 
: CTe_LbsNotifyPositionUpdateErrStep(aPsyStaticData)
	{
	}

EXPORT_C CTe_LbsAgpsNotifyPositionUpdateErrStep::~CTe_LbsAgpsNotifyPositionUpdateErrStep()
	{
	}
	
void CTe_LbsAgpsNotifyPositionUpdateErrStep::ReallyDoTestStepImpL()
	{
	CTe_LbsNotifyPositionUpdateErrStep::ReallyDoTestStepImpL();
	
	//POSITION UPDATE FUTILE ERROR - AUTONOMOUS MODE (Scenario 3.1)
	CTe_PsyPositioner* positioner = MakePositionerLC(); 
	CTe_PsyRequest* updateRequest = CTe_PsyRequest::NewLC(*this);
	
	INFO_PRINTF1(_L("POSITION UPDATE FUTILE ERROR - AUTONOMOUS MODE (Scenario 3.1) START"));
	INFO_PRINTF1(_L("Create positioner and the position update object"));
					
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
	updateRequest->NotifyPsyRequest();
	positioner->NotifyPositionUpdate();
	StartSchedulerWaitL(KUsualTimeout, 1);
			
	updateRequest->ExpectDefaultUpdateRequest();
	updateRequest->UpdateRequest().NewClient() = ETrue;	
	updateRequest->CheckPosUpdateOrReqReceivedL();
		
	INFO_PRINTF1(_L("Set a position update on the bus ALM->PSY"));
	CTe_PsyResponse* errUpdate = CTe_PsyResponse::IssueErrPosUpdateLC(KPositionCalculationFutile,
			TPositionModuleInfo::ETechnologyTerminal);

	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("The active scheduler stopped"));
		
	positioner->CheckPosUpdateFailedL(KPositionQualityLoss);
	CleanupStack::PopAndDestroy(errUpdate);
		
	INFO_PRINTF1(_L("POSITION UPDATE FUTILE ERROR - AUTONOMOUS MODE (Scenario 3.1) END"));

	CleanupStack::PopAndDestroy(updateRequest);
	CleanupStack::PopAndDestroy(positioner);		
	
	}
