// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsclientstepmodselect.h
// This contains the header file for Capability Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_MODSELECT_H__
#define __CT_LBS_CLIENT_STEP_MODSELECT_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"
#include <lbs/test/lbsnetsimtest.h>

// Literals used
_LIT(KLbsClientStep_ModSelect, "LbsClientStep_ModSelect");

class CT_LbsClientStep_ModSelect : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_ModSelect();

	static CT_LbsClientStep_ModSelect* New(CT_LbsClientServer& aParent);

	virtual TVerdict doTestStepL();

private:
	CT_LbsClientStep_ModSelect(CT_LbsClientServer& aParent);

	enum TModuleType
		{
		ERandomModule,
		EAGPSModule,
		ENetworkModule
		};
	enum TOpenSesstionMethod
		{
		ENone,
		EWithCriteria,
		EWithModuleId
		};
	
	void DoModuleSelectionL(TOpenSesstionMethod aMethod, TModuleType aModuleType);
	TInt SetupNetSimL();
	
	TInt Blocking_NotifyUpdateL();
	TInt Blocking_NofifyUpdateWithCriteriaL(TPositionCriteria aCriteria);
	TInt Blocking_NotifyUpdateWithModuleIdL(TPositionModuleId aModuleId);
	TInt Blocking_NotifyUpdateCommonL();
	
	TBool VerifyLastModuleIdWasAGpsL();
	TBool VerifyLastModuleIdWasNetworkL();
		
	RLbsNetSimTest 			iNetSim; 
	
	TPositionCriteria		iCriteria; // default capability is ECapabilityHorizontal 
	TPositionQuality    	iQuality;
	TPositionSelectionOrder iSelectOrder;
	TPositionModuleId 		iModuleId;
	};

#endif //__CT_LBS_CLIENT_STEP_MODSELECT_H__
