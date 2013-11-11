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
// The test step checking module info related functionality.
// 
//

#include "te_lbspsymodinfostep.h"
#include "te_lbspsyresponse.h"
#include "te_lbspsypositioner.h"


EXPORT_C CTe_LbsModuleInfoStep* CTe_LbsModuleInfoStep::NewL(MTe_LbsPsyStaticData* aPsyStaticData)
	{
	CTe_LbsModuleInfoStep* self = new(ELeave) CTe_LbsModuleInfoStep(aPsyStaticData);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTe_LbsModuleInfoStep::CTe_LbsModuleInfoStep(MTe_LbsPsyStaticData* aPsyStaticData) :
CTe_PsyBaseStep(aPsyStaticData)
	{
	SetTestStepName(KLbsModuleInfoStep);
	}


EXPORT_C CTe_LbsModuleInfoStep::~CTe_LbsModuleInfoStep()
	{
	}

	
void CTe_LbsModuleInfoStep::ReallyDoTestStepImpL()
	{
	CTe_PsyPositioner* positioner = MakePositionerLC();
	
	CTe_PsyPositioner* notifiedPositioner = positioner;
	if(iExtraPositioner)
		{
		notifiedPositioner = iExtraPositioner;
		}
	
	INFO_PRINTF1(_L("SUBTEST0 GETTING MODULE STATUS START"));
	
	//Scenario 0.0
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceUnknown, 
					TPositionModuleStatus::EDataQualityUnknown, ETrue);
	
	//Scenario 0.1
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceError, 
				TPositionModuleStatus::EDataQualityLoss, ETrue);
	 
	//Scenario 0.2
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceDisabled, 
				TPositionModuleStatus::EDataQualityLoss, EFalse);	
	
	//Scenario 0.3
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceInactive, 
				TPositionModuleStatus::EDataQualityPartial, ETrue);
	
	//Scenario 0.4
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceInitialising, 
				TPositionModuleStatus::EDataQualityNormal, ETrue);
	
	//Scenario 0.5
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceStandBy, 
				TPositionModuleStatus::EDataQualityPartial, ETrue);
	
	//Scenario 0.6
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceReady, 
				TPositionModuleStatus::EDataQualityNormal, ETrue);
		
	//Scenario 0.7
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceActive, 
				TPositionModuleStatus::EDataQualityLoss, ETrue);
	
	//Scenario 0.8
	RunModuleStatusTestL(*notifiedPositioner, -1, 
					TPositionModuleStatus::EDataQualityUnknown, EFalse);
	
	//Scenario 0.9
	RunModuleStatusTestL(*notifiedPositioner, TPositionModuleStatus::EDeviceReady, 
					-1, EFalse);
			
	
	INFO_PRINTF1(_L("SUBTEST0 GETTING MODULE STATUS END"));
	
	CleanupStack::PopAndDestroy(positioner);
	
	}

void CTe_LbsModuleInfoStep::RunModuleStatusTestL(CTe_PsyPositioner& aPositioner, TInt aDeviceStatus,
		TInt aQualityStatus, TBool aExpectUpdate)
	{
	TPositionModuleStatus moduleStatus;
	// set initial dynamic module status and tell interested parties
	moduleStatus.SetDeviceStatus(aDeviceStatus);
	moduleStatus.SetDataQualityStatus(aQualityStatus);
	TPositionModuleStatusEventBase::TModuleEvent occurredEventsSinceLastSet = 
					TPositionModuleStatusEventBase::EEventDeviceStatus | 
					TPositionModuleStatusEventBase::EEventDataQualityStatus;
	CTe_PsyResponse* modStatUpdate = CTe_PsyResponse::IssueModuleStatusLC(moduleStatus,
				occurredEventsSinceLastSet);
		
	StartSchedulerWaitL(KUsualTimeout, 0);
	 
	if(aExpectUpdate)
		{
		aPositioner.CheckModuleStatusReceivedL(*modStatUpdate);
		}
	else
		{
		aPositioner.CheckModuleStatusNotReceivedL();
		}
		
	CleanupStack::PopAndDestroy(modStatUpdate);
	}
	
