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
// This contains the header file for Module Information Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_MOD_INFO_H__
#define __CT_LBS_CLIENT_STEP_MOD_INFO_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"

// Literals used
_LIT(KLbsClientStep_ModInfo, "LbsClientStep_ModInfo");

class CT_LbsClientStep_ModInfo : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_ModInfo();

	static CT_LbsClientStep_ModInfo* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsClientStep_ModInfo(CT_LbsClientServer& aParent);
	};

#endif //__CT_LBS_CLIENT_STEP_MOD_INFO_H__
