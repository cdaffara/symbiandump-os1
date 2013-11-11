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
// @file ctlbsstepsetupstandaloneprivacyandsimplectlr.h
// This contains the header file for Set Autonomous Mode Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_STEP_SETUP_STANDALONE_PRIVACY_AND_SIMPLE_CTRL_H__
#define __CT_LBS_STEP_SETUP_STANDALONE_PRIVACY_AND_SIMPLE_CTRL_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

_LIT(KLbsStep_SetupStandalonePrivacyAndSimpleCtlr, "LbsStep_SetupStandalonePrivacyAndSimpleCtlr"); 



class CT_LbsStep_SetupStandalonePrivacyAndSimpleCtlr : public CT_LbsStep
	{
public:
	static CT_LbsStep_SetupStandalonePrivacyAndSimpleCtlr* New(CT_LbsServer& aParent);
	TVerdict doTestStepL();

private:
	CT_LbsStep_SetupStandalonePrivacyAndSimpleCtlr(CT_LbsServer& aParent);
	};

#endif // __CT_LBS_STEP_SETUP_STANDALONE_PRIVACY_AND_SIMPLE_CTRL_H__