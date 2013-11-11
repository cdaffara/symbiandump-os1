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
// @file CT_LbsStep_VerifyPosInfos.h
// This contains the header file for verify position info array test step.
// 
//

#ifndef __CT_LBS_VERIFY_POS_INFOS_H__
#define __CT_LBS_VERIFY_POS_INFOS_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_VerifyPosInfos, "LbsStep_VerifyPosInfos");

class CT_LbsStep_VerifyPosInfos : public CT_LbsStep
	{
public:
	static CT_LbsStep_VerifyPosInfos* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_VerifyPosInfos(CT_LbsServer& aParent);
	};

#endif //__CT_LBS_VERIFY_POS_INFOS_H__
