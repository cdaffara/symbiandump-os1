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
// @file ctlbsstepsetuppsy.h
// This contains the header file for step to set up the psy being used in a test
// 
//

#ifndef __CT_LBS_STEP_SET_UP_PSY_H__
#define __CT_LBS_STEP_SET_UP_PSY_H__

// LBS includes.

// LBS test includes.
#include "ctlbsstep.h"

#include <w32std.h>

// Literals used
_LIT(KLbsStep_SetUpPsy, "LbsStep_SetUpPsy");
_LIT(KPsyUid, "psy_uid");


class CT_LbsStep_SetUpPsy : public CT_LbsStep
	{
public:
	~CT_LbsStep_SetUpPsy();

	static CT_LbsStep_SetUpPsy* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	TBool GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint32& aResult);
	CT_LbsStep_SetUpPsy(CT_LbsServer& aParent);
	
private:

	CActiveScheduler *iScheduler ;
	};

#endif //__CT_LBS_STEP_SET_UP_PSY_H__
