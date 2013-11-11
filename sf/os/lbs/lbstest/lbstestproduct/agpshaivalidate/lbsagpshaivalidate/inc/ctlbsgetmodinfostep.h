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
// @file ctlbsgetmodinfostep.h
// This contains the header file for step to get HAI module info
// 
//

#ifndef __CT_LBS_GET_MODULE_INFO_STEP_H__
#define __CT_LBS_GET_MODULE_INFO_STEP_H__

// LBS includes.

// LBS test includes.
#include "ctlbsagpshaivalidatestepbase.h"
#include "ctlbsagpshaiserverroot.h"

// Literals used
_LIT(KLbsGetModuleInfoStep, "LbsAGPSHAIValidateStep_GetModuleInfo");

class CT_LbsAGPSHAIValidateStep_GetModuleInfo : public CT_LbsAgpsHAIValidateStepBase
	{
public:
	~CT_LbsAGPSHAIValidateStep_GetModuleInfo();

	static CT_LbsAGPSHAIValidateStep_GetModuleInfo* New(CT_LbsAGPSHAIServerRoot& aParent);
	virtual TVerdict doTestStepL();

protected:
	CT_LbsAGPSHAIServerRoot& iParent;
	
private:
	void GetModuleInfoL();
	CT_LbsAGPSHAIValidateStep_GetModuleInfo(CT_LbsAGPSHAIServerRoot& aParent);
	};

#endif //__CT_GET_MODULE_INFO_STEP_H__
