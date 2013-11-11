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
// @file ctlbsstepcreateverifymoduledata.h
// This contains the header file for Server Open Close Test
// 
//

#ifndef __CT_LBS_CREATE_VERIFY_MODULE_DATA_H__
#define __CT_LBS_CREATE_VERIFY_MODULE_DATA_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_CreateVerifyModuleData, "LbsStep_CreateVerifyModuleData");

class CT_LbsStep_CreateVerifyModuleData : public CT_LbsStep
	{
public:
	static CT_LbsStep_CreateVerifyModuleData* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_CreateVerifyModuleData(CT_LbsServer& aParent);
	};

#endif //__CT_LBS_CREATE_VERIFY_MODULE_DATA_H__
