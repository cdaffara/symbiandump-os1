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
// This contains the header file for Server Open Close Test
// 
//

#ifndef __CT_LBS_CREATE_VERIFY_POS_INFOS_H__
#define __CT_LBS_CREATE_VERIFY_POS_INFOS_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_CreateVerifyPosInfos, "LbsStep_CreateVerifyPosInfos");

class CT_LbsStep_CreateVerifyPosInfos : public CT_LbsStep
	{
public:
	static CT_LbsStep_CreateVerifyPosInfos* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsStep_CreateVerifyPosInfos(CT_LbsServer& aParent);

private:	
	CActiveScheduler *iScheduler ;

	};

#endif //__CT_LBS_CREATE_VERIFY_POS_INFOS_H__
