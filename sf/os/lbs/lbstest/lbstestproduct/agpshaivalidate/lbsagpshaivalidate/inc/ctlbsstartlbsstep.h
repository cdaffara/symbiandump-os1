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
// @file ctlbsstartlbsstep.h
// This contains the header file for step to start lbs system
// 
//

#ifndef __CT_LBS_START_LBS_STEP_H__
#define __CT_LBS_START_LBS_STEP_H__

// LBS includes.

// LBS test includes.
#include "ctlbsagpshaiserverroot.h"

// Literals used
_LIT(KLbsStartLbsStep, "LbsAGPSHAIValidateStep_StartLbs");

class CT_LbsAGPSHAIValidateStep_StartLbs : public CTestStep
	{
public:
	~CT_LbsAGPSHAIValidateStep_StartLbs();

	static CT_LbsAGPSHAIValidateStep_StartLbs* New(CT_LbsAGPSHAIServerRoot& aParent);
	virtual TVerdict doTestStepL();

protected:
	CT_LbsAGPSHAIServerRoot& iParent;
	
private:
	CT_LbsAGPSHAIValidateStep_StartLbs(CT_LbsAGPSHAIServerRoot& aParent);
	};

#endif //__CT_LBS_START_STEP_H__
