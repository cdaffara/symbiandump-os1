// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file CT_LbsStep_UpdateInfo.h
// This contains the header file for Server Open Close Test
// 
//

#ifndef __CT_LBS_CREATE_UPDATE_INFO_H__
#define __CT_LBS_CREATE_UPDATE_INFO_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_CreateUpdateInfo, "LbsStep_CreateUpdateInfo");

class CT_LbsStep_CreateUpdateInfo : public CT_LbsStep
	{
public:
	static CT_LbsStep_CreateUpdateInfo* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_CreateUpdateInfo(CT_LbsServer& aParent);
	};

#endif //__CT_LBS_CREATE_UPDATE_INFO_H__
