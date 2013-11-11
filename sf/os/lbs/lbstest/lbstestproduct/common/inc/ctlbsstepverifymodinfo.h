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
// @file ctlbsstepmodinfo.h
// This contains the header file for verify module information test step.
// 
//

#ifndef __CT_LBS_VERIFY_MOD_INFO_H__
#define __CT_LBS_VERIFY_MOD_INFO_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_VerifyModInfo, "LbsStep_VerifyModInfo");

class CT_LbsStep_VerifyModInfo : public CT_LbsStep
	{
public:
	static CT_LbsStep_VerifyModInfo* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_VerifyModInfo(CT_LbsServer& aParent);
	};

#endif //__CT_LBS_VERIFY_MOD_INFO_H__
