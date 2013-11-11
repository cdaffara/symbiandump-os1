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
// The PSY base test step.
// 
//

#include <e32base.h>
#include <lbs.h>
#include <lbspositioninfo.h>
#include <lbs/epos_cpositioner.h>

#include "te_lbspsybasestep.h"
#include "EPos_PositionerConstructParams.h"
#include "te_lbspsypositioner.h"
#include "te_lbspsyrequest.h"
#include "te_lbspsystaticdata.h"

EXPORT_C CTe_PsyBaseStep::CTe_PsyBaseStep(MTe_LbsPsyStaticData* aPsyStaticData) :
iPsyStaticData(aPsyStaticData)
	{	
	}

void CTe_PsyBaseStep::BaseConstructL()
	{
	}

EXPORT_C CTe_PsyBaseStep::~CTe_PsyBaseStep()
	{
	}

EXPORT_C CTe_PsyPositioner* CTe_PsyBaseStep::MakePositionerLC(TInt aPsyUid)
	{
	return CTe_PsyPositioner::NewLC(TUid::Uid(aPsyUid), *this, *this);	
	}

EXPORT_C CTe_PsyPositioner* CTe_PsyBaseStep::MakePositionerLC()
	{
	return CTe_PsyPositioner::NewLC(TUid::Uid(iPsyUid), *this, *this);	
	}

EXPORT_C void CTe_PsyBaseStep::OnRunLCall(const CTe_Active& /*aActive*/)
	{
	if(iAsyncCalls<0)
		{
		SetTestStepResult(EFail);
		iSchedulerWait->AsyncStop();
		}
	}

EXPORT_C void CTe_PsyBaseStep::StartSchedulerWaitL(TUint aTimeout, TUint aAsyncCallsExpected)
	{
	iAsyncCalls = aAsyncCallsExpected;
	iSchedulerWait->Start(aTimeout);
	
	if(TestStepResult()!=EPass)
		{
		User::Leave(EFail);
		}
	}

EXPORT_C void CTe_PsyBaseStep::WaitCheckCancelReceivedL(CTe_PsyRequest& aUpdateRequest)
	{
	StartSchedulerWaitL(KUsualTimeout, 1);
	aUpdateRequest.ExpectDefaultCancelRequest();
	aUpdateRequest.CheckPosUpdateOrReqReceivedL();
	}

EXPORT_C void CTe_PsyBaseStep::WaitCheckNotifyReqReceivedL(CTe_PsyRequest& aUpdateRequest)
	{
	StartSchedulerWaitL(KUsualTimeout, 1);
	aUpdateRequest.ExpectDefaultUpdateRequest();
	aUpdateRequest.CheckPosUpdateOrReqReceivedL();
	aUpdateRequest.NotifyPsyRequest();
	}

EXPORT_C void CTe_PsyBaseStep::ReallyDoTestStepL()
	{
	MTe_LbsPsyStaticData::SetStaticL(iPsyStaticData);
	
	User::LeaveIfError(GetIntFromConfig(ConfigSection(), KPsyUid, iPsyUid));
	
	TBool extraPositioner = EFalse;
	User::LeaveIfError(GetBoolFromConfig(ConfigSection(), KExtraPositioner, extraPositioner));
	if(extraPositioner)
		{
		iExtraPositioner = MakePositionerLC();
		ReallyDoTestStepImpL();
		CleanupStack::PopAndDestroy(iExtraPositioner);
		}
	else
		{
		ReallyDoTestStepImpL();
		}
	}

TInt CTe_PsyBaseStep::PsyUid()
	{
	return iPsyUid;
	}
