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
// @file CT_LbsStep_Create_Verify_ModInfo.h
// This contains the header file for create verify module info step
// 
//

#ifndef __CT_LBS_CREATE_VERIFY_MOD_INFO_H__
#define __CT_LBS_CREATE_VERIFY_MOD_INFO_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_CreateVerifyModInfo, "LbsStep_CreateVerifyModInfo");

class CT_LbsStep_CreateVerifyModInfo : public CT_LbsStep
	{
public:
	static CT_LbsStep_CreateVerifyModInfo* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_CreateVerifyModInfo(CT_LbsServer& aParent);
	};

#endif //__CT_LBS_CREATE_VERIFY_MOD_INFO_H__
