// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsclientstepignoreaccurracysettingtracking.h
// 
//

#ifndef __CT_LBS_CLIENT_STEP_IGNORE_ACCURACY_SETTING_TRACKING_H__
#define __CT_LBS_CLIENT_STEP_IGNORE_ACCURACY_SETTING_TRACKING_H__

#include <e32cmn.h>
#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"

#include <lbs/test/ctlbsagpshandler.h>

// Literals used
_LIT(KLbsClientStep_IgnoreAccuracySettingTracking, "LbsClientStep_IgnoreAccuracySettingTracking");

class CT_LbsClientStep_IgnoreAccuracySettingTracking : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_IgnoreAccuracySettingTracking();

	static CT_LbsClientStep_IgnoreAccuracySettingTracking* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();

private:
	CT_LbsClientStep_IgnoreAccuracySettingTracking(CT_LbsClientServer& aParent);
	void ConstructL();

private:
	TInt iTestCaseId;
	RPointerArray<TAny> iExpectedUpdateArray;
	};

#endif //__CT_LBS_CLIENT_STEP_IGNORE_ACCURACY_SETTING_TRACKING_H__
