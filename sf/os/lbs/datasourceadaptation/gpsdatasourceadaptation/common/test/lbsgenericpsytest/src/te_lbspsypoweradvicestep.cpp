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
// The test step testing power advice scenarios with psys.
// 
//

#include <lbs/lbsadmin.h>
#include <centralrepository.h>
#include "lbsextendmoduleinfo.h"

#include "te_lbspsypoweradvicestep.h"
#include "te_lbspsyutils.h"
#include "te_lbspsyrequest.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsystaticdata.h"
#include <lbs/adaptationpsyprivatecrkeys.h>

const TUint KDelta = 1000*1000;// 1s

EXPORT_C CTe_LbsPsyPowerAdviceStep* CTe_LbsPsyPowerAdviceStep::NewL(MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsPsyPowerAdviceStep* self = new(ELeave) CTe_LbsPsyPowerAdviceStep(aPsyStaticData);
	return self;
	}
	
EXPORT_C CTe_LbsPsyPowerAdviceStep::CTe_LbsPsyPowerAdviceStep(MTe_LbsPsyStaticData* aPsyStaticData) :
CTe_PsyBaseStep(aPsyStaticData)
	{
	SetTestStepName(KLbsPowerAdviceStep);
	}

EXPORT_C CTe_LbsPsyPowerAdviceStep::~CTe_LbsPsyPowerAdviceStep() 
	{
	
	}

EXPORT_C void CTe_LbsPsyPowerAdviceStep::ReallyDoTestStepImpL()
	{
	
	INFO_PRINTF1(_L("CTe_LbsPsyPowerAdviceStep::ReallyDoTestStepL() start "));
	
	CTe_PsyRequest* updateRequest = CTe_PsyRequest::NewLC(*this);	
	CTe_PsyPositioner* positioner = MakePositionerLC(); 
	
	INFO_PRINTF1(_L("Call NotifyPositionUpdate() PSY->LM"));
	updateRequest->NotifyPsyRequest();
	positioner->NotifyPositionUpdate();
	StartSchedulerWaitL(KUsualTimeout, 1);
	
	updateRequest->ExpectDefaultUpdateRequest();
	updateRequest->UpdateRequest().NewClient() = ETrue;
	updateRequest->CheckPosUpdateOrReqReceivedL();
	updateRequest->NotifyPsyRequest();
	CTe_PsyResponse* posUpdate = CTe_PsyResponse::IssueSimplePosUpdateLC(
			TPositionModuleInfo::ETechnologyTerminal);

	// Record current time that update issued to verify power advice timer expires correctly.
	TTime updateIssuedTime;
	updateIssuedTime.HomeTime();	
	
	// wait for update at client
	StartSchedulerWaitL(KUsualTimeout, 1);
	
	positioner->CheckPosUpdateReceivedL(*posUpdate);
	
	// Request has completed, now wait for inactivity timer to fire
	TUint inactivityTimeout = GetInactivityTimeout();

	TTimeIntervalMicroSeconds microseconds;
	TTime stopTime;
	stopTime.HomeTime();
	microseconds = stopTime.MicroSecondsFrom(updateIssuedTime); 
	TInt64 timeElapsed = microseconds.Int64();
	
	if(inactivityTimeout - (timeElapsed + KDelta) > 0) // haven't reached the inactivity timeout yet
		{
		StartSchedulerWaitL(inactivityTimeout - (timeElapsed + KDelta), 0);
		INFO_PRINTF1(_L("Check that no power advice received before inactivity timeout"));
		updateRequest->CheckNothingReceivedL();	// shouldn't get power advice before the inactivity timer fires
		updateRequest->NotifyPsyRequest();
		}
	INFO_PRINTF1(_L("Check that 'standby' power advice received at/shortly after inactivity timeout"));
	updateRequest->ExpectPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
	StartSchedulerWaitL(KUsualTimeout + KDelta, 1);
	updateRequest->CheckPowerAdviceReceivedL();
	updateRequest->NotifyPsyRequest();
	
	CleanupStack::PopAndDestroy(posUpdate);
	CleanupStack::PopAndDestroy(positioner);
	StartSchedulerWaitL(KUsualTimeout, 1);
	INFO_PRINTF1(_L("Check that 'off' power advice received 'shortly' after positioner closed"));
	updateRequest->ExpectPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceOff);
	updateRequest->CheckPowerAdviceReceivedL();
	CleanupStack::PopAndDestroy(updateRequest);
	
	INFO_PRINTF1(_L("CTe_LbsPsyPowerAdviceStep::ReallyDoTestStepL() end"));
	
	if(TestStepResult()!=EPass)
		{
		User::Leave(EFail);
		}
	}
	
TUint CTe_LbsPsyPowerAdviceStep::GetInactivityTimeout()
	{
	TInt  valInt;

	CRepository* rep = CRepository::NewLC(TUid::Uid(PsyUid()));	
	User::LeaveIfError(rep->Get(KAdaptationPSYInactivityDelay, valInt));
	CleanupStack::PopAndDestroy(rep);
	
	return valInt;
	
	}
