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
// @file ctlbsstoplbsstep.h
// This contains the header file for step to stop lbs system
// 
//

#ifndef __CT_LBS_STOP_LBS_STEP_H__
#define __CT_LBS_STOP_LBS_STEP_H__

// LBS includes.

// LBS test includes.
#include "ctlbsagpshaiserverroot.h"

// Literals used
_LIT(KLbsStopLbsStep, "LbsAGPSHAIValidateStep_StopLbs");


class CT_LbsAGPSHAIValidateStep_StopLbs : public CTestStep
	{
public:
	~CT_LbsAGPSHAIValidateStep_StopLbs();

	static CT_LbsAGPSHAIValidateStep_StopLbs* New(CT_LbsAGPSHAIServerRoot& aParent);
	virtual TVerdict doTestStepL();

protected:
	CT_LbsAGPSHAIServerRoot& iParent;

private:
	CT_LbsAGPSHAIValidateStep_StopLbs(CT_LbsAGPSHAIServerRoot& aParent);
	};

#endif //__CT_LBS_STOP_LBS_STEP_H__
