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
// @file ctlbsrootcenrepresetstep.h
// This contains the header file for step to reset the lbsroot's central rep file
// 
//

#ifndef __CT_LBS_ROOT_CEN_REP_RESET_STEP_H__
#define __CT_LBS_ROOT_CEN_REP_RESET_STEP_H__

// LBS includes.

// LBS test includes.
#include "ctlbsagpshaivalidatestepbase.h"
#include "ctlbsagpshaiserverroot.h"

// Literals used
_LIT(KLbsRootCenRepResetStep, "LbsAGPSHAIValidateStep_RootCenRepReset");

class CT_LbsAGPSHAIValidateStep_RootCenRepReset : public CT_LbsAgpsHAIValidateStepBase
	{
public:
	~CT_LbsAGPSHAIValidateStep_RootCenRepReset();

	static CT_LbsAGPSHAIValidateStep_RootCenRepReset* New(CT_LbsAGPSHAIServerRoot& aParent);
	virtual TVerdict doTestStepL();

protected:
	CT_LbsAGPSHAIServerRoot& iParent;
	
private:
	CT_LbsAGPSHAIValidateStep_RootCenRepReset(CT_LbsAGPSHAIServerRoot& aParent);
	};

#endif //__CT_LBS_ROOT_CEN_REP_RESET_STEP_H__
