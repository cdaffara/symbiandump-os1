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
// The test step testing the notify position update scenarios with errors.
// 
//

#include "te_lbspsyposupdateerrstep.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsyrequest.h"

#include "psypanic.h"


EXPORT_C CTe_LbsNotifyPositionUpdateErrStep* CTe_LbsNotifyPositionUpdateErrStep::NewL(
		MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsNotifyPositionUpdateErrStep* self = 
		new(ELeave) CTe_LbsNotifyPositionUpdateErrStep(aPsyStaticData);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CTe_LbsNotifyPositionUpdateErrStep::CTe_LbsNotifyPositionUpdateErrStep(
		MTe_LbsPsyStaticData* aPsyStaticData) : CTe_PsyBaseStep(aPsyStaticData)
	{
	SetTestStepName(KLbsNotifyPositionUpdateErrStep);
	}

EXPORT_C CTe_LbsNotifyPositionUpdateErrStep::~CTe_LbsNotifyPositionUpdateErrStep()
	{
	}
	
EXPORT_C void CTe_LbsNotifyPositionUpdateErrStep::ReallyDoTestStepImpL()
	{
	//SIMPLE POSITION UPDATE ERROR (Scenario 3.0)
	INFO_PRINTF1(_L("SIMPLE POSITION UPDATE ERROR (Scenario 3.0) START"));
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
	
	INFO_PRINTF1(_L("Set a position update on the bus ALM->PSY"));
	CTe_PsyResponse* errUpdate = CTe_PsyResponse::IssueErrPosUpdateLC(KErrNoMemory,
			TPositionModuleInfo::ETechnologyTerminal);

	INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("The active scheduler stopped"));
	
	positioner->CheckPosUpdateFailedL(KErrNoMemory);
	CleanupStack::PopAndDestroy(errUpdate);
	
	INFO_PRINTF1(_L("SIMPLE POSITION UPDATE ERROR (Scenario 3.0) END"));
	
	__UHEAP_FAILNEXT(0);	
	TInt* marker = new TInt(0);
	delete marker;
		
	if(marker) //it is the urel OS version - skipping the memory leak tests
		{
		CleanupStack::PopAndDestroy(updateRequest);
		CleanupStack::PopAndDestroy(positioner);
		return;
		}
	
	//CPOSITIONER CREATION/DESTRUCTION WITH MEMORY LEAKS (Scenario 3.2)
	INFO_PRINTF1(_L("CPOSITIONER CREATION/DESTRUCTION WITH MEMORY LEAKS (Scenario 3.2) START"));	
	for(TInt i=1; marker; i++)
		{			
		INFO_PRINTF1(_L("Calling the MakePositionerLC"));
		__UHEAP_FAILNEXT(i);
		CTe_PsyPositioner* positioner = NULL;
		TRAPD(err, positioner = MakePositionerLC(); CleanupStack::PopAndDestroy(positioner));
		marker = new TInt(0);
		delete marker;
			
		if(marker) //Leave has happened inside the MakePositionerLC
			{
			INFO_PRINTF4(_L("Leave has happened inside the MakePositionerLC, i=%d err=%d positioner=0x%x"),
					i, err, positioner);
			if(err==KErrNone && !positioner)
				{
				User::Leave(EFail);
				}
			}
		else//Leave hasn't happened inside the MakePositionerLC - not interested
			{
			INFO_PRINTF2(_L("No leave has happened inside the MakePositionerLC - stop the test, i=%d"), i);
			break;
			}
		}
	INFO_PRINTF1(_L("CPOSITIONER CREATION/DESTRUCTION WITH MEMORY LEAKS (Scenario 3.2) END"));
	
	
	//SIMPLE POSITION UPDATE WITH OOM WHILE CALLING NotifyPositionUpdate (Scenario 3.3)
	INFO_PRINTF1(_L("SIMPLE POSITION UPDATE WITH OOM WHILE CALLING NotifyPositionUpdate (Scenario 3.3) START"));
	INFO_PRINTF1(_L("Create positioner and the position update object"));
	
	marker = new TInt(0);
	delete marker;
	for(TInt i=1; marker; i++)
		{
		INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->ALM"));
		updateRequest->NotifyPsyRequest();
	
		__UHEAP_FAILNEXT(i);
		positioner->NotifyPositionUpdate();
		
		marker = new TInt(0);
		delete marker;
		
		StartSchedulerWaitL(KUsualTimeout, 1);
		
		if(!marker) //The leave has occured after the  NotifyPositionUpdate - not interested
			{
			break;
			}
		
		updateRequest->ExpectDefaultUpdateRequest();
		TRAPD(err, updateRequest->CheckPosUpdateOrReqReceivedL());
		if(err==KErrNone)
			{
			INFO_PRINTF1(_L("Set a position update on the bus ALM->PSY"));
			CTe_PsyResponse* simplePosUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(
					TPositionModuleInfo::ETechnologyTerminal);

			INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
			StartSchedulerWaitL(KUsualTimeout, 1);
			INFO_PRINTF1(_L("The active scheduler stopped"));
			positioner->CheckPosUpdateReceivedL(*simplePosUpdate);
			CleanupStack::PopAndDestroy(simplePosUpdate);
			}
		else if(err==KErrNotFound)
			{
			INFO_PRINTF2(_L("Start the active scheduler for %d microsec"), KUsualTimeout);
			StartSchedulerWaitL(KLongTimeoutInterval, 1);
			INFO_PRINTF1(_L("The active scheduler stopped"));
			positioner->CheckPosUpdateFailedL(KErrTimedOut);			
			}
		else //the request is received in the wrong format or some unexpected leave
			{
			User::Leave(err);
			}
		}
	
	
	INFO_PRINTF1(_L("SIMPLE POSITION UPDATE WITH OOM WHILE CALLING NotifyPositionUpdate (Scenario 3.3) END"));
		

	CleanupStack::PopAndDestroy(updateRequest);
	CleanupStack::PopAndDestroy(positioner);		
	
	}
