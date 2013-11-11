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
// @file ctlbsclientstepmodstatus.h
// This contains the header file for Module Status Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_MOD_STAT_H__
#define __CT_LBS_CLIENT_STEP_MOD_STAT_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"

#include <lbscommon.h>

// Literals used
_LIT(KLbsClientStep_ModStatus, "LbsClientStep_ModStatus");

class CT_LbsClientStep_ModStat : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_ModStat();

	static CT_LbsClientStep_ModStat* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

private:

 	TBool DoRequestAndVerifyModStatusEventsL(TPositionModuleStatusEvent::TModuleEvent aReqEventsMask, TPositionModuleStatus& aModStatus, TPositionModuleId aModId /*= KPositionNullModuleId*/);
 	TBool DoRequestAndVerifyAGPSModStatusEventsL(TPositionModuleStatusEvent::TModuleEvent aReqEventsMask, TPositionModuleStatus& aModStatus);
	TInt DoRequestModStatusEventsWithError(TPositionModuleStatusEvent::TModuleEvent aReqEventsMask, TPositionModuleId aModId);
	
	TBool DoVerifyModStatus(TPositionModuleStatusEvent::TModuleEvent aExpectedModStatus,TPositionModuleId aModId);
	void ConstructL();
	CT_LbsClientStep_ModStat(CT_LbsClientServer& aParent);
	};

#endif //__CT_LBS_CLIENT_STEP_MOD_STAT_H__
