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
// @file ctlbsclientstepmodinfo.h
// This contains the header file for verify module information test step.
// 
//

#ifndef __CT_LBS_VERIFY_MOD_STATUS_H__
#define __CT_LBS_VERIFY_MOD_STATUS_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_VerifyModuleStatus, "LbsStep_VerifyModuleStatus");

class CT_LbsStep_VerifyModStatus : public CT_LbsStep
	{
public:
	static CT_LbsStep_VerifyModStatus* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_VerifyModStatus(CT_LbsServer& aParent);
	};

#endif //__CT_LBS_VERIFY_MOD_STATUS_H__
