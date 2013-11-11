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
// @file ctlbsstartlbs.h
// This contains the header file for step to start lbs system
// 
//

#ifndef __CT_LBS_STEP_START_LBS_H__
#define __CT_LBS_STEP_START_LBS_H__

// LBS includes.

// LBS test includes.
#include "ctlbsstep.h"

const TUint KLbsTestDataSrcServerUidValue = 0x10282259;

// Literals used
_LIT(KLbsStep_StartLbs, "LbsStep_StartLbs");


class CT_LbsStep_StartLbs : public CT_LbsStep
	{
public:
	~CT_LbsStep_StartLbs();

	static CT_LbsStep_StartLbs* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_StartLbs(CT_LbsServer& aParent);
	};

#endif //__CT_LBS_STEP_START_H__
